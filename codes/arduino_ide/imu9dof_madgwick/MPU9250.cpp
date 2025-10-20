#include "MPU9250.h"
#include <Arduino.h>
#include <Wire.h>

MPU9250::MPU9250()
    : gyroScale(GYRO_SCALE), accelScale(ACCEL_SCALE), magScale(0.15f) {}

bool MPU9250::writeRegister(uint8_t deviceAddr, uint8_t regAddr, uint8_t data) {
    Wire.beginTransmission(deviceAddr);
    Wire.write(regAddr);
    Wire.write(data);
    return (Wire.endTransmission() == 0);
}

bool MPU9250::readRegister(uint8_t deviceAddr, uint8_t regAddr, uint8_t* data, uint8_t length) {
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

uint8_t MPU9250::readRegister(uint8_t deviceAddr, uint8_t regAddr) {
    uint8_t data;
    readRegister(deviceAddr, regAddr, &data, 1);
    return data;
}

bool MPU9250::mpu9250_init() {
    if (!writeRegister(MPU9250_ADDR, PWR_MGMT_1, 0x00)) {
        return false;
    }
    delay(100);

    if (!writeRegister(MPU9250_ADDR, ACCEL_CONFIG, ACCEL_FS)) {
        return false;
    }
    accelScale = ACCEL_SCALE;

    if (!writeRegister(MPU9250_ADDR, GYRO_CONFIG, GYRO_FS)) {
        return false;
    }
    gyroScale = GYRO_SCALE;

    if (!writeRegister(MPU9250_ADDR, CONFIG_REG, 0x04)) {
        return false;
    }

    if (!writeRegister(MPU9250_ADDR, SMPLRT_DIV, 0x09)) {
        return false;
    }

    if (!writeRegister(MPU9250_ADDR, INT_ENABLE, 0x01)) {
        return false;
    }

    return true;
}

bool MPU9250::ak8963_init() {
    if (!writeRegister(MPU9250_ADDR, INT_PIN_CFG, 0x02)) {
        return false;
    }
    delay(10);

    if (!writeRegister(AK8963_ADDR, CNTL1, 0x16)) {
        return false;
    }
    delay(10);

    magScale = 0.15f;
    return true;
}

bool MPU9250::readIMU(float* ax, float* ay, float* az, float* gx, float* gy, float* gz) {
    uint8_t data[14];
    if (!readRegister(MPU9250_ADDR, ACCEL_XOUT_H, data, 14)) {
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

bool MPU9250::readMag(float* mx, float* my, float* mz) {
    uint8_t data[7];
    if (!readRegister(AK8963_ADDR, ST1, data, 7)) {
        return false;
    }

    if (!(data[0] & 0x01)) {
        return false;
    }

    if (data[6] & 0x08) {
        return false;
    }

    int16_t rawX = (int16_t)((data[2] << 8) | data[1]);
    int16_t rawY = (int16_t)((data[4] << 8) | data[3]);
    int16_t rawZ = (int16_t)((data[6] << 8) | data[5]);

    *mx = rawX * magScale;
    *my = rawY * magScale;
    *mz = rawZ * magScale;

    return true;
}


