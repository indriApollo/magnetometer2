
#include "lsm303.h"

// default values via sphere fit
static int16_t OFFSET_X = 227;
static int16_t OFFSET_Y = 117;
static int16_t OFFSET_Z = -265;

void writeAccConfig(I2C_HandleTypeDef *hi2c)
{
	uint8_t value[4] = {0};
	value[0] = (ACC_50HZ|ACC_XYZ_ENABLE);
	value[3] = ACC_BDU;
	HAL_I2C_Mem_Write(hi2c, I2C_ACC_WRITE, CTRL_REG1_A|I2C_SUBADDR_MULTI, I2C_MEMADD_SIZE_8BIT, value, sizeof(value), I2C_TIMEOUT);
}

void writeMagConfig(I2C_HandleTypeDef *hi2c)
{
	uint8_t value[3] = {0};
	value[0] = MAG_30HZ;
	value[2] = MAG_CONTINUOUS;
	HAL_I2C_Mem_Write(hi2c, I2C_MAG_WRITE, CRA_REG_M|I2C_SUBADDR_MULTI, I2C_MEMADD_SIZE_8BIT, value, sizeof(value), I2C_TIMEOUT);
}

void readAccSample(I2C_HandleTypeDef *hi2c, sample* accSample)
{
	HAL_I2C_Mem_Read(hi2c, I2C_ACC_READ, OUT_X_L_A|I2C_SUBADDR_MULTI, I2C_MEMADD_SIZE_8BIT, (uint8_t*)accSample, SAMPLE_SIZE, I2C_TIMEOUT);
}

void readMagSample(I2C_HandleTypeDef *hi2c, sample* magSample)
{
	uint8_t value[SAMPLE_SIZE];
	HAL_I2C_Mem_Read(hi2c, I2C_MAG_READ, OUT_X_H_M|I2C_SUBADDR_MULTI, I2C_MEMADD_SIZE_8BIT, value, SAMPLE_SIZE, I2C_TIMEOUT);
	// re-order MSB,LSB
	magSample->x = (int16_t)( (uint16_t)(value[0]<<8)|value[1] )+OFFSET_X;
	magSample->y = (int16_t)( (uint16_t)(value[4]<<8)|value[5] )+OFFSET_Y;
	magSample->z = (int16_t)( (uint16_t)(value[2]<<8)|value[3] )+OFFSET_Z;
}

void setMagCalibValues(sample offsets)
{
	// we subtract because the lsm303 is upside down
	OFFSET_X -= offsets.x;
	OFFSET_Y -= offsets.y;
	OFFSET_Z -= offsets.z;
}
