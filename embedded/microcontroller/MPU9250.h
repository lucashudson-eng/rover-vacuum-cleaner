#pragma once
#include <stdint.h>

#define GYRO_FS_250 0x00
#define GYRO_FS_500 0x08
#define GYRO_FS_1000 0x10
#define GYRO_FS_2000 0x18

#define ACCEL_FS_2 0x00
#define ACCEL_FS_4 0x08
#define ACCEL_FS_8 0x10
#define ACCEL_FS_16 0x18

#define GYRO_FS GYRO_FS_250
#define ACCEL_FS ACCEL_FS_2
#define GYRO_SCALE 250.0f / 32768.0f
#define ACCEL_SCALE 2.0f / 32768.0f

class MPU9250 {
public:
    MPU9250();

    bool mpu9250_init();
    bool ak8963_init();

    bool readIMU(float* ax, float* ay, float* az, float* gx, float* gy, float* gz);
    bool readMag(float* mx, float* my, float* mz);

private:
    // I2C addresses
    static constexpr uint8_t MPU9250_ADDR = 0x68;
    static constexpr uint8_t AK8963_ADDR = 0x0C;

    // MPU9250 registers
    static constexpr uint8_t PWR_MGMT_1   = 0x6B;
    static constexpr uint8_t CONFIG_REG   = 0x1A;
    static constexpr uint8_t GYRO_CONFIG  = 0x1B;
    static constexpr uint8_t ACCEL_CONFIG = 0x1C;
    static constexpr uint8_t SMPLRT_DIV   = 0x19;
    static constexpr uint8_t INT_PIN_CFG  = 0x37;
    static constexpr uint8_t INT_ENABLE   = 0x38;
    static constexpr uint8_t ACCEL_XOUT_H = 0x3B;

    // AK8963 registers
    static constexpr uint8_t ST1   = 0x02;
    static constexpr uint8_t HXL   = 0x03; // not used directly, kept for reference
    static constexpr uint8_t CNTL1 = 0x0A;

    float gyroScale;
    float accelScale;
    float magScale;

    bool writeRegister(uint8_t deviceAddr, uint8_t regAddr, uint8_t data);
    bool readRegister(uint8_t deviceAddr, uint8_t regAddr, uint8_t* data, uint8_t length);
    uint8_t readRegister(uint8_t deviceAddr, uint8_t regAddr);
};
