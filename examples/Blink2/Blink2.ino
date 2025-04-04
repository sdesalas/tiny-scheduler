#include <TinyScheduler.h>


TinyScheduler msScheduler = TinyScheduler::millis();

const int LED_PIN = LED_BUILTIN;
int LED_VALUE = 0;

void toggleLed() {
  LED_VALUE = (LED_VALUE + 1) % 2;
  digitalWrite(LED_PIN, LED_VALUE);
}

void setup() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LED_VALUE);
  msScheduler
    .repeat(5, 250, 500, toggleLed)
    .repeat(5, 0, 500, toggleLed)
    .every(3000, 1000, toggleLed);
  msScheduler.debug(Serial);
}

void loop() {
  msScheduler.loop();
}