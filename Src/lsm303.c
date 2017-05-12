
#include "lsm303.h"

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
	HAL_I2C_Mem_Read(hi2c, I2C_ACC_READ, OUT_X_L_A|I2C_SUBADDR_MULTI, I2C_MEMADD_SIZE_8BIT, value, SAMPLE_SIZE, I2C_TIMEOUT);
	// re-oder MSB,LSB
	magSample->x = (int16_t)( ((uint16_t)value[0]<<8)|value[1] );
	magSample->y = (int16_t)( ((uint16_t)value[4]<<8)|value[5] );
	magSample->z = (int16_t)( ((uint16_t)value[2]<<8)|value[3] );
}
