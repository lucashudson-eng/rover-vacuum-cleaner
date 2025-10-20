#include <Wire.h>
#include <MadgwickAHRS.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "MPU9250.h"

// ----- I2C CONFIGURATION -----
#define I2C_SDA_PIN 21
#define I2C_SCL_PIN 22
#define I2C_FREQ 400000

// ----- MPU9250 CONFIGURATION -----
#define MPU_INT_PIN 23

// ----- ENCODERS PINS -----
#define ENC_LEFT_A      23
#define ENC_LEFT_B      22
#define ENC_RIGHT_A     18
#define ENC_RIGHT_B     19

// ----- ROBOT PARAMETERS -----
#define WHEEL_RADIUS    0.065/2    // meters
#define WHEEL_BASE      0.138     // meters
#define CPR             840       // counts per revolution (7x4xreduction)
#define UPDATE_PERIOD_MS 100      // odometry and IMU update interval

// ----- GLOBAL VARIABLES -----
// MPU9250
Madgwick filter;

QueueHandle_t mpu_queue = NULL;

unsigned long lastReadTime = 0;

float ax, ay, az;  // Accelerometer (g)
float gx, gy, gz;  // Gyroscope (deg/s)
float mx, my, mz;  // Magnetometer (µT)

float roll, pitch, yaw;

MPU9250 imu;

// ENCODERS
volatile int32_t count_left = 0;
volatile int32_t count_right = 0;

volatile int lastLA = 0, lastLB = 0;
volatile int lastRA = 0, lastRB = 0;

// Robot pose
float x = 0.0, y = 0.0, theta = 0.0;

// Estimated velocities
float v = 0.0;      // linear (m/s)
float omega = 0.0;   // angular (rad/s)

// ----- IMU MPU9250 ISR -----
void IRAM_ATTR mpu_intr_handler() {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    uint32_t dummy = 1;
    xQueueSendFromISR(mpu_queue, &dummy, &xHigherPriorityTaskWoken);
    if (xHigherPriorityTaskWoken) {
        portYIELD_FROM_ISR();
    }
}

// ----- ENCODER QUADRATURE X4 ISR -----
void IRAM_ATTR encoder_isr_handler() {
  int a = digitalRead(ENC_LEFT_A);
  int b = digitalRead(ENC_LEFT_B);
  int transition = (lastLA << 3) | (lastLB << 2) | (a << 1) | b;
  
  switch (transition) {
    case 0b0001: case 0b0111: case 0b1110: case 0b1000:
      count_left++; 
      break;
    case 0b0010: case 0b0100: case 0b1101: case 0b1011:
      count_left--; 
      break;
    default: 
      break;
  }
  lastLA = a;
  lastLB = b;
}

void IRAM_ATTR encoder_right_isr_handler() {
  int a = digitalRead(ENC_RIGHT_A);
  int b = digitalRead(ENC_RIGHT_B);
  int transition = (lastRA << 3) | (lastRB << 2) | (a << 1) | b;
  
  switch (transition) {
    case 0b0001: case 0b0111: case 0b1110: case 0b1000:
      count_right++; 
      break;
    case 0b0010: case 0b0100: case 0b1101: case 0b1011:
      count_right--; 
      break;
    default: 
      break;
  }
  lastRA = a;
  lastRB = b;
}

// ----- FREERTOS IMU TASK -----
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
      
      // Read magnetometer data
      bool magValid = imu.readMag(&mx, &my, &mz);
      
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

// ----- ODOMETRY FUNCTION -----
void update_odometry(int32_t delta_left, int32_t delta_right, float dt) {
  float dist_per_count = 2.0 * PI * WHEEL_RADIUS / CPR;

  float d_left  = delta_left * dist_per_count;
  float d_right = delta_right * dist_per_count;

  float ds = (d_right + d_left) / 2.0;
  float dtheta = (d_right - d_left) / WHEEL_BASE;

  x += ds * cos(theta + dtheta / 2.0);
  y += ds * sin(theta + dtheta / 2.0);
  theta += dtheta;

  // Velocities
  v = ds / dt;
  omega = dtheta / dt;
}

// ----- FREERTOS ODOMETRY TASK -----
void odometry_task(void *parameter) {
  int32_t last_count_left = 0;
  int32_t last_count_right = 0;
  
  Serial.println("Odometry task started");
  
  while (true) {
    // Wait for the specified period
    vTaskDelay(pdMS_TO_TICKS(UPDATE_PERIOD_MS));
    
    // Calculate deltas
    int32_t delta_left  = count_left - last_count_left;
    int32_t delta_right = count_right - last_count_right;

    last_count_left  = count_left;
    last_count_right = count_right;

    float dt = UPDATE_PERIOD_MS / 1000.0; // time in seconds

    // Update odometry
    update_odometry(delta_left, delta_right, dt);

    // Print odometry
    Serial.print("Pose: x=");
    Serial.print(x, 4);
    Serial.print(" m, y=");
    Serial.print(y, 4);
    Serial.print(" m, theta=");
    Serial.print(theta, 3);
    Serial.println(" rad");
    
    Serial.print("Velocity: v=");
    Serial.print(v, 4);
    Serial.print(" m/s, omega=");
    Serial.print(omega, 4);
    Serial.println(" rad/s");
    
    Serial.print("Counts: Left=");
    Serial.print(count_left);
    Serial.print(" Right=");
    Serial.println(count_right);
    Serial.println();
  }
}

void setup() {
  Serial.begin(115200);

  // Initialize I2C
  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
  Wire.setClock(I2C_FREQ);
  Serial.println("I2C initialized");
  
  // Initialize MPU9250
  if (!imu.mpu9250_init()) {
    Serial.println("Failed to initialize MPU9250!");
    while (1) {
      delay(1000);
    }
  }
  if (!imu.ak8963_init()) {
    Serial.println("Failed to initialize AK8963 magnetometer!");
    // Continue without magnetometer
  }

  // Create queue for IMU interrupt communication
  mpu_queue = xQueueCreate(10, sizeof(uint32_t));
  if (mpu_queue == NULL) {
    Serial.println("Failed to create queue");
    return;
  }
  Serial.println("Queue created");
  
  // Configure encoder pins
  pinMode(ENC_LEFT_A, INPUT_PULLUP);
  pinMode(ENC_LEFT_B, INPUT_PULLUP);
  pinMode(ENC_RIGHT_A, INPUT_PULLUP);
  pinMode(ENC_RIGHT_B, INPUT_PULLUP);
  // Configure IMU interrupt pin
  pinMode(MPU_INT_PIN, INPUT_PULLUP);
  
  // Initialize last states
  lastLA = digitalRead(ENC_LEFT_A);
  lastLB = digitalRead(ENC_LEFT_B);
  lastRA = digitalRead(ENC_RIGHT_A);
  lastRB = digitalRead(ENC_RIGHT_B);
  
  // Attach interrupts
  attachInterrupt(digitalPinToInterrupt(ENC_LEFT_A), encoder_isr_handler, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENC_LEFT_B), encoder_isr_handler, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENC_RIGHT_A), encoder_right_isr_handler, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENC_RIGHT_B), encoder_right_isr_handler, CHANGE);
  attachInterrupt(digitalPinToInterrupt(MPU_INT_PIN), mpu_intr_handler, RISING);

  // Initialize Madgwick filter
  filter.begin(UPDATE_PERIOD_MS);
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

  // Create FreeRTOS task for odometry processing
  xTaskCreate(
    odometry_task,           // Task function
    "OdometryTask",          // Task name
    2048,                    // Stack size (words)
    NULL,                    // Task parameters
    1,                       // Task priority
    NULL                     // Task handle
  );
}

void loop() {
  delay(1000); // Small delay to prevent watchdog reset
}
