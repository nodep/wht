#ifndef MPU_H
#define MPU_H

typedef struct
{
	uint8_t		mag_sensor;		// 0 - none, 1 - AKM, 2 - HMC
	int16_t		quat[4];
	int16_t		accel[3];
	int16_t		gyro[3];
	int16_t		mag[3];			// the magnetometer readings: x, y, x
} mpu_readings_t;

void mpu_init(void);
void mpu_shutdown(void);
bool dmp_read_fifo(mpu_readings_t* mpu_rd);

int16_t mpu_read_temperature(void);
void mpu_read_compass(mpu_readings_t* mpu_rd);

void mpu_calibrate_bias(void);

void akm_self_test(int16_t* mag_self_test);
void hmc_self_test(int16_t* mag_self_test);

#endif	// MPU_H
