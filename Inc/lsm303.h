
#ifndef LSM303_H_
#define LSM303_H_

#include <stdint.h>
#include "stm32f7xx_hal.h"

#define I2C_ACC_READ	0x33
#define I2C_ACC_WRITE	0x32
#define I2C_MAG_READ	0x3d
#define I2C_MAG_WRITE	0x3c

#define CTRL_REG1_A		0x20
#define CTRL_REG4_A		0x21
#define OUT_X_L_A		0x28
#define OUT_X_H_A		0x29
#define OUT_Y_L_A		0x2a
#define OUT_Y_H_A		0x2b
#define OUT_Z_L_A		0x2c
#define OUT_z_H_A		0x2d

#define CRA_REG_M		0x00
#define CRB_REG_M		0x01
#define MR_REG_M		0x02
#define OUT_X_H_M		0x03
#define OUT_X_L_M		0x04
#define OUT_Z_H_M		0x05
#define OUT_Z_L_M		0x06
#define OUT_Y_H_M		0x07
#define OUT_Y_L_M		0x08

#define ACC_50HZ		0x40
#define ACC_XYZ_ENABLE	0x07
#define ACC_BDU			0x80
#define MAG_30HZ		0x14
#define MAG_CONTINUOUS	0x00
#define MAG_SINGLE		0x01

#define I2C_TIMEOUT		10
#define SAMPLE_SIZE		6
#define I2C_SUBADDR_MULTI 0x80

typedef struct {
	int16_t x,y,z;
} sample;

void readAccSample(I2C_HandleTypeDef *hi2c, sample* accSample);

void readMagSample(I2C_HandleTypeDef *hi2c, sample* magSample);

void writeAccConfig(I2C_HandleTypeDef *hi2c);

void writeMagConfig(I2C_HandleTypeDef *hi2c);

void setMagCalibValues(sample offsets);

#endif /* LSM303_H_ */
