#include <Arduino.h>

const int adcPin = A0;

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);
}

void loop() {
  uint16_t raw = analogRead(adcPin);
  uint32_t mv  = analogReadMilliVolts(adcPin);

  Serial.print(raw);
  Serial.print(",");
  Serial.println(mv);

  delay(10);
}
