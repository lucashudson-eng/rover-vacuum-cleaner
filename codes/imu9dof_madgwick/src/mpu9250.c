#include "mpu9250.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string.h>

static const char *TAG = "MPU9250";

// Internal state
static mpu9250_config_t s_config;
static bool s_initialized = false;
static uint8_t s_accel_range = 0;
static uint8_t s_gyro_range = 0;

// Helper function to write a byte to MPU9250
static esp_err_t write_mpu(uint8_t reg, uint8_t data) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (MPU9250_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg, true);
    i2c_master_write_byte(cmd, data, true);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(s_config.i2c_port, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}

// Helper function to write a byte to AK8963 magnetometer
static esp_err_t write_ak8963(uint8_t reg, uint8_t data) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (AK8963_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg, true);
    i2c_master_write_byte(cmd, data, true);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(s_config.i2c_port, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}

esp_err_t mpu9250_init(const mpu9250_config_t *config) {
    if (config == NULL) {
        ESP_LOGE(TAG, "Configuration is NULL");
        return ESP_ERR_INVALID_ARG;
    }
    
    if (s_initialized) {
        ESP_LOGW(TAG, "MPU9250 already initialized");
        return ESP_OK;
    }
    
    // Store configuration
    memcpy(&s_config, config, sizeof(mpu9250_config_t));
    
    esp_err_t ret;
    
    // Wake up MPU9250
    ret = write_mpu(0x6B, 0x00);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to wake up MPU9250: %s", esp_err_to_name(ret));
        return ret;
    }
    vTaskDelay(pdMS_TO_TICKS(100));
    
    // Configure MPU9250 with default settings
    ret = mpu9250_configure(MPU9250_ACCEL_RANGE_DEFAULT, 
                           MPU9250_GYRO_RANGE_DEFAULT, 
                           MPU9250_DLPF_CFG_DEFAULT, 
                           MPU9250_SAMPLE_RATE_DIV_DEFAULT);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure MPU9250 with defaults: %s", esp_err_to_name(ret));
        return ret;
    }
    
    // Enable Data Ready interrupt
    ret = write_mpu(0x38, 0x01);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to enable interrupt: %s", esp_err_to_name(ret));
        return ret;
    }
    
    // Configure magnetometer (AK8963)
    // First, enable I2C bypass to access magnetometer directly
    ret = write_mpu(0x37, 0x02);  // Enable I2C bypass
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to enable I2C bypass: %s", esp_err_to_name(ret));
        return ret;
    }
    vTaskDelay(pdMS_TO_TICKS(10));
    
    // Initialize magnetometer
    ret = write_ak8963(0x0A, 0x16);  // Set magnetometer to 16-bit output, continuous mode 2
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure magnetometer: %s", esp_err_to_name(ret));
        return ret;
    }
    vTaskDelay(pdMS_TO_TICKS(10));
    
    s_initialized = true;
    ESP_LOGI(TAG, "MPU9250 initialized successfully");
    
    return ESP_OK;
}

esp_err_t mpu9250_deinit(void) {
    if (!s_initialized) {
        return ESP_OK;
    }
    
    // Put MPU9250 to sleep
    write_mpu(0x6B, 0x40);
    
    s_initialized = false;
    ESP_LOGI(TAG, "MPU9250 deinitialized");
    
    return ESP_OK;
}

esp_err_t mpu9250_read_imu(mpu9250_data_t *data) {
    if (!s_initialized) {
        ESP_LOGE(TAG, "MPU9250 not initialized");
        return ESP_ERR_INVALID_STATE;
    }
    
    if (data == NULL) {
        ESP_LOGE(TAG, "Data pointer is NULL");
        return ESP_ERR_INVALID_ARG;
    }
    
    uint8_t buffer[14];
    
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (MPU9250_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, 0x3B, true);
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (MPU9250_ADDR << 1) | I2C_MASTER_READ, true);
    i2c_master_read(cmd, buffer, 14, I2C_MASTER_LAST_NACK);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(s_config.i2c_port, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    
    if (ret == ESP_OK) {
        data->ax = (buffer[0] << 8) | buffer[1];
        data->ay = (buffer[2] << 8) | buffer[3];
        data->az = (buffer[4] << 8) | buffer[5];
        // buffer[6] and buffer[7] are temperature (ignored)
        data->gx = (buffer[8] << 8) | buffer[9];
        data->gy = (buffer[10] << 8) | buffer[11];
        data->gz = (buffer[12] << 8) | buffer[13];
    }
    
    return ret;
}

esp_err_t mpu9250_read_magnetometer(mpu9250_data_t *data) {
    if (!s_initialized) {
        ESP_LOGE(TAG, "MPU9250 not initialized");
        return ESP_ERR_INVALID_STATE;
    }
    
    if (data == NULL) {
        ESP_LOGE(TAG, "Data pointer is NULL");
        return ESP_ERR_INVALID_ARG;
    }
    
    uint8_t buffer[7];
    
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (AK8963_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, 0x03, true);  // AK8963_ST1 register
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (AK8963_ADDR << 1) | I2C_MASTER_READ, true);
    i2c_master_read(cmd, buffer, 7, I2C_MASTER_LAST_NACK);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(s_config.i2c_port, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    
    if (ret == ESP_OK) {
        // Check if data is ready (ST1 bit 0)
        if (buffer[0] & 0x01) {
            data->mx = (buffer[2] << 8) | buffer[1];  // Note: LSB first for magnetometer
            data->my = (buffer[4] << 8) | buffer[3];
            data->mz = (buffer[6] << 8) | buffer[5];
        } else {
            ret = ESP_ERR_TIMEOUT;  // Data not ready
        }
    }
    
    return ret;
}

esp_err_t mpu9250_read_all(mpu9250_data_t *data) {
    if (!s_initialized) {
        ESP_LOGE(TAG, "MPU9250 not initialized");
        return ESP_ERR_INVALID_STATE;
    }
    
    if (data == NULL) {
        ESP_LOGE(TAG, "Data pointer is NULL");
        return ESP_ERR_INVALID_ARG;
    }
    
    // Read IMU data first
    esp_err_t ret = mpu9250_read_imu(data);
    if (ret != ESP_OK) {
        return ret;
    }
    
    // Read magnetometer data
    esp_err_t mag_ret = mpu9250_read_magnetometer(data);
    if (mag_ret != ESP_OK) {
        // Set magnetometer data to zero if read fails
        data->mx = 0;
        data->my = 0;
        data->mz = 0;
    }
    
    return ESP_OK;
}

esp_err_t mpu9250_configure(uint8_t accel_range, uint8_t gyro_range, 
                           uint8_t dlpf_cfg, uint8_t sample_rate_div) {
    if (!s_initialized) {
        ESP_LOGE(TAG, "MPU9250 not initialized");
        return ESP_ERR_INVALID_STATE;
    }
    
    esp_err_t ret;
    
    // Configure accelerometer range
    if (accel_range > 3) {
        ESP_LOGE(TAG, "Invalid accelerometer range: %d", accel_range);
        return ESP_ERR_INVALID_ARG;
    }
    ret = write_mpu(0x1C, accel_range << 3);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure accelerometer range: %s", esp_err_to_name(ret));
        return ret;
    }
    s_accel_range = accel_range;
    
    // Configure gyroscope range
    if (gyro_range > 3) {
        ESP_LOGE(TAG, "Invalid gyroscope range: %d", gyro_range);
        return ESP_ERR_INVALID_ARG;
    }
    ret = write_mpu(0x1B, gyro_range << 3);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure gyroscope range: %s", esp_err_to_name(ret));
        return ret;
    }
    s_gyro_range = gyro_range;
    
    // Configure DLPF
    if (dlpf_cfg > 6) {
        ESP_LOGE(TAG, "Invalid DLPF configuration: %d", dlpf_cfg);
        return ESP_ERR_INVALID_ARG;
    }
    ret = write_mpu(0x1A, dlpf_cfg);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure DLPF: %s", esp_err_to_name(ret));
        return ret;
    }
    
    // Configure sample rate divider
    ret = write_mpu(0x19, sample_rate_div);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure sample rate: %s", esp_err_to_name(ret));
        return ret;
    }
    
    ESP_LOGI(TAG, "MPU9250 configured: accel_range=%d, gyro_range=%d, dlpf=%d, div=%d", 
             accel_range, gyro_range, dlpf_cfg, sample_rate_div);
    
    return ESP_OK;
}

esp_err_t mpu9250_enable_interrupt(void) {
    if (!s_initialized) {
        ESP_LOGE(TAG, "MPU9250 not initialized");
        return ESP_ERR_INVALID_STATE;
    }
    
    return write_mpu(0x38, 0x01);
}

esp_err_t mpu9250_disable_interrupt(void) {
    if (!s_initialized) {
        ESP_LOGE(TAG, "MPU9250 not initialized");
        return ESP_ERR_INVALID_STATE;
    }
    
    return write_mpu(0x38, 0x00);
}

gpio_num_t mpu9250_get_int_pin(void) {
    return s_config.int_pin;
}

float mpu9250_accel_to_g(int16_t raw, uint8_t range) {
    const float scales[] = {16384.0f, 8192.0f, 4096.0f, 2048.0f};
    if (range > 3) range = 0;
    return (float)raw / scales[range];
}

float mpu9250_gyro_to_dps(int16_t raw, uint8_t range) {
    const float scales[] = {131.0f, 65.5f, 32.8f, 16.4f};
    if (range > 3) range = 0;
    return (float)raw / scales[range];
}

float mpu9250_mag_to_ut(int16_t raw) {
    // AK8963 sensitivity: 0.15 µT/LSB in 16-bit mode
    return (float)raw * 0.15f;
}

bool mpu9250_magnetometer_ready(void) {
    if (!s_initialized) {
        return false;
    }
    
    uint8_t st1;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (AK8963_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, 0x02, true);  // AK8963_ST1 register
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (AK8963_ADDR << 1) | I2C_MASTER_READ, true);
    i2c_master_read(cmd, &st1, 1, I2C_MASTER_LAST_NACK);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(s_config.i2c_port, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    
    return (ret == ESP_OK) && (st1 & 0x01);
}
