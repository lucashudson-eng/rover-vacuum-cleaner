//=============================================================================================
// madgwick_ahrs.h
//=============================================================================================
//
// Implementation of Madgwick IMU and AHRS algorithms for ESP-IDF.
// Based on: http://www.x-io.co.uk/open-source-imu-and-ahrs-algorithms/
//
// From x-io website: "Open source resources available on this site are
// provided under the GNU General Public License, unless an alternative
// license is provided in the source code."
//
// Date			Author          Notes
// 29/09/2011	SOH Madgwick    Initial release
// 02/10/2011	SOH Madgwick	Optimized to reduce CPU load
// 19/02/2012	SOH Madgwick	Magnetometer measurement is normalized
// [Current date]	Adapted for ESP-IDF
//
//=============================================================================================
#ifndef MADGWICK_AHRS_H
#define MADGWICK_AHRS_H

#include <math.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

//--------------------------------------------------------------------------------------------
// Variable declarations
typedef struct {
    float beta;				// algorithm gain
    float q0;
    float q1;
    float q2;
    float q3;	// quaternion of sensor frame relative to auxiliary frame
    float invSampleFreq;
    float roll;
    float pitch;
    float yaw;
    uint8_t anglesComputed;
} madgwick_ahrs_t;

//-------------------------------------------------------------------------------------------
// Function declarations

/**
 * @brief Initialize Madgwick AHRS structure
 * 
 * @param filter Pointer to the filter structure
 * @return esp_err_t ESP_OK on success
 */
esp_err_t madgwick_ahrs_init(madgwick_ahrs_t *filter);

/**
 * @brief Configure sampling frequency
 * 
 * @param filter Pointer to the filter structure
 * @param sampleFrequency Sampling frequency in Hz
 * @return esp_err_t ESP_OK on success
 */
esp_err_t madgwick_ahrs_begin(madgwick_ahrs_t *filter, float sampleFrequency);

/**
 * @brief Update filter with gyroscope, accelerometer and magnetometer data
 * 
 * @param filter Pointer to the filter structure
 * @param gx Gyroscope angular velocity on X axis (degrees/sec)
 * @param gy Gyroscope angular velocity on Y axis (degrees/sec)
 * @param gz Gyroscope angular velocity on Z axis (degrees/sec)
 * @param ax Acceleration on X axis (g)
 * @param ay Acceleration on Y axis (g)
 * @param az Acceleration on Z axis (g)
 * @param mx Magnetic field on X axis (µT)
 * @param my Magnetic field on Y axis (µT)
 * @param mz Magnetic field on Z axis (µT)
 * @return esp_err_t ESP_OK on success
 */
esp_err_t madgwick_ahrs_update(madgwick_ahrs_t *filter, 
                               float gx, float gy, float gz, 
                               float ax, float ay, float az, 
                               float mx, float my, float mz);

/**
 * @brief Update filter with gyroscope and accelerometer data only (without magnetometer)
 * 
 * @param filter Pointer to the filter structure
 * @param gx Gyroscope angular velocity on X axis (degrees/sec)
 * @param gy Gyroscope angular velocity on Y axis (degrees/sec)
 * @param gz Gyroscope angular velocity on Z axis (degrees/sec)
 * @param ax Acceleration on X axis (g)
 * @param ay Acceleration on Y axis (g)
 * @param az Acceleration on Z axis (g)
 * @return esp_err_t ESP_OK on success
 */
esp_err_t madgwick_ahrs_update_imu(madgwick_ahrs_t *filter, 
                                   float gx, float gy, float gz, 
                                   float ax, float ay, float az);

/**
 * @brief Get roll angle in degrees
 * 
 * @param filter Pointer to the filter structure
 * @return float Roll angle in degrees
 */
float madgwick_ahrs_get_roll(madgwick_ahrs_t *filter);

/**
 * @brief Get pitch angle in degrees
 * 
 * @param filter Pointer to the filter structure
 * @return float Pitch angle in degrees
 */
float madgwick_ahrs_get_pitch(madgwick_ahrs_t *filter);

/**
 * @brief Get yaw angle in degrees
 * 
 * @param filter Pointer to the filter structure
 * @return float Yaw angle in degrees
 */
float madgwick_ahrs_get_yaw(madgwick_ahrs_t *filter);

/**
 * @brief Get roll angle in radians
 * 
 * @param filter Pointer to the filter structure
 * @return float Roll angle in radians
 */
float madgwick_ahrs_get_roll_radians(madgwick_ahrs_t *filter);

/**
 * @brief Get pitch angle in radians
 * 
 * @param filter Pointer to the filter structure
 * @return float Pitch angle in radians
 */
float madgwick_ahrs_get_pitch_radians(madgwick_ahrs_t *filter);

/**
 * @brief Get yaw angle in radians
 * 
 * @param filter Pointer to the filter structure
 * @return float Yaw angle in radians
 */
float madgwick_ahrs_get_yaw_radians(madgwick_ahrs_t *filter);

/**
 * @brief Set algorithm beta gain
 * 
 * @param filter Pointer to the filter structure
 * @param beta Beta gain value
 * @return esp_err_t ESP_OK on success
 */
esp_err_t madgwick_ahrs_set_beta(madgwick_ahrs_t *filter, float beta);

/**
 * @brief Get algorithm beta gain
 * 
 * @param filter Pointer to the filter structure
 * @return float Beta gain value
 */
float madgwick_ahrs_get_beta(madgwick_ahrs_t *filter);

#ifdef __cplusplus
}
#endif

#endif // MADGWICK_AHRS_H


