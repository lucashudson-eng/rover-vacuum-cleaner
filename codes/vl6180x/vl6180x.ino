#include <DFRobot_VL6180X.h>

DFRobot_VL6180X VL6180X;

const uint8_t NUM_SENSORS = 2;

volatile bool interruptFlag = false;

void interruptHandler() {
  interruptFlag = true;
}

void TCA9548A(uint8_t bus) {
  Wire.beginTransmission(0x70);
  Wire.write(1 << bus);
  Wire.endTransmission();
}

void setupSensor(uint8_t bus) {
  TCA9548A(bus);
  delay(10);
  
  while (!VL6180X.begin()) {
    Serial.print("Sensor on bus ");
    Serial.print(bus);
    Serial.println(" not found! Check connection.");
    delay(1000);
  }
  
  // Enable interrupt with LOW output (pull-up resistor already in place)
  VL6180X.setInterrupt(VL6180X_LOW_INTERRUPT);
  
  // Configure interrupt to trigger when value > threshold_high
  VL6180X.rangeConfigInterrupt(VL6180X_LEVEL_HIGH);
  
  // Set measurement period (0 = 10ms = 100Hz, max 2550ms)
  VL6180X.rangeSetInterMeasurementPeriod(0);
  
  // Set threshold: interrupt when reading > 60mm
  VL6180X.setRangeThresholdValue(0, 60);
  
  // Start continuous range measuring mode
  VL6180X.rangeStartContinuousMode();
  
  Serial.print("Sensor on bus ");
  Serial.print(bus);
  Serial.println(" configured!");
}

void setup() {
  Serial.begin(115200);
  Wire.begin();
  
  // Configure all 3 sensors
  for (uint8_t i = 0; i < NUM_SENSORS; i++) {
    setupSensor(i);
  }
  
  // Attach interrupt handler (shared by all sensors)
  attachInterrupt(digitalPinToInterrupt(A2), interruptHandler, RISING);
  
  Serial.println("All sensors ready!");
}

void loop() {
  if (interruptFlag) {
    interruptFlag = false;
    
    uint8_t activeInterrupts = 0;
    uint8_t ranges[NUM_SENSORS];
    bool hasError[NUM_SENSORS];
    
    // Check all sensors for interrupt status
    for (uint8_t i = 0; i < NUM_SENSORS; i++) {
      TCA9548A(i);
      delay(1);
      
      if (VL6180X.rangeGetInterruptStatus() == VL6180X_LEVEL_HIGH) {
        activeInterrupts++;
        uint8_t range = VL6180X.rangeGetMeasurement();
        uint8_t status = VL6180X.getRangeResult();
        VL6180X.clearRangeInterrupt();
        
        ranges[i] = range;
        hasError[i] = (status != VL6180X_NO_ERR);
      } else {
        // Read measurement even if no interrupt (for display)
        uint8_t range = VL6180X.rangeGetMeasurement();
        uint8_t status = VL6180X.getRangeResult();
        
        ranges[i] = range;
        hasError[i] = (status != VL6180X_NO_ERR);
      }
    }

    for (uint8_t i = 0; i < NUM_SENSORS; i++) {
      TCA9548A(i);
      delay(1);
      VL6180X.clearRangeInterrupt();
    }
    
    // If at least 2 sensors have active interrupts, print all measurements
    if (activeInterrupts >= 2) {
      Serial.println("--- Measurements (2+ interrupts) ---");
      for (uint8_t i = 0; i < NUM_SENSORS; i++) {
        Serial.print("Sensor ");
        Serial.print(i);
        Serial.print(": ");
        if (hasError[i]) {
          Serial.println("ERROR");
        } else {
          Serial.print(ranges[i]);
          Serial.println(" mm");
        }
      }
      Serial.println("-----------------------------------");
    }
  }
}
