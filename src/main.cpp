#include <Arduino.h>

#define LED 13  // The pin the LED is connected to

void setup() {
  pinMode(LED, OUTPUT); // Declare the LED as an output
}

void loop() {
  digitalWrite(LED, HIGH);
  delay(20);
  digitalWrite(LED, LOW);
  delay(20);
}
