/*
 * IMU 6DOF with Madgwick AHRS for Arduino ESP32
 * Uses MPU6050 IMU with Madgwick filter for orientation estimation
 * Custom MPU6050 implementation using Wire library
 * 
 * Hardware:
 * - ESP32 Dev Board
 * - MPU6050 6DOF IMU (Accelerometer + Gyroscope)
 * 
 * Pin Configuration:
 * - SDA: GPIO A4
 * - SCL: GPIO A5
 * - INT: GPIO A3 (optional, for interrupt-driven reading)
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
#include "MPU6050.h"

// ----- I2C CONFIGURATION -----
#define I2C_SDA_PIN A4
#define I2C_SCL_PIN A5
#define I2C_FREQ 400000

// ----- INTERRUPT CONFIGURATION -----
#define INT_PIN A3

// ----- SAMPLING CONFIGURATION -----
#define SAMPLE_RATE_HZ 100
#define MICROS_PER_READING (1000000 / SAMPLE_RATE_HZ)

// ----- GLOBAL VARIABLES -----
Madgwick filter;

// Queue for communication between interrupt and task
QueueHandle_t mpu_queue = NULL;

// Timing variables
unsigned long lastReadTime = 0;

// Data storage
float ax, ay, az;  // Accelerometer (g)
float gx, gy, gz;  // Gyroscope (deg/s)

// Orientation results
float roll, pitch, yaw;

// Driver da IMU
MPU6050 imu;

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
            bool dataValid = imu.readIMU(&ax, &ay, &az, &gx, &gy, &gz);
            
            if (dataValid) {
                // Update Madgwick filter with IMU-only mode (6DOF)
                filter.updateIMU(gx, gy, gz, ax, ay, az);
                
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
                Serial.println(yaw, 2);
            } else {
                Serial.println("Failed to read IMU data");
            }
        }
    }
}

// ----- ARDUINO SETUP -----
void setup() {
    Serial.begin(115200);
    Serial.println("IMU 6DOF with Madgwick AHRS - Starting...");
    
    // Initialize I2C
    Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
    Wire.setClock(I2C_FREQ);
    Serial.println("I2C initialized");
    
    // Initialize MPU6050
    if (!imu.mpu6050_init()) {
        Serial.println("Failed to initialize MPU6050!");
        while (1) {
            delay(1000);
        }
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
