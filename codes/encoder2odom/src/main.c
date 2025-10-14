#include <stdio.h>
#include <math.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// ----- PINOS DOS ENCODERS TRASEIROS -----
#define ENC_LEFT_A      23
#define ENC_LEFT_B      22
#define ENC_RIGHT_A     18
#define ENC_RIGHT_B     19

// ----- PARÂMETROS DO ROBÔ -----
#define WHEEL_RADIUS    0.065/2    // metros
#define WHEEL_BASE      0.138    // metros
#define CPR             840     // counts por rotação (7x4xreduction)
#define UPDATE_PERIOD_MS 100    // intervalo de atualização da odometria

// ----- VARIÁVEIS GLOBAIS -----
volatile int32_t count_left = 0;
volatile int32_t count_right = 0;

volatile int lastLA=0, lastLB=0;
volatile int lastRA=0, lastRB=0;

// Pose do robô
float x = 0.0, y = 0.0, theta = 0.0;

// Velocidades estimadas
float v = 0.0;      // linear (m/s)
float omega = 0.0;  // angular (rad/s)

// ----- ISR ENCODER QUADRATURA X4 -----
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

// ----- FUNÇÃO DE ODOMETRIA -----
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

    // Velocidades
    v = ds / dt;
    omega = dtheta / dt;
}

// ----- FUNÇÃO PRINCIPAL -----
void app_main(void)
{
    // Configura pinos
    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_ANYEDGE,
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = ((1ULL<<ENC_LEFT_A)|(1ULL<<ENC_LEFT_B)|(1ULL<<ENC_RIGHT_A)|(1ULL<<ENC_RIGHT_B)),
        .pull_up_en = 1,
        .pull_down_en = 0
    };
    gpio_config(&io_conf);

    // Inicializa últimos estados
    lastLA = gpio_get_level(ENC_LEFT_A);
    lastLB = gpio_get_level(ENC_LEFT_B);
    lastRA = gpio_get_level(ENC_RIGHT_A);
    lastRB = gpio_get_level(ENC_RIGHT_B);

    // Instala ISR
    gpio_install_isr_service(0);
    gpio_isr_handler_add(ENC_LEFT_A, encoder_isr_handler, (void*)(intptr_t)ENC_LEFT_A);
    gpio_isr_handler_add(ENC_LEFT_B, encoder_isr_handler, (void*)(intptr_t)ENC_LEFT_B);
    gpio_isr_handler_add(ENC_RIGHT_A, encoder_isr_handler, (void*)(intptr_t)ENC_RIGHT_A);
    gpio_isr_handler_add(ENC_RIGHT_B, encoder_isr_handler, (void*)(intptr_t)ENC_RIGHT_B);

    int32_t last_count_left = 0;
    int32_t last_count_right = 0;

    while(1){
        vTaskDelay(pdMS_TO_TICKS(UPDATE_PERIOD_MS));

        int32_t delta_left  = count_left - last_count_left;
        int32_t delta_right = count_right - last_count_right;

        last_count_left  = count_left;
        last_count_right = count_right;

        float dt = UPDATE_PERIOD_MS / 1000.0; // tempo em segundos

        update_odometry(delta_left, delta_right, dt);

        // Print da odometria
        printf("Pose: x=%.4f m, y=%.4f m, theta=%.3f rad\n", x, y, theta);
        printf("Velocidade: v=%.4f m/s, omega=%.4f rad/s\n", v, omega);
        printf("Counts: Left=%ld Right=%ld\n\n", count_left, count_right);
    }
}
