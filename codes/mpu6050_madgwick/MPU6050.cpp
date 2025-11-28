#include "MPU6050.h"
#include <Arduino.h>
#include <Wire.h>

MPU6050::MPU6050()
    : gyroScale(GYRO_SCALE), accelScale(ACCEL_SCALE) {}

bool MPU6050::writeRegister(uint8_t deviceAddr, uint8_t regAddr, uint8_t data) {
    Wire.beginTransmission(deviceAddr);
    Wire.write(regAddr);
    Wire.write(data);
    return (Wire.endTransmission() == 0);
}

bool MPU6050::readRegister(uint8_t deviceAddr, uint8_t regAddr, uint8_t* data, uint8_t length) {
    Wire.beginTransmission(deviceAddr);
    Wire.write(regAddr);
    if (Wire.endTransmission(false) != 0) {
        return false;
    }

    Wire.requestFrom(deviceAddr, length);
    if (Wire.available() != length) {
        return false;
    }

    for (uint8_t i = 0; i < length; i++) {
        data[i] = Wire.read();
    }
    return true;
}

uint8_t MPU6050::readRegister(uint8_t deviceAddr, uint8_t regAddr) {
    uint8_t data;
    readRegister(deviceAddr, regAddr, &data, 1);
    return data;
}

bool MPU6050::mpu6050_init() {
    if (!writeRegister(MPU6050_ADDR, PWR_MGMT_1, 0x00)) {
        return false;
    }
    delay(100);

    if (!writeRegister(MPU6050_ADDR, ACCEL_CONFIG, ACCEL_FS)) {
        return false;
    }
    accelScale = ACCEL_SCALE;

    if (!writeRegister(MPU6050_ADDR, GYRO_CONFIG, GYRO_FS)) {
        return false;
    }
    gyroScale = GYRO_SCALE;

    if (!writeRegister(MPU6050_ADDR, CONFIG_REG, 0x04)) {
        return false;
    }

    if (!writeRegister(MPU6050_ADDR, SMPLRT_DIV, 0x09)) {
        return false;
    }

    if (!writeRegister(MPU6050_ADDR, INT_ENABLE, 0x01)) {
        return false;
    }

    return true;
}

bool MPU6050::readIMU(float* ax, float* ay, float* az, float* gx, float* gy, float* gz) {
    uint8_t data[14];
    if (!readRegister(MPU6050_ADDR, ACCEL_XOUT_H, data, 14)) {
        return false;
    }

    int16_t rawAX = (int16_t)((data[0] << 8) | data[1]);
    int16_t rawAY = (int16_t)((data[2] << 8) | data[3]);
    int16_t rawAZ = (int16_t)((data[4] << 8) | data[5]);

    int16_t rawGX = (int16_t)((data[8] << 8) | data[9]);
    int16_t rawGY = (int16_t)((data[10] << 8) | data[11]);
    int16_t rawGZ = (int16_t)((data[12] << 8) | data[13]);

    *ax = rawAX * accelScale;
    *ay = rawAY * accelScale;
    *az = rawAZ * accelScale;
    *gx = rawGX * gyroScale;
    *gy = rawGY * gyroScale;
    *gz = rawGZ * gyroScale;

    return true;
}

