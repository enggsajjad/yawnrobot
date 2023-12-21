#include <GyverOLED.h>

GyverOLED<SSH1106_128x64> oled;

void setup() {
  oled.init(4,5);  
  
  oled.clear();   
  oled.update(); 

  oled.setCursorXY(28, 16);   
  oled.print("Hello World....");
  oled.setCursorXY(28, 36);   
  oled.print("FZI Karlsruhe");
  oled.rect(0,0,127,63,OLED_STROKE);
  oled.update();
}

void loop() {    
}
