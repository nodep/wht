#include <stdio.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include <reg24le1.h>
#include <nrfutils.h>

#include "i2c.h"
#include "tracker.h"
#include "sensor_regs.h"
#include "sensors.h"
#include "sensor_dmp_firmware.h"
#include "rf_protocol.h"
#include "tracker_settings.h"
#include "nrfdbg.h"

// This library configures the MPU chips in DMP mode and it reads the temperature and compass
// data separately.
// It also handles the HMC5883L and reads the compass data if that sensor is present instead
// of the AK8975A (which is a part of MPU-9150)

enum mag_type_e
{
	mt_none,
	mt_ak,		// AK8975A
	mt_hmc,		// HMC5883L
};

uint8_t mag_type = mt_none;
int16_t mag_sens_adj[3];

bool mpu_write_byte(uint8_t reg_addr, uint8_t val)
{
	return i2c_write(MPU_ADDR, reg_addr, 1, &val);
}

bool mpu_read_byte(uint8_t reg_addr, uint8_t* val)
{
	return i2c_read(MPU_ADDR, reg_addr, 1, val);
}

bool mpu_read_array(uint8_t reg_addr, uint8_t bytes, uint8_t* val)
{
	return i2c_read(MPU_ADDR, reg_addr, bytes, val);
}

bool mpu_write_array(uint8_t reg_addr, uint8_t bytes, const uint8_t* val)
{
	return i2c_write(MPU_ADDR, reg_addr, bytes, val);
}

bool ak_write_byte(uint8_t reg_addr, uint8_t val)
{
	return i2c_write(AK8975A_ADDR, reg_addr, 1, &val);
}

bool ak_read_byte(uint8_t reg_addr, uint8_t* val)
{
	return i2c_read(AK8975A_ADDR, reg_addr, 1, val);
}

bool ak_read_array(uint8_t reg_addr, uint8_t cnt, uint8_t* val)
{
	return i2c_read(AK8975A_ADDR, reg_addr, cnt, val);
}

bool hmc_read_array(uint8_t reg_addr, uint8_t cnt, uint8_t* val)
{
	return i2c_read(HMC5883L_ADDR, reg_addr, cnt, val);
}

// *************************************************************

bool mpu_write_mem(uint16_t mem_addr, uint16_t length, const uint8_t* data2write)
{
	uint8_t tmp[2];

	// swap bytes
	tmp[0] = (uint8_t)(mem_addr >> 8);
	tmp[1] = (uint8_t)(mem_addr & 0xFF);

	if (!mpu_write_array(BANK_SEL, 2, tmp))
		return false;

	if (!mpu_write_array(MEM_R_W, length, data2write))
		return false;

	return true;
}

bool mpu_read_mem(uint16_t mem_addr, uint16_t length, uint8_t* data2read)
{
	uint8_t tmp[2];

	tmp[0] = (uint8_t)(mem_addr >> 8);
	tmp[1] = (uint8_t)(mem_addr & 0xFF);

	if (!mpu_write_array(BANK_SEL, 2, tmp))
		return false;

	if (!mpu_read_array(MEM_R_W, length, data2read))
		return false;

	return true;
}

// **************************************************************************************

bool dmp_load_firmware(void)
{
#define LOAD_CHUNK		16
#define START_ADDR		0x0400
#define MAX_RETRY		5

	uint16_t ii, this_write;

	uint8_t cur[LOAD_CHUNK], tmp[2];

	for (ii = 0; ii < DMP_CODE_SIZE; ii += this_write)
	{
		this_write = DMP_CODE_SIZE - ii;
		if (this_write > LOAD_CHUNK)
			this_write = LOAD_CHUNK;

		if (!mpu_write_mem(ii, this_write, dmp_memory + ii))
		{
			dputs("write failed");
			return false;
		}

		if (!mpu_read_mem(ii, this_write, cur))
		{
			dputs("read failed");
			return false;
		}

		if (memcmp(dmp_memory + ii, cur, this_write))
		{
			dputs("verify failed");
			return false;
		}
	}

	// Set program start address.
	tmp[0] = START_ADDR >> 8;
	tmp[1] = START_ADDR & 0xFF;
	if (!mpu_write_array(PRGM_START_H, 2, tmp))
	{
		dputs("PRGM_START_H failed");
		return false;
	}

	return true;
}

bool dmp_set_orientation(void)
{
	const uint8_t __code arr1[3] = {0xCD, 0x4C, 0x6C};
	const uint8_t __code arr2[3] = {0xC9, 0x0C, 0x2C};
	const uint8_t __code arr3[3] = {0x36, 0x57, 0x76};
	const uint8_t __code arr4[3] = {0x26, 0x47, 0x66};

	return mpu_write_mem(FCFG_1, 3, arr1)
			&& mpu_write_mem(FCFG_2, 3, arr2)
			&& mpu_write_mem(FCFG_3, 3, arr3)
			&& mpu_write_mem(FCFG_7, 3, arr4);
}

void mpu_set_gyro_bias(const int16_t* gyro_bias)
{
	uint8_t d[2], i;

	for (i = 0; i < 3; i++)
	{
		d[0] = (gyro_bias[i] >> 8) & 0xff;
		d[1] = (gyro_bias[i]) & 0xff;
		mpu_write_array(0x13 + 2 * i, 2, d);
	}
}

void mpu_read_accel_bias(int16_t* accel_bias)
{
	uint8_t d[2], i;

	for (i = 0; i < 3; i++)
	{
		mpu_read_array(0x06 + i * 2, 2, d);
		accel_bias[i] = (d[0] << 8) | d[1];
	}
}

void mpu_set_accel_bias(const int16_t* accel_bias)
{
	uint8_t i;
	uint8_t data[2];

	// Bit 0 of the 2 byte bias is for temp comp
	// calculations need to compensate for this and not change it
	//
	// Unlike the eMPL library, we achieve this by only incrementing/decrementing the
	// accel bias with even values, thus preserving the parity of the bias and the least
	// significant bit.
	//
	// However, it looks like MPU is temperature dependant, and doesn't do compensation
	// regardless of the value of the last bit...
	for (i = 0; i < 3; i++)
	{
		data[0] = (accel_bias[i] >> 8) & 0xff;
		data[1] = accel_bias[i] & 0xff;

		mpu_write_array(0x06 + i * 2, 2, data);
	}
}

void dmp_enable_feature(void)
{
	// it would be interesting to see what would happen if I incorporated these values directly
	// into the DMP firmware image, and not write them after the image is already uploaded.

	{
	const uint8_t __code arr[] = {0x02,0xca,0xe3,0x09};
	mpu_write_mem(D_0_104, sizeof arr, arr);
	}

	{
	const uint8_t __code arr[] = {0xa3,0xc0,0xc8,0xc2,0xc4,0xcc,0xc6,0xa3,0xa3,0xa3};
	mpu_write_mem(CFG_15, sizeof arr, arr);
	}

	{	// tap is off
	const uint8_t __code arr[] = {0xd8};
	mpu_write_mem(CFG_27, sizeof arr, arr);
	}

	{
	const uint8_t __code arr[] = {0xb8,0xaa,0xb3,0x8d,0xb4,0x98,0x0d,0x35,0x5d};	// dmp_enable_gyro_cal(1)
	//const uint8_t __code arr[] = {0xb8,0xaa,0xaa,0xaa,0xb0,0x88,0xc3,0xc5,0xc7};	// dmp_enable_gyro_cal(0)
	mpu_write_mem(CFG_MOTION_BIAS, sizeof arr, arr);
	}

	{
	const uint8_t __code arr[] = {0xB0,0x80,0xB4,0x90};		// DMP_FEATURE_SEND_RAW_GYRO
	mpu_write_mem(CFG_GYRO_RAW_DATA, sizeof arr, arr);
	}

	{
	const uint8_t __code arr[] = {0xf8};
	mpu_write_mem(CFG_20, sizeof arr, arr);
	}

	{
	const uint8_t __code arr[] = {0xd8};
	mpu_write_mem(CFG_ANDROID_ORIENT_INT, sizeof arr, arr);
	}
	{
	const uint8_t __code arr[] = {0x8b,0x8b,0x8b,0x8b};
	mpu_write_mem(CFG_LP_QUAT, sizeof arr, arr);
	}
	{
	const uint8_t __code arr[] = {0x20,0x28,0x30,0x38};
	mpu_write_mem(CFG_8, sizeof arr, arr);
	}

	// this is dmp_set_fifo_rate()
	// the 0x03 is the divider of the 200 Hz DMS sample rate
	{
	const uint8_t __code arr[] = {0x00,0x03};
	mpu_write_mem(D_0_22, sizeof arr, arr);
	}
	{
	const uint8_t __code arr[] = {0xfe,0xf2,0xab,0xc4,0xaa,0xf1,0xdf,0xdf,0xbb,0xaf,0xdf,0xdf};
	mpu_write_mem(CFG_6, sizeof arr, arr);
	}
}

void mpu_set_bypass(uint8_t bypass)
{
	uint8_t byte, pin_cfg;

	mpu_read_byte(USER_CTRL, &byte);

	if (bypass)
	{
		byte &= ~BIT_I2C_MST_EN;
		pin_cfg = BIT_I2C_BYPASS_EN | BIT_INT_LEVEL;
	} else {
		byte |= BIT_I2C_MST_EN;
		pin_cfg = BIT_INT_LEVEL;
	}

	mpu_write_byte(USER_CTRL, byte);

	delay_ms(3);

	mpu_write_byte(INT_PIN_CFG, pin_cfg);
}

// data sizes
#define TEMP_DATA_SIZE		2
#define COMPASS_DATA_SIZE	8
#define QUAT_DATA_SIZE		16
#define ACCEL_DATA_SIZE		6
#define GYRO_DATA_SIZE		6

#define FIFO_BUFFER_CAPACITY	(QUAT_DATA_SIZE + ACCEL_DATA_SIZE + GYRO_DATA_SIZE)

bool mpu_read_fifo_packet(uint8_t* buffer)
{
	uint8_t tmp[2], chunk_bytes;
	uint16_t fifo_count;

	// read number of bytes in the FIFO
	if (!mpu_read_array(FIFO_COUNT_H, 2, tmp))
		return false;

	// mind the endianness
	fifo_count = (tmp[0] << 8) | tmp[1];

	if (fifo_count == 0)
		return false;

	// dprintf("! %d !\n", fifo_count);

	do {
		// read bytes up to the number of bytes in out packet
		chunk_bytes = (FIFO_BUFFER_CAPACITY < fifo_count ? FIFO_BUFFER_CAPACITY : fifo_count);

		mpu_read_array(FIFO_R_W, chunk_bytes, buffer);

		// decrease the number of bytes remaining
		fifo_count -= chunk_bytes;

	} while (fifo_count);

	// we had a success only if the size of the last packet read is equal to the
	// expected FIFO packet size
	return chunk_bytes == FIFO_BUFFER_CAPACITY;
}

bool dmp_read_fifo(mpu_readings_t* pckt)
{
	uint8_t fifo_data[FIFO_BUFFER_CAPACITY];
	uint8_t i;

	if (!mpu_read_fifo_packet(fifo_data))
		return false;

	// we're truncating the lower 16 bits of the quaternions. only the higher 16 bits are really
	// used in the calculations, so there's no point in dragging the entire 32 bit integer around.
	for (i = 0; i < 4; i++)
		pckt->quat[i] = (fifo_data[i*4] << 8) | fifo_data[1 + i*4];

	// we don't directly use the accel and the gyro data at the moment,
	// but this will change in the future
	for (i = 0; i < 3; i++)
		pckt->accel[i] = (fifo_data[16 + i*2] << 8) | fifo_data[17 + i*2];

	for (i = 0; i < 3; i++)
		pckt->gyro[i] = (fifo_data[22 + i*2] << 8) | fifo_data[23 + i*2];

	return true;
}

// temperature calculation functions
#define TEMP_OFFSET		521
#define TEMP_SENS		34

int16_t mpu_read_temperature(void)
{
	uint8_t buff[2];

	// get the raw value
	mpu_read_array(TEMP_OUT_H, 2, buff);

	// result in tenths of Celsius
	return (int16_t)(350 + ((((buff[0] << 8) | buff[1]) + TEMP_OFFSET) / TEMP_SENS));
}

void mpu_read_compass(mpu_readings_t* pckt)
{
	uint8_t buff[8], i;

	pckt->mag_sensor = 0;

	// read the compass data if we are running on a MPU-9150
	if (mag_type == mt_ak)
	{
		mpu_read_array(RAW_COMPASS, 8, buff);

		if ((buff[0] & AKM_DATA_READY) == 0  ||  (buff[7] & AKM_OVERFLOW) || (buff[7] & AKM_DATA_ERROR))
			return;

		for (i = 0; i < 3; i++)
		{
			int32_t data = (buff[2 + i*2] << 8) | buff[1 + i*2];
			pckt->mag[i] = (data * mag_sens_adj[i]) >> 8;
		}

		pckt->mag_sensor = 1;		// AKM

	} else if (mag_type == mt_hmc) {

		// read the HKM5883L data
		
		mpu_read_array(RAW_COMPASS, 7, buff);

		// check the status register (todo...
		// if (buff[6])

		// mind which is which axis
		pckt->mag[0] = (buff[4] << 8) | buff[5];
		pckt->mag[1] = (buff[0] << 8) | buff[1];
		pckt->mag[2] = (buff[2] << 8) | buff[3];

		pckt->mag[2] = -pckt->mag[2];

		//dprintf("%d %d %d\n", pckt->mag[0], pckt->mag[1], pckt->mag[2]);

		pckt->mag_sensor = 2;		// HMC
		
		dprintf("%d %d %d R%cL%c\n", pckt->mag[0], pckt->mag[1], pckt->mag[2],
								(buff[6] & HMC_STATUS_RDY) ? '1' : '0',
								(buff[6] & HMC_STATUS_LOCK) ? '1' : '0');
	}
}

void mpu_load_biases(void)
{
	const tracker_settings_t* pSettings = get_tracker_settings();

	if (pSettings  &&  pSettings->is_calibrated)
	{
		//dprintf("%s\ngyro %d %d %d\naccel %d %d %d\n",
		//				"loading",
		//				pSettings->gyro_bias[0], pSettings->gyro_bias[1], pSettings->gyro_bias[2],
		//				pSettings->accel_bias[0], pSettings->accel_bias[1], pSettings->accel_bias[2]);

		mpu_set_gyro_bias(pSettings->gyro_bias);
		mpu_set_accel_bias(pSettings->accel_bias);
	} else {
		dputs("no settings saved");
	}
}

void akm_self_test(int16_t* mag_self_test)
{
    uint8_t tmp[6];
    uint8_t tries = 10;

    mpu_set_bypass(1);

	ak_write_byte(AKM_REG_CNTL, AKM_POWER_DOWN);
    ak_write_byte(AKM_REG_ASTC, AKM_BIT_SELF_TEST);
    ak_write_byte(AKM_REG_CNTL, AKM_MODE_SELF_TEST);

    do {
        delay_ms(10);
        ak_read_byte(AKM_REG_ST1, tmp);
    } while (tries-- > 0  &&  (tmp[0] & AKM_DATA_READY) == 0);

    if (tmp[0] & AKM_DATA_READY)
	{
		ak_read_array(AKM_REG_HXL, 6, tmp);

		mag_self_test[0] = (int16_t)(tmp[1] << 8) | tmp[0];
		//if ((data > 100) || (data < -100))
		//    result |= 0x01;
		mag_self_test[1] = (int16_t)(tmp[3] << 8) | tmp[2];
		//if ((data > 100) || (data < -100))
		//    result |= 0x02;
		mag_self_test[2] = (int16_t)(tmp[5] << 8) | tmp[4];
		//if ((data > -300) || (data < -1000))
		//    result |= 0x04;

		//dprintf("%d %d %d\n", x,y,z);
	}

    ak_write_byte(AKM_REG_ASTC, 0);
    ak_write_byte(AKM_REG_CNTL, 0);

    mpu_set_bypass(0);
}

#define SAMPLE_RATE_HZ		200

void mpu_setup_compass(void)
{
	uint8_t data[3], i;

	data[0] = 'a';
	data[1] = 'a';
	data[2] = 'a';

	mpu_set_bypass(1);

	mag_type = mt_none;

	// do we have a MPU-9150?
	data[0] = 0;
	if (ak_read_byte(AKM_REG_WHOAMI, data)  &&  data[0] == AKM_WHOAMI)
	{
		mag_type = mt_ak;
		dputs("MPU9150 detected");

		ak_write_byte(AKM_REG_CNTL, AKM_POWER_DOWN);
		delay_ms(1);
		ak_write_byte(AKM_REG_CNTL, AKM_FUSE_ROM_ACCESS);

		ak_read_array(AKM_REG_ASAX, 3, data);

		// calc the mag adjustments
		for (i = 0; i < 3; i++)
			mag_sens_adj[i] = data[i] + 128;

		// dprintf("mag_sens_adj %d %d %d\n", mag_sens_adj[0], mag_sens_adj[1], mag_sens_adj[2]);

		ak_write_byte(AKM_REG_CNTL, AKM_POWER_DOWN);
		delay_ms(1);

	} else if (hmc_read_array(HMC_REG_ID_A, 3, data)
				&&  data[0] == 'H'  &&  data[1] == '4'  &&  data[2] == '3') {		// try the HMC5883

		mag_type = mt_hmc;
		dputs("HMC5883L detected");

		data[0] = HMC_CFGA_AVG2 | HMC_CFGA_DRT_15 | HMC_CFGA_MSR_MODE_NORM;
		i2c_write(HMC5883L_ADDR, HMC_REG_CONFIG_A, 1, data);
		
	} else {
		dputs("magnetometer NOT detected");
	}

	mpu_set_bypass(0);

	if (mag_type == mt_ak)
	{
		mpu_write_byte(I2C_MST_CTRL, BIT_WAIT_FOR_ES);				// Set up master mode, master clock, and ES bit.

		mpu_write_byte(S0_ADDR, BIT_I2C_READ | AK8975A_ADDR);		// Slave 0 reads from AKM data registers.
		mpu_write_byte(S0_REG, AKM_REG_ST1);						// Compass reads start at this register.
		mpu_write_byte(S0_CTRL, BIT_I2C_SLVx_EN | 8);				// Enable slave 0, 8-byte reads.

		mpu_write_byte(S1_ADDR, AK8975A_ADDR);						// Slave 1 changes AKM measurement mode.
		mpu_write_byte(S1_REG, AKM_REG_CNTL);						// AKM measurement mode register.
		mpu_write_byte(S1_CTRL, BIT_I2C_SLVx_EN | 1);				// Enable slave 1, 1-byte writes.

		mpu_write_byte(S1_DO, AKM_SINGLE_MEASUREMENT);				// Set slave 1 data.

		// Trigger slave 0 and slave 1 actions at each sample.
		mpu_write_byte(I2C_MST_DELAY_CTRL, BIT_I2C_SLV1_DLY_EN | BIT_I2C_SLV0_DLY_EN);

		mpu_write_byte(YG_OFFS_TC, BIT_I2C_MST_VDDIO);				// For the MPU9150, the auxiliary I2C bus needs to be set to VDD.

		mpu_write_byte(S4_CTRL, SAMPLE_RATE_HZ / 100 - 1);			// compass sample rate

	} else if (mag_type == mt_hmc) {

		mpu_write_byte(I2C_MST_CTRL, BIT_WAIT_FOR_ES);				// set up master mode, master clock, and ES bit

		mpu_write_byte(S0_ADDR, BIT_I2C_READ | HMC5883L_ADDR);		// slave 0 reads from HMC data registers
		mpu_write_byte(S0_REG, HMC_REG_DATA_X_MSB);					// compass reads start at this register
		mpu_write_byte(S0_CTRL, BIT_I2C_SLVx_EN | 7);				// enable slave 0, 7 byte reads

		mpu_write_byte(S1_ADDR, HMC5883L_ADDR);						// slave 1 changes HMC measurement mode
		mpu_write_byte(S1_REG, HMC_REG_MODE);						// HMC measurement mode register
		mpu_write_byte(S1_CTRL, BIT_I2C_SLVx_EN | 1);				// enable slave 1, 1 byte writes

		mpu_write_byte(S1_DO, HMC_MODE_SINGLE_MSRMT);				// set slave 1 data

		// trigger slave 0 and slave 1 actions at each sample
		mpu_write_byte(I2C_MST_DELAY_CTRL, BIT_I2C_SLV1_DLY_EN | BIT_I2C_SLV0_DLY_EN);

		mpu_write_byte(YG_OFFS_TC, BIT_I2C_MST_VDDIO);				// the auxiliary I2C bus needs to be set to VDD.

		mpu_write_byte(S4_CTRL, SAMPLE_RATE_HZ / 50 - 1);			// compass sample rate
	}
}

void hmc_self_test(int16_t* mag_self_test)
{
	uint8_t data[7];
	
	// we want to talk directly with the HMC
	mpu_set_bypass(1);

	// setup self test (write both config A and B registers)
	data[0] = HMC_CFGA_AVG2 | HMC_CFGA_DRT_15 | HMC_CFGA_MSR_MODE_POS;
	data[1] = HMC_GAIN_5;
	i2c_write(HMC5883L_ADDR, HMC_REG_CONFIG_A, 2, data);
	
	data[0] = HMC_MODE_SINGLE_MSRMT;
	i2c_write(HMC5883L_ADDR, HMC_REG_MODE, 1, data);

	// wait for the data
	while (DRDY)
		;
	
	// read the measurement
	i2c_read(HMC5883L_ADDR, HMC_REG_DATA_X_MSB, 7, data);

	// mind the order
	mag_self_test[0] = (data[4] << 8) | data[5];
	mag_self_test[1] = (data[0] << 8) | data[1];
	mag_self_test[2] = (data[2] << 8) | data[3];
	
	mag_self_test[2] = -mag_self_test[2];

	// back to normal operation (write both config A and B registers)
	data[0] = HMC_CFGA_AVG2 | HMC_CFGA_DRT_15 | HMC_CFGA_MSR_MODE_NORM;
	data[1] = HMC_GAIN_1;
	i2c_write(HMC5883L_ADDR, HMC_REG_CONFIG_A, 2, data);
	
	// bypass off
	mpu_set_bypass(0);
}

void mpu_shutdown(void)
{
	// reset, MPU is in sleep mode after reset
	mpu_write_byte(PWR_MGMT_1, BIT_DEVICE_RESET);

	// if we have a magnetometer, power it down
	if (mag_type == mt_ak)
		ak_write_byte(AKM_REG_CNTL, AKM_POWER_DOWN);
}

void mpu_init(void)
{
	mpu_write_byte(PWR_MGMT_1, BIT_DEVICE_RESET);	// reset, MPU is in sleep mode after reset
	delay_ms(100);
	mpu_write_byte(PWR_MGMT_1, 0x00);				// wakeup
	//mpu_write_byte(PWR_MGMT_2, 0x00);				// standby off on all sensors (default value already 0)

	mpu_write_byte(GYRO_CONFIG, INV_FSR_2000DPS << 3);		// == mpu_set_gyro_fsr(2000)
	mpu_write_byte(ACCEL_CONFIG, INV_FSR_2G << 3);			// == mpu_set_accel_fsr(2)
	mpu_write_byte(SMPLRT_DIV, 1000 / SAMPLE_RATE_HZ - 1);	// == mpu_set_sample_rate(SAMPLE_RATE_HZ)
	mpu_write_byte(CONFIG, INV_FILTER_98HZ);				// == mpu_set_lpf(98)

	mpu_write_byte(INT_PIN_CFG, 0x80);				// pin interrupt is active low

	mpu_setup_compass();

	if (!dmp_load_firmware())
	{
		dputs("dmp_load_firmware FAILED!!!");
		return;
	}

	if (!dmp_set_orientation())
	{
		dputs("dmp_set_orientation FAILED!!!");
		return;
	}

	dmp_enable_feature();

	// quat, gyro and accel are copied into the FIFO by the DMP,
	// so we don't need to enable other stuff into the FIFO
	mpu_write_byte(FIFO_EN, 0);
	mpu_write_byte(USER_CTRL, 0x00);
	mpu_write_byte(USER_CTRL, BIT_DMP_RESET | BIT_FIFO_RESET);
	delay_ms(50);
	mpu_write_byte(USER_CTRL, BIT_I2C_MST_EN | BIT_DMP_EN | BIT_FIFO_EN);

	// we want to know when the DMP has something for us
	mpu_write_byte(INT_ENABLE, BIT_DMP_INT_EN);

	mpu_load_biases();
}

void mpu_calibrate_bias(void)
{
	uint8_t scnt;
	int8_t accel_step = 10;
	mpu_readings_t rd;
	tracker_settings_t new_settings;
	uint8_t old_sleep_mode;

	LED_RED = 0;
	LED_GREEN = 0;
	LED_BLUE = 1;

	dputs("**************** calibrating");

	mpu_init();

	// init the new settings struct
	old_sleep_mode = get_tracker_settings()->sleep_mode;
	memset(&new_settings, 0, sizeof(new_settings));

	// read the current accel bias
	mpu_read_accel_bias(new_settings.accel_bias);

	// set default gyro bias
	mpu_set_gyro_bias(new_settings.gyro_bias);

	//dprintf("old\ngyro %d %d %d\naccel %d %d %d\n",
	//				new_settings.gyro_bias[0], new_settings.gyro_bias[1], new_settings.gyro_bias[2],
	//				new_settings.accel_bias[0], new_settings.accel_bias[1], new_settings.accel_bias[2]);

	for (scnt = 0; scnt < 100; scnt++)
	{
		while (MPU_IRQ == 1)
			dbgPoll();
		while (MPU_IRQ == 0)
			;

		if (scnt == 40)
			accel_step = 2;

		dmp_read_fifo(&rd);

		//if (dbgEmpty())
		//	dprintf("g %6d %6d %6d  a %6d %6d %6d\n",
		//				rd.gyro[0], rd.gyro[1], rd.gyro[2],
		//				rd.accel[0], rd.accel[1], rd.accel[2]);

		// accel
		if (rd.accel[0] >= 1)
			new_settings.accel_bias[0] -= accel_step;
		else if (rd.accel[0] <= -1)
			new_settings.accel_bias[0] += accel_step;

		if (rd.accel[1] >= 1)
			new_settings.accel_bias[1] -= accel_step;
		else if (rd.accel[1] <= -1)
			new_settings.accel_bias[1] += accel_step;

		if (rd.accel[2] > 0x4000)
			new_settings.accel_bias[2] -= accel_step;
		else if (rd.accel[2] < 0x4000)
			new_settings.accel_bias[2] += accel_step;

		// gyro
		if (rd.gyro[0] > 1)
			new_settings.gyro_bias[0]--;
		else if (rd.gyro[0] < -1)
			new_settings.gyro_bias[0]++;

		if (rd.gyro[1] > 1)
			new_settings.gyro_bias[1]--;
		else if (rd.gyro[1] < -1)
			new_settings.gyro_bias[1]++;

		if (rd.gyro[2] > 1)
			new_settings.gyro_bias[2]--;
		else if (rd.gyro[2] < -1)
			new_settings.gyro_bias[2]++;

		// push the biases back to the MPU
		mpu_set_gyro_bias(new_settings.gyro_bias);
		mpu_set_accel_bias(new_settings.accel_bias);
	}

	// now save our settings
	new_settings.is_calibrated = 1;
	new_settings.sleep_mode = old_sleep_mode;
	save_tracker_settings(&new_settings);

	//dprintf("new\ngyro %6d %6d %6d\naccel %6d %6d %6d\n",
	//				new_settings.gyro_bias[0], new_settings.gyro_bias[1], new_settings.gyro_bias[2],
	//				new_settings.accel_bias[0], new_settings.accel_bias[1], new_settings.accel_bias[2]);

	dbgFlush();

	LED_BLUE = 0;
}
