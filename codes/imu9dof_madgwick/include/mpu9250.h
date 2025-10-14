#ifndef MPU9250_H
#define MPU9250_H

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"
#include "driver/i2c.h"
#include "driver/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

// MPU9250 Configuration
#define MPU9250_ADDR 0x68
#define AK8963_ADDR 0x0C  // AK8963 Magnetometer

// MPU9250 Configuration Defaults
// To change settings, modify only these defines:
#define MPU9250_ACCEL_RANGE_DEFAULT    MPU9250_ACCEL_RANGE_2G   // ±2g
#define MPU9250_GYRO_RANGE_DEFAULT     MPU9250_GYRO_RANGE_250DPS // ±250°/s
#define MPU9250_DLPF_CFG_DEFAULT       MPU9250_DLPF_20HZ       // 20Hz
#define MPU9250_SAMPLE_RATE_DIV_DEFAULT MPU9250_SAMPLE_RATE_100HZ // 100Hz

// Accelerometer Range Options
#define MPU9250_ACCEL_RANGE_2G    0  // ±2g
#define MPU9250_ACCEL_RANGE_4G    1  // ±4g
#define MPU9250_ACCEL_RANGE_8G    2  // ±8g
#define MPU9250_ACCEL_RANGE_16G   3  // ±16g

// Gyroscope Range Options
#define MPU9250_GYRO_RANGE_250DPS  0  // ±250°/s
#define MPU9250_GYRO_RANGE_500DPS  1  // ±500°/s
#define MPU9250_GYRO_RANGE_1000DPS 2  // ±1000°/s
#define MPU9250_GYRO_RANGE_2000DPS 3  // ±2000°/s

// Digital Low Pass Filter Options
#define MPU9250_DLPF_250HZ  0  // 250Hz
#define MPU9250_DLPF_184HZ  1  // 184Hz
#define MPU9250_DLPF_92HZ   2  // 92Hz
#define MPU9250_DLPF_41HZ   3  // 41Hz
#define MPU9250_DLPF_20HZ   4  // 20Hz
#define MPU9250_DLPF_10HZ   5  // 10Hz
#define MPU9250_DLPF_5HZ    6  // 5Hz

// Sample Rate Options (1kHz / (1 + div))
#define MPU9250_SAMPLE_RATE_1KHZ   0  // 1000Hz
#define MPU9250_SAMPLE_RATE_500HZ  1  // 500Hz
#define MPU9250_SAMPLE_RATE_250HZ  3  // 250Hz
#define MPU9250_SAMPLE_RATE_200HZ  4  // 200Hz
#define MPU9250_SAMPLE_RATE_100HZ  9  // 100Hz
#define MPU9250_SAMPLE_RATE_50HZ   19 // 50Hz
#define MPU9250_SAMPLE_RATE_25HZ  39  // 25Hz
#define MPU9250_SAMPLE_RATE_10HZ  99  // 10Hz

// Structure for MPU9250 data (9DOF)
typedef struct {
    int16_t ax, ay, az;  // Accelerometer
    int16_t gx, gy, gz;  // Gyroscope
    int16_t mx, my, mz;  // Magnetometer
} mpu9250_data_t;

// MPU9250 configuration structure
typedef struct {
    i2c_port_t i2c_port;
    gpio_num_t int_pin;
    uint32_t i2c_freq;
} mpu9250_config_t;

// Default configuration
#define MPU9250_DEFAULT_CONFIG() { \
    .i2c_port = I2C_NUM_0, \
    .int_pin = GPIO_NUM_23, \
    .i2c_freq = 400000 \
}

/**
 * @brief Initialize MPU9250 sensor
 * 
 * @param config Configuration structure
 * @return esp_err_t ESP_OK on success
 */
esp_err_t mpu9250_init(const mpu9250_config_t *config);

/**
 * @brief Deinitialize MPU9250 sensor
 * 
 * @return esp_err_t ESP_OK on success
 */
esp_err_t mpu9250_deinit(void);

/**
 * @brief Read accelerometer and gyroscope data from MPU9250
 * 
 * @param data Pointer to data structure
 * @return esp_err_t ESP_OK on success
 */
esp_err_t mpu9250_read_imu(mpu9250_data_t *data);

/**
 * @brief Read magnetometer data from AK8963
 * 
 * @param data Pointer to data structure
 * @return esp_err_t ESP_OK on success
 */
esp_err_t mpu9250_read_magnetometer(mpu9250_data_t *data);

/**
 * @brief Read all sensor data (IMU + magnetometer)
 * 
 * @param data Pointer to data structure
 * @return esp_err_t ESP_OK on success
 */
esp_err_t mpu9250_read_all(mpu9250_data_t *data);

/**
 * @brief Configure MPU9250 settings
 * 
 * @param accel_range Accelerometer range (0=±2g, 1=±4g, 2=±8g, 3=±16g)
 * @param gyro_range Gyroscope range (0=±250°/s, 1=±500°/s, 2=±1000°/s, 3=±2000°/s)
 * @param dlpf_cfg Digital Low Pass Filter configuration
 * @param sample_rate_div Sample rate divider
 * @return esp_err_t ESP_OK on success
 */
esp_err_t mpu9250_configure(uint8_t accel_range, uint8_t gyro_range, 
                           uint8_t dlpf_cfg, uint8_t sample_rate_div);

/**
 * @brief Enable data ready interrupt
 * 
 * @return esp_err_t ESP_OK on success
 */
esp_err_t mpu9250_enable_interrupt(void);

/**
 * @brief Disable data ready interrupt
 * 
 * @return esp_err_t ESP_OK on success
 */
esp_err_t mpu9250_disable_interrupt(void);

/**
 * @brief Get interrupt pin number
 * 
 * @return gpio_num_t Interrupt pin number
 */
gpio_num_t mpu9250_get_int_pin(void);

/**
 * @brief Convert raw accelerometer data to g
 * 
 * @param raw Raw accelerometer value
 * @param range Accelerometer range setting
 * @return float Acceleration in g
 */
float mpu9250_accel_to_g(int16_t raw, uint8_t range);

/**
 * @brief Convert raw gyroscope data to degrees/second
 * 
 * @param raw Raw gyroscope value
 * @param range Gyroscope range setting
 * @return float Angular velocity in degrees/second
 */
float mpu9250_gyro_to_dps(int16_t raw, uint8_t range);

/**
 * @brief Convert raw magnetometer data to microtesla
 * 
 * @param raw Raw magnetometer value
 * @return float Magnetic field in microtesla
 */
float mpu9250_mag_to_ut(int16_t raw);

/**
 * @brief Check if magnetometer data is available
 * 
 * @return bool True if magnetometer is ready
 */
bool mpu9250_magnetometer_ready(void);

#ifdef __cplusplus
}
#endif

#endif // MPU9250_H
