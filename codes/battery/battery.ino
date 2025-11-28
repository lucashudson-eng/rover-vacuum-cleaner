#include <Arduino.h>

// === ADC pins (change to your actual pins) ===
const int ADC1_PIN = A0;   // First ADC channel
const int ADC2_PIN = A1;   // Second ADC channel

// === Calibration for ADC1 ===
// V = a1 * RAW1 + b1
const float a1 = 0.0015314030;
const float b1 = 1.9424100135;

// === Calibration for ADC2 ===
// Put the correct values here after calibration
const float a2 = 0.0007493703;   // example value
const float b2 = 1.0988619939;   // example value

// === Moving average window size ===
const int N = 10;

// Buffers for ADC1
float buffer1[N];
int index1 = 0;
bool full1 = false;

// Buffers for ADC2
float buffer2[N];
int index2 = 0;
bool full2 = false;

void setup() {
  Serial.begin(115200);
  delay(2000);

  analogReadResolution(12); // 0–4095 resolution

  // Initialize buffers
  for (int i = 0; i < N; i++) {
    buffer1[i] = 0.0;
    buffer2[i] = 0.0;
  }
}

float computeMovingAverage(float buffer[], int count) {
  float sum = 0;
  for (int i = 0; i < count; i++) {
    sum += buffer[i];
  }
  return sum / count;
}

void loop() {
  // === Read raw ADC values ===
  int raw1 = analogRead(ADC1_PIN);
  int raw2 = analogRead(ADC2_PIN);

  // === Apply calibration ===
  float v1 = a1 * raw1 + b1;
  float v2 = a2 * raw2 + b2;

  // === Store into moving average buffers ===
  // ADC1 buffer
  buffer1[index1] = v1;
  index1++;
  if (index1 >= N) { index1 = 0; full1 = true; }

  // ADC2 buffer
  buffer2[index2] = v2;
  index2++;
  if (index2 >= N) { index2 = 0; full2 = true; }

  // === Compute averages ===
  int count1 = full1 ? N : index1;
  int count2 = full2 ? N : index2;

  float avg1 = computeMovingAverage(buffer1, count1);
  float avg2 = computeMovingAverage(buffer2, count2);

  // === Print formatted output ===
  Serial.print("RAW1=");
  Serial.print(raw1);
  Serial.print("  V1=");
  Serial.print(v1, 4);
  Serial.print("  AVG1=");
  Serial.print(avg1, 4);

  Serial.print("   ||   ");

  Serial.print("RAW2=");
  Serial.print(raw2);
  Serial.print("  V2=");
  Serial.print(v2, 4);
  Serial.print("  AVG2=");
  Serial.println(avg2, 4);

  delay(200); // adjust as needed
}
