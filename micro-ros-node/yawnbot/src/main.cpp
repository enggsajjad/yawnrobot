#include <Arduino.h>
#include <pins_arduino.h>
#include <micro_ros_platformio.h>

#include <rcl/rcl.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>

#include <std_msgs/msg/int32.h>

extern"C"{
#include <rmw_microros/rmw_microros.h>
}
#include <GyverOLED.h>

const uint LED_PIN = 25;
// Used for generating interrupts using CLK signal
const int PinA = 28;//D4;
// Used for reading DT signal
const int PinB = 27;//D5;
// Keep track of last rotary value
int lastCount = 25;
// Updated by the ISR (Interrupt Service Routine)
volatile int virtualPosition = 25;
volatile int realPosition = virtualPosition;
bool pulse;
const uint ENC_MAX = 100;
const uint ENC_MIN = 0;

GyverOLED<SSH1106_128x64> oled;

rcl_publisher_t yawn_pub;
std_msgs__msg__Int32 yawn_msg,yawn_target_msg;//Sajjad

enum states {
  WAITING_AGENT,
  AGENT_AVAILABLE,
  AGENT_CONNECTED,
  AGENT_DISCONNECTED
} state;

rcl_timer_t timer;
rcl_node_t node;
rcl_allocator_t allocator;
rclc_support_t support;
rclc_executor_t executor;

const char * servo_target_topic_name = "micro_ros_platformio_node_subscriber";
rcl_subscription_t servo_sub;





// ------------------------------------------------------------------
// ISR
// ------------------------------------------------------------------
void isrA ()  {

  //Serial.println("A");
  static unsigned long lastInterruptTime = 0;
  unsigned long interruptTime = millis();

  // If interrupts come faster than 5ms, assume it's a bounce and ignore
  if (interruptTime - lastInterruptTime > 20) {

    byte pinB = digitalRead(PinB);
    if (pinB == 1)
    {
      virtualPosition++ ; // Could be -5 or -10
    }
    else {
      virtualPosition-- ; // Could be +5 or +10
    }

    // Restrict value from 0 to +100
    virtualPosition = min(ENC_MAX, max(ENC_MIN, virtualPosition));
  }

  // Keep track of when we were here last (no more than every 5ms)
  lastInterruptTime = interruptTime;
}

void isrB ()  {
  //Serial.println("B");
  static unsigned long lastInterruptTime = 0;
  unsigned long interruptTime = millis();

  // If interrupts come faster than 5ms, assume it's a bounce and ignore
  if (interruptTime - lastInterruptTime > 20) {

    byte pinA = digitalRead(PinA);
    if (pinA == 0)
    {
      virtualPosition++ ; // Could be -5 or -10
    }
    else{
      virtualPosition-- ; // Could be +5 or +10
    }

    // Restrict value from 0 to +100
virtualPosition = min(ENC_MAX, max(ENC_MIN, virtualPosition));
}
  // Keep track of when we were here last (no more than every 5ms)
  lastInterruptTime = interruptTime;
}

void subscription_callback(const void * msgin){
    //const ros2_servo__msg__Servo * msg = (const ros2_servo__msg__Servo *)msgin;
    //servo_device[msg->servo].goRad(msg->radians);
    const std_msgs__msg__Int32 * msg = (const std_msgs__msg__Int32 *)msgin;
    //digitalWrite(LED_PIN, msg->data > 10 ?  HIGH : LOW );
    realPosition = msg->data;
    //pulse = ! pulse;
    //digitalWrite(LED_PIN, pulse);

    oled.setCursorXY(88, 12);
    oled.print("   ");
    oled.update();
    oled.setCursorXY(88, 12);
    oled.print(String(realPosition));
    oled.update();
}



void timer_callback(rcl_timer_t *timer, int64_t last_call_time){

	/*for (uint8_t i = 0; i < NUM_SERVOS; i++){
		yawn_msg.servo = i;
		yawn_msg.radians = servo_device[i].getRad();
		rcl_ret_t ret = rcl_publish(&yawn_pub, &yawn_msg, NULL);
	}*/
RCLC_UNUSED(last_call_time);
  if (timer != NULL) {
    rcl_ret_t ret = rcl_publish(&yawn_pub, &yawn_msg, NULL);
    yawn_msg.data = virtualPosition;//virtualPosition;
  }
}

bool pingAgent(){
    // Wait for agent successful ping for 2 minutes.
    const int timeout_ms = 100;
    const uint8_t attempts = 1;

    rcl_ret_t ret = rmw_uros_ping_agent(timeout_ms, attempts);

    oled.setCursorXY(120, 54); 
    if (ret != RCL_RET_OK){
      digitalWrite(LED_PIN, LOW );
      oled.print("0");
    	return false;
    } else {
      digitalWrite(LED_PIN, HIGH );
      oled.print("1");
    }
    oled.update();
    return true;
}

void createEntities(){
	allocator = rcl_get_default_allocator();

	rclc_support_init(&support, 0, NULL, &allocator);

	rclc_node_init_default(&node, "pico_node", "", &support);

	rclc_publisher_init_default(
			&yawn_pub,
			&node,
			ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
			"micro_ros_platformio_node_publisher");

	rclc_timer_init_default(
		&timer,
		&support,
		RCL_MS_TO_NS(1000),
		timer_callback);

	const rosidl_message_type_support_t * type_support =
	    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32);
	rclc_subscription_init_default(
	        &servo_sub,
	        &node,
	        type_support,
	        servo_target_topic_name);

	rclc_executor_init(&executor, &support.context, 2, &allocator);
	rclc_executor_add_timer(&executor, &timer);

	rclc_executor_add_subscription(&executor,
			&servo_sub,
			&yawn_target_msg,
			&subscription_callback,
			ON_NEW_DATA);
}

void destroyEntities(){
	rmw_context_t * rmw_context = rcl_context_get_rmw_context(&support.context);
	(void) rmw_uros_set_context_entity_destroy_session_timeout(rmw_context, 0);

	rcl_publisher_fini(&yawn_pub, &node);
	rcl_subscription_fini(&servo_sub, &node);
	rcl_timer_fini(&timer);
	rclc_executor_fini(&executor);
	rcl_node_fini(&node);
	rclc_support_fini(&support);
}




void setup()
{

  // Configure serial transport
  Serial.begin(115200);
  set_microros_serial_transports(Serial);
  delay(2000);
// Rotary pulses are INPUTs
  pinMode(PinA, INPUT_PULLUP);
  pinMode(PinB, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW );

  // Attach the routine to service the interrupts
  attachInterrupt(PinA, isrA, LOW);
  attachInterrupt(PinB, isrB, LOW);

  oled.init(4,5);  
  oled.clear();   
  oled.update(); 
  oled.setCursorXY(18, 14);   
  oled.print("Hello Encoder....");
  oled.setCursorXY(18, 30);   
  oled.print("FZI Karlsruhe");
  oled.rect(0,0,127,63,OLED_STROKE);
  oled.update();

  delay(2000);
  oled.clear();   
  oled.update(); 
  oled.setCursorXY(28, 12);   
  oled.print("Real FB:   ");
  oled.setCursorXY(28, 26);   
  oled.print("Current:   ");
  oled.setCursorXY(28, 40);   
  oled.print("Direction: ");
  oled.rect(0,0,127,63,OLED_STROKE);

  oled.setCursorXY(88, 12);
  oled.print(String(realPosition));
  oled.setCursorXY(88, 26);
  oled.print(String(virtualPosition));
  oled.setCursorXY(88, 40);
  oled.print("REST");
  oled.update();


  allocator = rcl_get_default_allocator();
  state = WAITING_AGENT;
}

void loop(void)
{
    	switch (state) {
    	    case WAITING_AGENT:
    	      state = pingAgent() ? AGENT_AVAILABLE : WAITING_AGENT;
    	      break;
    	    case AGENT_AVAILABLE:
    	      createEntities();
    	      state = AGENT_CONNECTED ;
    	      break;
    	    case AGENT_CONNECTED:
    	      state = pingAgent() ? AGENT_CONNECTED : AGENT_DISCONNECTED;
    	      if (state == AGENT_CONNECTED) {
    	        rclc_executor_spin_some(&executor, RCL_MS_TO_NS(100));
    	      }
    	      break;
    	    case AGENT_DISCONNECTED:
    	      destroyEntities();
    	      state = WAITING_AGENT;
    	      break;
    	    default:
    	      break;
    	  }

        //pulse = ! pulse;
        //gpio_put(PULSE_LED, pulse);
  if (virtualPosition != lastCount) {

    // Write out to serial monitor the value and direction
    //Serial.print(virtualPosition > lastCount ? "Up:" : "Down:");
    //Serial.println(virtualPosition);

  oled.setCursorXY(88, 12);
  oled.print("  ");
  oled.setCursorXY(88, 26);
  oled.print("  ");
  oled.setCursorXY(88, 40);
  oled.print("   ");
  oled.update();
  oled.setCursorXY(88, 12);
  oled.print(String(realPosition));
  oled.setCursorXY(88, 26);
  oled.print(String(virtualPosition));
  oled.setCursorXY(88, 40);
  oled.print(virtualPosition > lastCount ? "Up  " : "Down");
  oled.update();
  // Keep track of this new value
  lastCount = virtualPosition ;
  
  delay(10);
  }
}
