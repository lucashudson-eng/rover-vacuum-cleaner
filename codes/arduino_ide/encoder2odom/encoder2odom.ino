/*
 * Encoder to Odometry for Arduino ESP32
 * Converts encoder readings to robot pose and velocity
 * Uses FreeRTOS for periodic odometry processing
 * 
 * Hardware:
 * - ESP32 Dev Board
 * - 2x Quadrature Encoders (Left and Right wheels)
 * 
 * Pin Configuration:
 * - Left Encoder A: GPIO 23
 * - Left Encoder B: GPIO 22
 * - Right Encoder A: GPIO 18
 * - Right Encoder B: GPIO 19
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// ----- REAR ENCODER PINS -----
#define ENC_LEFT_A      23
#define ENC_LEFT_B      22
#define ENC_RIGHT_A     18
#define ENC_RIGHT_B     19

// ----- ROBOT PARAMETERS -----
#define WHEEL_RADIUS    0.065/2    // meters
#define WHEEL_BASE      0.138     // meters
#define CPR             840       // counts per revolution (7x4xreduction)
#define UPDATE_PERIOD_MS 100      // odometry update interval

// ----- GLOBAL VARIABLES -----
volatile int32_t count_left = 0;
volatile int32_t count_right = 0;

volatile int lastLA = 0, lastLB = 0;
volatile int lastRA = 0, lastRB = 0;

// Robot pose
float x = 0.0, y = 0.0, theta = 0.0;

// Estimated velocities
float v = 0.0;      // linear (m/s)
float omega = 0.0;   // angular (rad/s)

// ----- ENCODER QUADRATURE X4 ISR -----
void IRAM_ATTR encoder_isr_handler() {
    // Left encoder
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
    // Right encoder
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

// ----- ARDUINO SETUP -----
void setup() {
    Serial.begin(115200);
    Serial.println("Encoder to Odometry - Starting...");
    
    // Configure encoder pins
    pinMode(ENC_LEFT_A, INPUT_PULLUP);
    pinMode(ENC_LEFT_B, INPUT_PULLUP);
    pinMode(ENC_RIGHT_A, INPUT_PULLUP);
    pinMode(ENC_RIGHT_B, INPUT_PULLUP);
    
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
    
    Serial.println("Encoder interrupts configured");
    
    // Create FreeRTOS task for odometry processing
    xTaskCreate(
        odometry_task,           // Task function
        "OdometryTask",          // Task name
        2048,                    // Stack size (words)
        NULL,                    // Task parameters
        1,                       // Task priority
        NULL                     // Task handle
    );
    
    Serial.println("Odometry task created");
    Serial.println("System ready!");
}

// ----- ARDUINO MAIN LOOP -----
void loop() {
    // Main loop is now free for other tasks
    // Odometry processing is handled by FreeRTOS task
    delay(1000); // Small delay to prevent watchdog reset
}
