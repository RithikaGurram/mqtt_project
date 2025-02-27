#include <Arduino.h>

// put function declarations here:


void setup() {
  Serial.begin(921600);
  // put your setup code here, to run once:
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.println("Setup ended");
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(500);
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.println("LED ON in loop");
  delay(500);
  digitalWrite(LED_BUILTIN, LOW);
}

