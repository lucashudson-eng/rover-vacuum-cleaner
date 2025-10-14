#include <stdio.h>
#include <string.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/i2c.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "madgwick_ahrs.h"
#include "mpu9250.h"

static const char *TAG = "AHRS_MPU9250";

// I2C Configuration
#define I2C_MASTER_SCL_IO 22
#define I2C_MASTER_SDA_IO 21
#define I2C_MASTER_NUM I2C_NUM_0
#define I2C_MASTER_FREQ_HZ 400000
#define I2C_MASTER_TX_BUF_DISABLE 0
#define I2C_MASTER_RX_BUF_DISABLE 0

// Queue for communication between interrupt and task
static QueueHandle_t mpu_queue = NULL;

// AHRS filter instance
static madgwick_ahrs_t filter;

// Timing variables
static uint64_t last_read_time = 0;

/**
 * @brief Initialize I2C master
 */
static esp_err_t i2c_master_init(void) {
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };
    
    esp_err_t ret = i2c_param_config(I2C_MASTER_NUM, &conf);
    if (ret != ESP_OK) {
        return ret;
    }
    
    return i2c_driver_install(I2C_MASTER_NUM, conf.mode, 
                             I2C_MASTER_RX_BUF_DISABLE, 
                             I2C_MASTER_TX_BUF_DISABLE, 0);
}


/**
 * @brief MPU9250 interrupt handler
 */
static void IRAM_ATTR mpu_intr_handler(void *arg) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    uint32_t dummy = 1;
    xQueueSendFromISR(mpu_queue, &dummy, &xHigherPriorityTaskWoken);
    if (xHigherPriorityTaskWoken) {
        portYIELD_FROM_ISR();
    }
}

/**
 * @brief Configure interrupt pin
 */
static esp_err_t setup_interrupt_pin(void) {
    gpio_num_t int_pin = mpu9250_get_int_pin();
    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_POSEDGE,
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = (1ULL << int_pin),
        .pull_down_en = 0,
        .pull_up_en = 1,
    };
    
    esp_err_t ret = gpio_config(&io_conf);
    if (ret != ESP_OK) {
        return ret;
    }
    
    return gpio_install_isr_service(0);
}


/**
 * @brief Main task to process MPU9250 data
 */
static void mpu_task(void *pvParameters) {
    mpu9250_data_t mpu_data;
    uint32_t dummy;
    
    while (1) {
        // Wait for interrupt signal
        if (xQueueReceive(mpu_queue, &dummy, portMAX_DELAY)) {
            uint64_t now = esp_timer_get_time();
            float dt = (now - last_read_time) / 1e6f;
            last_read_time = now;
            
            // Read all MPU9250 data (IMU + magnetometer)
            if (mpu9250_read_all(&mpu_data) == ESP_OK) {
                // Convert to physical units using library functions with default ranges
                float axg = mpu9250_accel_to_g(mpu_data.ax, MPU9250_ACCEL_RANGE_DEFAULT);
                float ayg = mpu9250_accel_to_g(mpu_data.ay, MPU9250_ACCEL_RANGE_DEFAULT);
                float azg = mpu9250_accel_to_g(mpu_data.az, MPU9250_ACCEL_RANGE_DEFAULT);
                float gxds = mpu9250_gyro_to_dps(mpu_data.gx, MPU9250_GYRO_RANGE_DEFAULT);
                float gyds = mpu9250_gyro_to_dps(mpu_data.gy, MPU9250_GYRO_RANGE_DEFAULT);
                float gzds = mpu9250_gyro_to_dps(mpu_data.gz, MPU9250_GYRO_RANGE_DEFAULT);
                
                // Convert magnetometer to microtesla (µT)
                float mx_ut = mpu9250_mag_to_ut(mpu_data.mx);
                float my_ut = mpu9250_mag_to_ut(mpu_data.my);
                float mz_ut = mpu9250_mag_to_ut(mpu_data.mz);
                
                // Check if magnetometer data is valid
                bool mag_valid = (mpu_data.mx != 0 || mpu_data.my != 0 || mpu_data.mz != 0);
                
                // Update AHRS filter with magnetometer data if available
                if (mag_valid) {
                    madgwick_ahrs_update(&filter, gxds, gyds, gzds, axg, ayg, azg, mx_ut, my_ut, mz_ut);
                } else {
                    // Fallback to IMU-only mode if magnetometer fails
                    madgwick_ahrs_update_imu(&filter, gxds, gyds, gzds, axg, ayg, azg);
                }
                
                // Get Euler angles (in degrees)
                float roll = madgwick_ahrs_get_roll(&filter);
                float pitch = madgwick_ahrs_get_pitch(&filter);
                float yaw = madgwick_ahrs_get_yaw(&filter);
                
                // Real sampling frequency
                float freq = (dt > 0) ? (1.0f / dt) : 0;
                
                // Display results
                printf("f: %.2f Hz  Roll: %.2f  Pitch: %.2f  Yaw: %.2f  Mag: %s\n", 
                       freq, roll, pitch, yaw, mag_valid ? "OK" : "FAIL");
            }
        }
    }
}

void app_main(void) {
    esp_err_t ret;
    
    // Initialize AHRS filter
    ret = madgwick_ahrs_init(&filter);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize AHRS filter: %s", esp_err_to_name(ret));
        return;
    }
    
    ret = madgwick_ahrs_begin(&filter, 100.0f); // 100 Hz
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure filter frequency: %s", esp_err_to_name(ret));
        return;
    }
    
    // Initialize I2C
    ret = i2c_master_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize I2C: %s", esp_err_to_name(ret));
        return;
    }
    
    // Initialize MPU9250
    mpu9250_config_t mpu_config = MPU9250_DEFAULT_CONFIG();
    mpu_config.int_pin = GPIO_NUM_23;
    ret = mpu9250_init(&mpu_config);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize MPU9250: %s", esp_err_to_name(ret));
        return;
    }
    
    
    // Create queue for interrupt
    mpu_queue = xQueueCreate(10, sizeof(uint32_t));
    if (mpu_queue == NULL) {
        ESP_LOGE(TAG, "Failed to create queue");
        return;
    }
    
    // Configure interrupt pin
    ret = setup_interrupt_pin();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure interrupt pin: %s", esp_err_to_name(ret));
        return;
    }
    
    // Add interrupt handler
    gpio_num_t int_pin = mpu9250_get_int_pin();
    ret = gpio_isr_handler_add(int_pin, mpu_intr_handler, NULL);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to add interrupt handler: %s", esp_err_to_name(ret));
        return;
    }
    
    // Create task to process data
    xTaskCreate(mpu_task, "mpu_task", 4096, NULL, 5, NULL);
    
    ESP_LOGI(TAG, "MPU9250 + AHRS initialized!");
    
    // Initialize time
    last_read_time = esp_timer_get_time();
}