#ifndef __SDCC_REVISION
#include <intrins.h>
#endif

// This is a bit-banged implementation of a subset of the I2C protocol
// with an emphasis on the MPU's command syntax.
//
// After days of trying to get around the bugs in the hardware module of
// nRF24LE1's I2C (Two Wire Protocol) implementation, I've decided enough
// is enough, and wrote this library. I've had no problems with it since.
//
// I might make an assembler version of the library in the future. It would
// be a good exercise in assembly programming.

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include <compiler_mcs51.h>

#include <reg24le1.h>
#include <nrfdbg.h>
#include <nrfutils.h>

#include "i2c.h"

#define ADDR_READ(a)			(((a) << 1) | 1)
#define ADDR_WRITE(a)			((a) << 1)


#define I2C_SDA_PDIR	P1DIR
#define I2C_SCL_PDIR	P1DIR

#define I2C_SDA			P13
#define I2C_SCL			P12

#define I2C_SDA_BIT		3
#define I2C_SCL_BIT		2

#define i2c_sda_pull()		{ I2C_SDA_PDIR &= ~_BV(I2C_SDA_BIT); I2C_SDA = 0; }
/*void i2c_sda_pull(void)
{
	P0DIR &= ~_BV(5);	// output
	I2C_SDA = 0;		// drive low
}*/

#define i2c_sda_release()		I2C_SDA_PDIR |= _BV(I2C_SDA_BIT)
/*void i2c_sda_release(void)
{
	P0DIR |= _BV(I2C_SDA_BIT);	// input
}*/

#define i2c_scl_release()		I2C_SCL_PDIR |= _BV(I2C_SCL_BIT)
/*void i2c_scl_release(void)
{
	P0DIR |= _BV(I2C_SCL_BIT);	// input
}*/

#define i2c_scl_pull()			{ I2C_SCL_PDIR &= ~_BV(I2C_SCL_BIT); I2C_SCL = 0; }
/*void i2c_scl_pull(void)
{
	P0DIR &= ~_BV(I2C_SCL_BIT);	// output
	I2C_SCL = 0;				// drive low
}*/

void i2c_start(void)
{
	i2c_sda_release();
	i2c_scl_release();
	delay_us(2);
	i2c_sda_pull();
	delay_us(2);
	i2c_scl_pull();
	delay_us(1);
	i2c_sda_release();
	delay_us(2);
}

void i2c_stop(void)
{
	i2c_sda_pull();
	delay_us(2);
	i2c_scl_release();
	delay_us(2);
	i2c_sda_release();
	delay_us(4);
}

void i2c_write_bit(bool bit2write)
{
	if (bit2write)
		i2c_sda_release();	// SDA hi
	else
		i2c_sda_pull();		// SDA lo
		
	i2c_scl_release();		// SCL hi
#ifdef __SDCC_REVISION	
		__asm
		nop
		nop
		nop
		nop
		nop
		__endasm;
#else		
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
#endif
	i2c_scl_pull();			// SCL lo
}

bool i2c_read_bit(void)
{
	bool result;
	
	i2c_sda_release();		// SDA lo
	//__asm
	//nop
	//__endasm;
	i2c_scl_release();		// SCL hi
	result = I2C_SDA;
	i2c_scl_pull();			// SCL lo
	
	return result;
}

bool i2c_write_byte(uint8_t __data val)
{
	// send each bit
	i2c_write_bit(val & 0x80);
	i2c_write_bit(val & 0x40);
	i2c_write_bit(val & 0x20);
	i2c_write_bit(val & 0x10);
	i2c_write_bit(val & 0x08);
	i2c_write_bit(val & 0x04);
	i2c_write_bit(val & 0x02);
	i2c_write_bit(val & 0x01);
	
	return !i2c_read_bit();
}

uint8_t i2c_read_byte(bool should_ack)
{
	uint8_t result = 0;
	
	// read the bits
	if (i2c_read_bit())		result |= 0x80;
	if (i2c_read_bit())		result |= 0x40;
	if (i2c_read_bit())		result |= 0x20;
	if (i2c_read_bit())		result |= 0x10;
	if (i2c_read_bit())		result |= 0x08;
	if (i2c_read_bit())		result |= 0x04;
	if (i2c_read_bit())		result |= 0x02;
	if (i2c_read_bit())		result |= 0x01;
	
	i2c_write_bit(should_ack);
	
	return result;
}

void i2c_init(void)
{
	// inputs
#if I2C_SDA_PDIR == I2C_SCL_PDIR
	I2C_SDA_PDIR |= _BV(I2C_SDA_BIT) | _BV(I2C_SCL_BIT);
#else
	I2C_SDA_PDIR |= _BV(I2C_SDA_BIT);
	I2C_SCL_PDIR |= _BV(I2C_SCL_BIT);
#endif
	
	i2c_stop();
}

bool i2c_write(uint8_t dev_addr, uint8_t reg_addr, uint8_t data_len, const uint8_t* data_ptr)
{
	bool ret_val = false;
	
	i2c_start();
	
	// send the address and the register
	if (i2c_write_byte(ADDR_WRITE(dev_addr))  &&  i2c_write_byte(reg_addr))
	{
		if (data_len == 0)
			ret_val = true;

		// write the required number of bytes
		while (data_len--)
		{
			if (!i2c_write_byte(*data_ptr++))
				break;

			if (data_len == 0)
				ret_val = true;
		}
	}

	i2c_stop();
	
	return ret_val;
}

bool i2c_read(uint8_t dev_addr, uint8_t reg_addr, uint8_t data_len, uint8_t *data_ptr)
{
	bool ret_val = false;
	
	i2c_start();
	
	// setup a read
	if (i2c_write_byte(ADDR_WRITE(dev_addr))  &&  i2c_write_byte(reg_addr))
	{
		i2c_start();		// repeated start

		// now start reading
		if (i2c_write_byte(ADDR_READ(dev_addr)))
		{
			while (data_len--)
				*data_ptr++ = i2c_read_byte(data_len == 0);
				
			ret_val = true;
		}
	}

	i2c_stop();
		
	return ret_val;
}
