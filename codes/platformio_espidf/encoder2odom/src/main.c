#include <stdio.h>
#include <math.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// ----- REAR ENCODER PINS -----
#define ENC_LEFT_A      23
#define ENC_LEFT_B      22
#define ENC_RIGHT_A     18
#define ENC_RIGHT_B     19

// ----- ROBOT PARAMETERS -----
#define WHEEL_RADIUS    0.065/2    // meters
#define WHEEL_BASE      0.138    // meters
#define CPR             840     // counts per revolution (7x4xreduction)
#define UPDATE_PERIOD_MS 100    // odometry update interval

// ----- GLOBAL VARIABLES -----
volatile int32_t count_left = 0;
volatile int32_t count_right = 0;

volatile int lastLA=0, lastLB=0;
volatile int lastRA=0, lastRB=0;

// Robot pose
float x = 0.0, y = 0.0, theta = 0.0;

// Estimated velocities
float v = 0.0;      // linear (m/s)
float omega = 0.0;  // angular (rad/s)

// ----- ENCODER QUADRATURE X4 ISR -----
static void IRAM_ATTR encoder_isr_handler(void* arg)
{
    int pin = (int)(intptr_t)arg;

    if(pin == ENC_LEFT_A || pin == ENC_LEFT_B){
        int a = gpio_get_level(ENC_LEFT_A);
        int b = gpio_get_level(ENC_LEFT_B);
        int transition = (lastLA<<3)|(lastLB<<2)|(a<<1)|b;
        switch(transition){
            case 0b0001: case 0b0111: case 0b1110: case 0b1000:
                count_left++; break;
            case 0b0010: case 0b0100: case 0b1101: case 0b1011:
                count_left--; break;
            default: break;
        }
        lastLA = a;
        lastLB = b;
    } else {
        int a = gpio_get_level(ENC_RIGHT_A);
        int b = gpio_get_level(ENC_RIGHT_B);
        int transition = (lastRA<<3)|(lastRB<<2)|(a<<1)|b;
        switch(transition){
            case 0b0001: case 0b0111: case 0b1110: case 0b1000:
                count_right++; break;
            case 0b0010: case 0b0100: case 0b1101: case 0b1011:
                count_right--; break;
            default: break;
        }
        lastRA = a;
        lastRB = b;
    }
}

// ----- ODOMETRY FUNCTION -----
void update_odometry(int32_t delta_left, int32_t delta_right, float dt)
{
    float dist_per_count = 2.0 * M_PI * WHEEL_RADIUS / CPR;

    float d_left  = delta_left * dist_per_count;
    float d_right = delta_right * dist_per_count;

    float ds = (d_right + d_left)/2.0;
    float dtheta = (d_right - d_left)/WHEEL_BASE;

    x += ds * cos(theta + dtheta/2.0);
    y += ds * sin(theta + dtheta/2.0);
    theta += dtheta;

    // Velocities
    v = ds / dt;
    omega = dtheta / dt;
}

// ----- FREERTOS ODOMETRY TASK -----
static void odometry_task(void *pvParameters)
{
    int32_t last_count_left = 0;
    int32_t last_count_right = 0;
    
    printf("Odometry task started\n");
    
    while(1) {
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
        printf("Pose: x=%.4f m, y=%.4f m, theta=%.3f rad\n", x, y, theta);
        printf("Velocity: v=%.4f m/s, omega=%.4f rad/s\n", v, omega);
        printf("Counts: Left=%ld Right=%ld\n\n", count_left, count_right);
    }
}

// ----- MAIN FUNCTION -----
void app_main(void)
{
    printf("Encoder to Odometry - Starting...\n");
    
    // Configure pins
    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_ANYEDGE,
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = ((1ULL<<ENC_LEFT_A)|(1ULL<<ENC_LEFT_B)|(1ULL<<ENC_RIGHT_A)|(1ULL<<ENC_RIGHT_B)),
        .pull_up_en = 1,
        .pull_down_en = 0
    };
    gpio_config(&io_conf);

    // Initialize last states
    lastLA = gpio_get_level(ENC_LEFT_A);
    lastLB = gpio_get_level(ENC_LEFT_B);
    lastRA = gpio_get_level(ENC_RIGHT_A);
    lastRB = gpio_get_level(ENC_RIGHT_B);

    // Install ISR
    gpio_install_isr_service(0);
    gpio_isr_handler_add(ENC_LEFT_A, encoder_isr_handler, (void*)(intptr_t)ENC_LEFT_A);
    gpio_isr_handler_add(ENC_LEFT_B, encoder_isr_handler, (void*)(intptr_t)ENC_LEFT_B);
    gpio_isr_handler_add(ENC_RIGHT_A, encoder_isr_handler, (void*)(intptr_t)ENC_RIGHT_A);
    gpio_isr_handler_add(ENC_RIGHT_B, encoder_isr_handler, (void*)(intptr_t)ENC_RIGHT_B);

    printf("Encoder interrupts configured\n");
    
    // Create FreeRTOS task for odometry processing
    xTaskCreate(
        odometry_task,           // Task function
        "OdometryTask",          // Task name
        2048,                    // Stack size (words)
        NULL,                    // Task parameters
        1,                       // Task priority
        NULL                     // Task handle
    );
    
    printf("Odometry task created\n");
    printf("System ready!\n");
    
    // Main function can now return - FreeRTOS scheduler will handle the rest
}
