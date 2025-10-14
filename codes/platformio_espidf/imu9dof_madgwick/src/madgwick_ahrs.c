//=============================================================================================
// madgwick_ahrs.c
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

//-------------------------------------------------------------------------------------------
// Header files

#include "madgwick_ahrs.h"
#include <math.h>
#include <string.h>

//-------------------------------------------------------------------------------------------
// Definitions

#define SAMPLE_FREQ_DEF   512.0f          // sampling frequency in Hz
#define BETA_DEF          0.1f            // 2 * proportional gain

//-------------------------------------------------------------------------------------------
// Helper functions

/**
 * @brief Fast inverse square root
 * See: http://en.wikipedia.org/wiki/Fast_inverse_square_root
 */
static float inv_sqrt(float x) {
    float halfx = 0.5f * x;
    float y = x;
    long i = *(long*)&y;
    i = 0x5f3759df - (i>>1);
    y = *(float*)&i;
    y = y * (1.5f - (halfx * y * y));
    y = y * (1.5f - (halfx * y * y));
    return y;
}

/**
 * @brief Calculate angles from quaternion
 */
static void compute_angles(madgwick_ahrs_t *filter) {
    filter->roll = atan2f(filter->q0 * filter->q1 + filter->q2 * filter->q3, 
                         0.5f - filter->q1 * filter->q1 - filter->q2 * filter->q2);
    filter->pitch = asinf(-2.0f * (filter->q1 * filter->q3 - filter->q0 * filter->q2));
    filter->yaw = atan2f(filter->q1 * filter->q2 + filter->q0 * filter->q3, 
                        0.5f - filter->q2 * filter->q2 - filter->q3 * filter->q3);
    filter->anglesComputed = 1;
}

//-------------------------------------------------------------------------------------------
// Public functions implementation

esp_err_t madgwick_ahrs_init(madgwick_ahrs_t *filter) {
    if (filter == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    
    memset(filter, 0, sizeof(madgwick_ahrs_t));
    filter->beta = BETA_DEF;
    filter->q0 = 1.0f;
    filter->q1 = 0.0f;
    filter->q2 = 0.0f;
    filter->q3 = 0.0f;
    filter->invSampleFreq = 1.0f / SAMPLE_FREQ_DEF;
    filter->anglesComputed = 0;
    
    return ESP_OK;
}

esp_err_t madgwick_ahrs_begin(madgwick_ahrs_t *filter, float sampleFrequency) {
    if (filter == NULL || sampleFrequency <= 0.0f) {
        return ESP_ERR_INVALID_ARG;
    }
    
    filter->invSampleFreq = 1.0f / sampleFrequency;
    return ESP_OK;
}

esp_err_t madgwick_ahrs_update(madgwick_ahrs_t *filter, 
                               float gx, float gy, float gz, 
                               float ax, float ay, float az, 
                               float mx, float my, float mz) {
    if (filter == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    
    float recipNorm;
    float s0, s1, s2, s3;
    float qDot1, qDot2, qDot3, qDot4;
    float hx, hy;
    float _2q0mx, _2q0my, _2q0mz, _2q1mx, _2bx, _2bz, _4bx, _4bz, _2q0, _2q1, _2q2, _2q3, _2q0q2, _2q2q3, q0q0, q0q1, q0q2, q0q3, q1q1, q1q2, q1q3, q2q2, q2q3, q3q3;

    // Use IMU algorithm if magnetometer measurement is invalid (avoids NaN in magnetometer normalization)
    if((mx == 0.0f) && (my == 0.0f) && (mz == 0.0f)) {
        return madgwick_ahrs_update_imu(filter, gx, gy, gz, ax, ay, az);
    }

    // Convert gyroscope from degrees/sec to radians/sec
    gx *= 0.0174533f;
    gy *= 0.0174533f;
    gz *= 0.0174533f;

    // Rate of change of quaternion from gyroscope
    qDot1 = 0.5f * (-filter->q1 * gx - filter->q2 * gy - filter->q3 * gz);
    qDot2 = 0.5f * (filter->q0 * gx + filter->q2 * gz - filter->q3 * gy);
    qDot3 = 0.5f * (filter->q0 * gy - filter->q1 * gz + filter->q3 * gx);
    qDot4 = 0.5f * (filter->q0 * gz + filter->q1 * gy - filter->q2 * gx);

    // Calculate feedback only if accelerometer measurement is valid (avoids NaN in accelerometer normalization)
    if(!((ax == 0.0f) && (ay == 0.0f) && (az == 0.0f))) {

        // Normalize accelerometer measurement
        recipNorm = inv_sqrt(ax * ax + ay * ay + az * az);
        ax *= recipNorm;
        ay *= recipNorm;
        az *= recipNorm;

        // Normalize magnetometer measurement
        recipNorm = inv_sqrt(mx * mx + my * my + mz * mz);
        mx *= recipNorm;
        my *= recipNorm;
        mz *= recipNorm;

        // Auxiliary variables to avoid repeated arithmetic
        _2q0mx = 2.0f * filter->q0 * mx;
        _2q0my = 2.0f * filter->q0 * my;
        _2q0mz = 2.0f * filter->q0 * mz;
        _2q1mx = 2.0f * filter->q1 * mx;
        _2q0 = 2.0f * filter->q0;
        _2q1 = 2.0f * filter->q1;
        _2q2 = 2.0f * filter->q2;
        _2q3 = 2.0f * filter->q3;
        _2q0q2 = 2.0f * filter->q0 * filter->q2;
        _2q2q3 = 2.0f * filter->q2 * filter->q3;
        q0q0 = filter->q0 * filter->q0;
        q0q1 = filter->q0 * filter->q1;
        q0q2 = filter->q0 * filter->q2;
        q0q3 = filter->q0 * filter->q3;
        q1q1 = filter->q1 * filter->q1;
        q1q2 = filter->q1 * filter->q2;
        q1q3 = filter->q1 * filter->q3;
        q2q2 = filter->q2 * filter->q2;
        q2q3 = filter->q2 * filter->q3;
        q3q3 = filter->q3 * filter->q3;

        // Reference direction of Earth's magnetic field
        hx = mx * q0q0 - _2q0my * filter->q3 + _2q0mz * filter->q2 + mx * q1q1 + _2q1 * my * filter->q2 + _2q1 * mz * filter->q3 - mx * q2q2 - mx * q3q3;
        hy = _2q0mx * filter->q3 + my * q0q0 - _2q0mz * filter->q1 + _2q1mx * filter->q2 - my * q1q1 + my * q2q2 + _2q2 * mz * filter->q3 - my * q3q3;
        _2bx = sqrtf(hx * hx + hy * hy);
        _2bz = -_2q0mx * filter->q2 + _2q0my * filter->q1 + mz * q0q0 + _2q1mx * filter->q3 - mz * q1q1 + _2q2 * my * filter->q3 - mz * q2q2 + mz * q3q3;
        _4bx = 2.0f * _2bx;
        _4bz = 2.0f * _2bz;

        // Passo corretivo do algoritmo de descida de gradiente
        s0 = -_2q2 * (2.0f * q1q3 - _2q0q2 - ax) + _2q1 * (2.0f * q0q1 + _2q2q3 - ay) - _2bz * filter->q2 * (_2bx * (0.5f - q2q2 - q3q3) + _2bz * (q1q3 - q0q2) - mx) + (-_2bx * filter->q3 + _2bz * filter->q1) * (_2bx * (q1q2 - q0q3) + _2bz * (q0q1 + q2q3) - my) + _2bx * filter->q2 * (_2bx * (q0q2 + q1q3) + _2bz * (0.5f - q1q1 - q2q2) - mz);
        s1 = _2q3 * (2.0f * q1q3 - _2q0q2 - ax) + _2q0 * (2.0f * q0q1 + _2q2q3 - ay) - 4.0f * filter->q1 * (1 - 2.0f * q1q1 - 2.0f * q2q2 - az) + _2bz * filter->q3 * (_2bx * (0.5f - q2q2 - q3q3) + _2bz * (q1q3 - q0q2) - mx) + (_2bx * filter->q2 + _2bz * filter->q0) * (_2bx * (q1q2 - q0q3) + _2bz * (q0q1 + q2q3) - my) + (_2bx * filter->q3 - _4bz * filter->q1) * (_2bx * (q0q2 + q1q3) + _2bz * (0.5f - q1q1 - q2q2) - mz);
        s2 = -_2q0 * (2.0f * q1q3 - _2q0q2 - ax) + _2q3 * (2.0f * q0q1 + _2q2q3 - ay) - 4.0f * filter->q2 * (1 - 2.0f * q1q1 - 2.0f * q2q2 - az) + (-_4bx * filter->q2 - _2bz * filter->q0) * (_2bx * (0.5f - q2q2 - q3q3) + _2bz * (q1q3 - q0q2) - mx) + (_2bx * filter->q1 + _2bz * filter->q3) * (_2bx * (q1q2 - q0q3) + _2bz * (q0q1 + q2q3) - my) + (_2bx * filter->q0 - _4bz * filter->q2) * (_2bx * (q0q2 + q1q3) + _2bz * (0.5f - q1q1 - q2q2) - mz);
        s3 = _2q1 * (2.0f * q1q3 - _2q0q2 - ax) + _2q2 * (2.0f * q0q1 + _2q2q3 - ay) + (-_4bx * filter->q3 + _2bz * filter->q1) * (_2bx * (0.5f - q2q2 - q3q3) + _2bz * (q1q3 - q0q2) - mx) + (-_2bx * filter->q0 + _2bz * filter->q2) * (_2bx * (q1q2 - q0q3) + _2bz * (q0q1 + q2q3) - my) + _2bx * filter->q1 * (_2bx * (q0q2 + q1q3) + _2bz * (0.5f - q1q1 - q2q2) - mz);
        recipNorm = inv_sqrt(s0 * s0 + s1 * s1 + s2 * s2 + s3 * s3); // normaliza magnitude do passo
        s0 *= recipNorm;
        s1 *= recipNorm;
        s2 *= recipNorm;
        s3 *= recipNorm;

        // Aplica passo de feedback
        qDot1 -= filter->beta * s0;
        qDot2 -= filter->beta * s1;
        qDot3 -= filter->beta * s2;
        qDot4 -= filter->beta * s3;
    }

    // Integrate quaternion rate of change to obtain quaternion
    filter->q0 += qDot1 * filter->invSampleFreq;
    filter->q1 += qDot2 * filter->invSampleFreq;
    filter->q2 += qDot3 * filter->invSampleFreq;
    filter->q3 += qDot4 * filter->invSampleFreq;

    // Normaliza quaternion
    recipNorm = inv_sqrt(filter->q0 * filter->q0 + filter->q1 * filter->q1 + filter->q2 * filter->q2 + filter->q3 * filter->q3);
    filter->q0 *= recipNorm;
    filter->q1 *= recipNorm;
    filter->q2 *= recipNorm;
    filter->q3 *= recipNorm;
    filter->anglesComputed = 0;
    
    return ESP_OK;
}

esp_err_t madgwick_ahrs_update_imu(madgwick_ahrs_t *filter, 
                                   float gx, float gy, float gz, 
                                   float ax, float ay, float az) {
    if (filter == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    
    float recipNorm;
    float s0, s1, s2, s3;
    float qDot1, qDot2, qDot3, qDot4;
    float _2q0, _2q1, _2q2, _2q3, _4q0, _4q1, _4q2 ,_8q1, _8q2, q0q0, q1q1, q2q2, q3q3;

    // Convert gyroscope from degrees/sec to radians/sec
    gx *= 0.0174533f;
    gy *= 0.0174533f;
    gz *= 0.0174533f;

    // Rate of change of quaternion from gyroscope
    qDot1 = 0.5f * (-filter->q1 * gx - filter->q2 * gy - filter->q3 * gz);
    qDot2 = 0.5f * (filter->q0 * gx + filter->q2 * gz - filter->q3 * gy);
    qDot3 = 0.5f * (filter->q0 * gy - filter->q1 * gz + filter->q3 * gx);
    qDot4 = 0.5f * (filter->q0 * gz + filter->q1 * gy - filter->q2 * gx);

    // Calculate feedback only if accelerometer measurement is valid (avoids NaN in accelerometer normalization)
    if(!((ax == 0.0f) && (ay == 0.0f) && (az == 0.0f))) {

        // Normalize accelerometer measurement
        recipNorm = inv_sqrt(ax * ax + ay * ay + az * az);
        ax *= recipNorm;
        ay *= recipNorm;
        az *= recipNorm;

        // Auxiliary variables to avoid repeated arithmetic
        _2q0 = 2.0f * filter->q0;
        _2q1 = 2.0f * filter->q1;
        _2q2 = 2.0f * filter->q2;
        _2q3 = 2.0f * filter->q3;
        _4q0 = 4.0f * filter->q0;
        _4q1 = 4.0f * filter->q1;
        _4q2 = 4.0f * filter->q2;
        _8q1 = 8.0f * filter->q1;
        _8q2 = 8.0f * filter->q2;
        q0q0 = filter->q0 * filter->q0;
        q1q1 = filter->q1 * filter->q1;
        q2q2 = filter->q2 * filter->q2;
        q3q3 = filter->q3 * filter->q3;

        // Passo corretivo do algoritmo de descida de gradiente
        s0 = _4q0 * q2q2 + _2q2 * ax + _4q0 * q1q1 - _2q1 * ay;
        s1 = _4q1 * q3q3 - _2q3 * ax + 4.0f * q0q0 * filter->q1 - _2q0 * ay - _4q1 + _8q1 * q1q1 + _8q1 * q2q2 + _4q1 * az;
        s2 = 4.0f * q0q0 * filter->q2 + _2q0 * ax + _4q2 * q3q3 - _2q3 * ay - _4q2 + _8q2 * q1q1 + _8q2 * q2q2 + _4q2 * az;
        s3 = 4.0f * q1q1 * filter->q3 - _2q1 * ax + 4.0f * q2q2 * filter->q3 - _2q2 * ay;
        recipNorm = inv_sqrt(s0 * s0 + s1 * s1 + s2 * s2 + s3 * s3); // normaliza magnitude do passo
        s0 *= recipNorm;
        s1 *= recipNorm;
        s2 *= recipNorm;
        s3 *= recipNorm;

        // Aplica passo de feedback
        qDot1 -= filter->beta * s0;
        qDot2 -= filter->beta * s1;
        qDot3 -= filter->beta * s2;
        qDot4 -= filter->beta * s3;
    }

    // Integrate quaternion rate of change to obtain quaternion
    filter->q0 += qDot1 * filter->invSampleFreq;
    filter->q1 += qDot2 * filter->invSampleFreq;
    filter->q2 += qDot3 * filter->invSampleFreq;
    filter->q3 += qDot4 * filter->invSampleFreq;

    // Normaliza quaternion
    recipNorm = inv_sqrt(filter->q0 * filter->q0 + filter->q1 * filter->q1 + filter->q2 * filter->q2 + filter->q3 * filter->q3);
    filter->q0 *= recipNorm;
    filter->q1 *= recipNorm;
    filter->q2 *= recipNorm;
    filter->q3 *= recipNorm;
    filter->anglesComputed = 0;
    
    return ESP_OK;
}

float madgwick_ahrs_get_roll(madgwick_ahrs_t *filter) {
    if (filter == NULL) {
        return 0.0f;
    }
    if (!filter->anglesComputed) {
        compute_angles(filter);
    }
    return filter->roll * 57.29578f;
}

float madgwick_ahrs_get_pitch(madgwick_ahrs_t *filter) {
    if (filter == NULL) {
        return 0.0f;
    }
    if (!filter->anglesComputed) {
        compute_angles(filter);
    }
    return filter->pitch * 57.29578f;
}

float madgwick_ahrs_get_yaw(madgwick_ahrs_t *filter) {
    if (filter == NULL) {
        return 0.0f;
    }
    if (!filter->anglesComputed) {
        compute_angles(filter);
    }
    return filter->yaw * 57.29578f + 180.0f;
}

float madgwick_ahrs_get_roll_radians(madgwick_ahrs_t *filter) {
    if (filter == NULL) {
        return 0.0f;
    }
    if (!filter->anglesComputed) {
        compute_angles(filter);
    }
    return filter->roll;
}

float madgwick_ahrs_get_pitch_radians(madgwick_ahrs_t *filter) {
    if (filter == NULL) {
        return 0.0f;
    }
    if (!filter->anglesComputed) {
        compute_angles(filter);
    }
    return filter->pitch;
}

float madgwick_ahrs_get_yaw_radians(madgwick_ahrs_t *filter) {
    if (filter == NULL) {
        return 0.0f;
    }
    if (!filter->anglesComputed) {
        compute_angles(filter);
    }
    return filter->yaw;
}

esp_err_t madgwick_ahrs_set_beta(madgwick_ahrs_t *filter, float beta) {
    if (filter == NULL || beta < 0.0f) {
        return ESP_ERR_INVALID_ARG;
    }
    filter->beta = beta;
    return ESP_OK;
}

float madgwick_ahrs_get_beta(madgwick_ahrs_t *filter) {
    if (filter == NULL) {
        return 0.0f;
    }
    return filter->beta;
}


