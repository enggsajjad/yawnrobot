#include <Encoder.h>

// Change these two numbers to the pins connected to your encoder.
//   Best Performance: both pins have interrupt capability
//   Good Performance: only the first pin has interrupt capability
//   Low Performance:  neither pin has interrupt capability
Encoder myEnc(3, 4);
//   avoid using pins with LEDs attached

const byte switchPin = 2;

void setup() {
  Serial.begin(115200);
  pinMode(switchPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(switchPin), handleSwitch, RISING);
  Serial.println("Library Encoder Test");
}

long oldPosition  = -999;

void loop() {
  long newPosition = myEnc.read();
  if (newPosition != oldPosition && newPosition %  2 == 0) {
    oldPosition = newPosition;
    Serial.println(newPosition / 2);
  }
}

void handleSwitch() {
  Serial.println("Switch pressed");
}
