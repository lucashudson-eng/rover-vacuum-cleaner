/*
 * IMU 9DOF with Madgwick AHRS for Arduino ESP32
 * Uses MPU9250 IMU with Madgwick filter for orientation estimation
 * Custom MPU9250 implementation using Wire library
 * 
 * Hardware:
 * - ESP32 Dev Board
 * - MPU9250 9DOF IMU (Accelerometer + Gyroscope + Magnetometer)
 * 
 * Pin Configuration:
 * - SDA: GPIO 21
 * - SCL: GPIO 22
 * - INT: GPIO 23 (optional, for interrupt-driven reading)
 * 
 * Libraries Required:
 * - MadgwickAHRS (by Paul Stoffregen)
 * - Wire (built-in)
 */

#include <Wire.h>
#include <MadgwickAHRS.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

// ----- I2C CONFIGURATION -----
#define I2C_SDA_PIN 21
#define I2C_SCL_PIN 22
#define I2C_FREQ 400000

// ----- MPU9250 I2C ADDRESSES -----
#define MPU9250_ADDR 0x68
#define AK8963_ADDR 0x0C

// ----- MPU9250 REGISTERS -----
#define PWR_MGMT_1 0x6B        // Power management 1 - wake up device
#define CONFIG 0x1A             // Digital Low Pass Filter (DLPF) configuration
#define GYRO_CONFIG 0x1B        // Gyroscope configuration - range
#define ACCEL_CONFIG 0x1C       // Accelerometer configuration - range
#define SMPLRT_DIV 0x19         // Sample rate divider
#define INT_PIN_CFG 0x37        // Interrupt pin configuration - I2C bypass
#define INT_ENABLE 0x38         // Interrupt enable register
#define ACCEL_XOUT_H 0x3B       // Accelerometer data start (14 bytes: accel + temp + gyro)

// Magnetometer registers (AK8963)
#define ST1 0x02                // Status 1 - data ready flag
#define HXL 0x03                // Magnetometer data start (7 bytes: ST1 + 6 data + ST2)
#define CNTL1 0x0A               // Control 1 - measurement mode

// ----- INTERRUPT CONFIGURATION -----
#define INT_PIN 23

// ----- SAMPLING CONFIGURATION -----
#define SAMPLE_RATE_HZ 100
#define MICROS_PER_READING (1000000 / SAMPLE_RATE_HZ)

// ----- MPU9250 CONFIGURATION VALUES -----
#define GYRO_FS_250 0x00
#define GYRO_FS_500 0x08
#define GYRO_FS_1000 0x10
#define GYRO_FS_2000 0x18

#define ACCEL_FS_2 0x00
#define ACCEL_FS_4 0x08
#define ACCEL_FS_8 0x10
#define ACCEL_FS_16 0x18

// ----- SCALE FACTORS -----
#define GYRO_SCALE_250 (250.0f / 32768.0f)
#define GYRO_SCALE_500 (500.0f / 32768.0f)
#define GYRO_SCALE_1000 (1000.0f / 32768.0f)
#define GYRO_SCALE_2000 (2000.0f / 32768.0f)

#define ACCEL_SCALE_2 (2.0f / 32768.0f)
#define ACCEL_SCALE_4 (4.0f / 32768.0f)
#define ACCEL_SCALE_8 (8.0f / 32768.0f)
#define ACCEL_SCALE_16 (16.0f / 32768.0f)

// ----- GLOBAL VARIABLES -----
Madgwick filter;

// Queue for communication between interrupt and task
QueueHandle_t mpu_queue = NULL;

// Timing variables
unsigned long lastReadTime = 0;

// Data storage
float ax, ay, az;  // Accelerometer (g)
float gx, gy, gz;  // Gyroscope (deg/s)
float mx, my, mz;  // Magnetometer (µT)

// Orientation results
float roll, pitch, yaw;

// MPU9250 configuration
float gyroScale = GYRO_SCALE_250;
float accelScale = ACCEL_SCALE_2;
float magScale = 0.15f;  // µT per LSB

// ----- INTERRUPT HANDLER -----
void IRAM_ATTR mpu_intr_handler() {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    uint32_t dummy = 1;
    xQueueSendFromISR(mpu_queue, &dummy, &xHigherPriorityTaskWoken);
    if (xHigherPriorityTaskWoken) {
        portYIELD_FROM_ISR();
    }
}

// ----- INTERRUPT PIN CONFIGURATION -----
bool setup_interrupt_pin() {
    pinMode(INT_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(INT_PIN), mpu_intr_handler, RISING);
    return true;
}

// ----- I2C COMMUNICATION FUNCTIONS -----
bool writeRegister(uint8_t deviceAddr, uint8_t regAddr, uint8_t data) {
    Wire.beginTransmission(deviceAddr);
    Wire.write(regAddr);
    Wire.write(data);
    return (Wire.endTransmission() == 0);
}

bool readRegister(uint8_t deviceAddr, uint8_t regAddr, uint8_t* data, uint8_t length) {
    Wire.beginTransmission(deviceAddr);
    Wire.write(regAddr);
    if (Wire.endTransmission(false) != 0) {
        return false;
    }
    
    Wire.requestFrom(deviceAddr, length);
    if (Wire.available() != length) {
        return false;
    }
    
    for (uint8_t i = 0; i < length; i++) {
        data[i] = Wire.read();
    }
    return true;
}

uint8_t readRegister(uint8_t deviceAddr, uint8_t regAddr) {
    uint8_t data;
    readRegister(deviceAddr, regAddr, &data, 1);
    return data;
}

// ----- MPU9250 INITIALIZATION -----
bool mpu9250_init() {
    // Wake up MPU9250
    if (!writeRegister(MPU9250_ADDR, PWR_MGMT_1, 0x00)) {
        Serial.println("Failed to wake up MPU9250");
        return false;
    }
    delay(100);
    
    // Configure MPU9250 with default settings
    // Accelerometer range: ±2g
    if (!writeRegister(MPU9250_ADDR, ACCEL_CONFIG, ACCEL_FS_2)) {
        Serial.println("Failed to configure accelerometer");
        return false;
    }
    accelScale = ACCEL_SCALE_2;
    
    // Gyroscope range: ±250 dps
    if (!writeRegister(MPU9250_ADDR, GYRO_CONFIG, GYRO_FS_250)) {
        Serial.println("Failed to configure gyroscope");
        return false;
    }
    gyroScale = GYRO_SCALE_250;
    
    // DLPF: 20Hz
    if (!writeRegister(MPU9250_ADDR, CONFIG, 0x04)) {
        Serial.println("Failed to configure DLPF");
        return false;
    }
    
    // Sample rate: 100Hz
    if (!writeRegister(MPU9250_ADDR, SMPLRT_DIV, 0x09)) {
        Serial.println("Failed to configure sample rate");
        return false;
    }
    
    // Enable data ready interrupt
    if (!writeRegister(MPU9250_ADDR, INT_ENABLE, 0x01)) {
        Serial.println("Failed to enable interrupt");
        return false;
    }
    
    Serial.println("MPU9250 initialized successfully");
    return true;
}

// ----- AK8963 MAGNETOMETER INITIALIZATION -----
bool ak8963_init() {
    // Enable I2C bypass to access magnetometer directly
    if (!writeRegister(MPU9250_ADDR, INT_PIN_CFG, 0x02)) {
        Serial.println("Failed to enable I2C bypass");
        return false;
    }
    delay(10);
    
    // Set magnetometer to 16-bit output, continuous mode 2
    if (!writeRegister(AK8963_ADDR, CNTL1, 0x16)) {
        Serial.println("Failed to configure magnetometer");
        return false;
    }
    delay(10);
    
    // Use default magnetometer scale
    magScale = 0.15f;  // µT per LSB
    
    Serial.println("AK8963 initialized successfully");
    return true;
}

// ----- READ IMU DATA (ACCEL + GYRO) -----
bool readIMU(float* ax, float* ay, float* az, float* gx, float* gy, float* gz) {
    uint8_t data[14];  // 6 accel + 2 temp + 6 gyro
    if (!readRegister(MPU9250_ADDR, ACCEL_XOUT_H, data, 14)) {
        return false;
    }
    
    // Accelerometer data (bytes 0-5)
    int16_t rawAX = (int16_t)((data[0] << 8) | data[1]);
    int16_t rawAY = (int16_t)((data[2] << 8) | data[3]);
    int16_t rawAZ = (int16_t)((data[4] << 8) | data[5]);
    
    // Gyroscope data (bytes 8-13, skip temp bytes 6-7)
    int16_t rawGX = (int16_t)((data[8] << 8) | data[9]);
    int16_t rawGY = (int16_t)((data[10] << 8) | data[11]);
    int16_t rawGZ = (int16_t)((data[12] << 8) | data[13]);
    
    *ax = rawAX * accelScale;
    *ay = rawAY * accelScale;
    *az = rawAZ * accelScale;
    *gx = rawGX * gyroScale;
    *gy = rawGY * gyroScale;
    *gz = rawGZ * gyroScale;
    
    return true;
}

// ----- READ MAGNETOMETER DATA -----
bool readMag(float* mx, float* my, float* mz) {
    uint8_t data[7];  // ST1 + 6 data bytes + ST2
    if (!readRegister(AK8963_ADDR, ST1, data, 7)) {
        return false;
    }
    
    // Check if data is ready (ST1 bit 0)
    if (!(data[0] & 0x01)) {
        return false;  // No new data
    }
    
    // Check for magnetic sensor overflow (ST2 bit 3)
    if (data[6] & 0x08) {
        return false;  // Magnetic sensor overflow
    }
    
    // Read magnetometer data (LSB first for magnetometer)
    int16_t rawX = (int16_t)((data[2] << 8) | data[1]);
    int16_t rawY = (int16_t)((data[4] << 8) | data[3]);
    int16_t rawZ = (int16_t)((data[6] << 8) | data[5]);
    
    *mx = rawX * magScale;
    *my = rawY * magScale;
    *mz = rawZ * magScale;
    
    return true;
}

// ----- FREERTOS TASK FOR IMU PROCESSING -----
void imu_task(void *parameter) {
    Serial.println("IMU task started");
    uint32_t dummy;
    
    while (true) {
        // Wait for interrupt signal (same as ESP-IDF)
        if (xQueueReceive(mpu_queue, &dummy, portMAX_DELAY)) {
            unsigned long now = micros();
            float dt = (now - lastReadTime) / 1000000.0f;
            lastReadTime = now;
            
            // Read IMU data (accelerometer + gyroscope) in one transaction
            bool dataValid = readIMU(&ax, &ay, &az, &gx, &gy, &gz);
            
            // Read magnetometer data
            bool magValid = readMag(&mx, &my, &mz);
            
            if (dataValid) {
                // Update Madgwick filter
                if (magValid) {
                    // Use full 9DOF data (IMU + magnetometer)
                    filter.update(gx, gy, gz, ax, ay, az, mx, my, mz);
                } else {
                    // Fallback to IMU-only mode
                    filter.updateIMU(gx, gy, gz, ax, ay, az);
                }
                
                // Get Euler angles (in degrees)
                roll = filter.getRoll();
                pitch = filter.getPitch();
                yaw = filter.getYaw();
                
                // Calculate actual sampling frequency
                float freq = (dt > 0) ? (1.0f / dt) : 0;
                
                // Display results
                Serial.print("f: ");
                Serial.print(freq, 2);
                Serial.print(" Hz  Roll: ");
                Serial.print(roll, 2);
                Serial.print("  Pitch: ");
                Serial.print(pitch, 2);
                Serial.print("  Yaw: ");
                Serial.print(yaw, 2);
                Serial.print("  Mag: ");
                Serial.println(magValid ? "OK" : "FAIL");
            } else {
                Serial.println("Failed to read IMU data");
            }
        }
    }
}

// ----- ARDUINO SETUP -----
void setup() {
    Serial.begin(115200);
    Serial.println("IMU 9DOF with Madgwick AHRS - Starting...");
    
    // Initialize I2C
    Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
    Wire.setClock(I2C_FREQ);
    Serial.println("I2C initialized");
    
    // Initialize MPU9250
    if (!mpu9250_init()) {
        Serial.println("Failed to initialize MPU9250!");
        while (1) {
            delay(1000);
        }
    }
    
    // Initialize AK8963 magnetometer
    if (!ak8963_init()) {
        Serial.println("Failed to initialize AK8963 magnetometer!");
        // Continue without magnetometer
    }
    
    // Create queue for interrupt communication
    mpu_queue = xQueueCreate(10, sizeof(uint32_t));
    if (mpu_queue == NULL) {
        Serial.println("Failed to create queue");
        return;
    }
    Serial.println("Queue created");
    
    // Configure interrupt pin
    if (!setup_interrupt_pin()) {
        Serial.println("Failed to configure interrupt pin");
        return;
    }
    Serial.println("Interrupt pin configured");
    
    // Initialize Madgwick filter
    filter.begin(SAMPLE_RATE_HZ);
    Serial.println("Madgwick filter initialized");
    
    // Initialize timing
    lastReadTime = micros();
    
    // Create FreeRTOS task for IMU processing
    xTaskCreate(
        imu_task,              // Task function
        "IMUTask",             // Task name
        4096,                  // Stack size (words)
        NULL,                  // Task parameters
        1,                     // Task priority
        NULL                   // Task handle
    );
    
    Serial.println("IMU task created");
    Serial.println("System ready!");
}

// ----- ARDUINO MAIN LOOP -----
void loop() {
    // Main loop is free for other tasks
    // IMU processing is handled by FreeRTOS task
    delay(1000); // Small delay to prevent watchdog reset
}
