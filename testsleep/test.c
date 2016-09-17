#include <reg24le1.h>
#include <stdint.h>

/*
// An nRF24LE1 MEMORY RETENTION TIMER ON example application 
uint8_t toggle;

void mcu_init(void)
{
	// XOSC32K input
	P0DIR = 0x03;
	P1DIR = 0x00;
	P2DIR = 0x00;
	P3DIR = 0x00;

	P0 = 0x00;
	P1 = 0x00;
	P2 = 0x00;
	P3 = 0x00;

	// Open latch 
	OPMCON = 0x00; 
}

void wakeup_tick() __interrupt INTERRUPT_TICK
{
	// Toggle output on wakeup
	P02 = toggle;
	toggle = !toggle;
}

void main(void)
{
	mcu_init();

	// If wakeup from tick, turn on LED
	if (PWRDWN & 0x40)
	{
		IEN1 = 0x20;
		EA = 1; 
		// Go to standby mode and wait for RTC interrupt
		PWRDWN = 0x07;
		// Clear power down
		PWRDWN = 0x00;
	} else {
		// Init RTC timer
		
		//hal_rtc_start(false);
		RTC2CON &= ~(1<<0);

		//hal_clklf_set_source(HAL_CLKLF_XOSC32K);		HAL_CLKLF_XOSC32K == 0
		//CLKLFCTRL = (uint8_t)(CLKLFCTRL & (uint8_t)~0x07U) | (uint8_t)((uint8_t)source & (uint8_t)0x07U);
		CLKLFCTRL = 0;		// source XOSC32K

		//hal_rtc_set_compare_mode(HAL_RTC_COMPARE_MODE_0);		HAL_RTC_COMPARE_MODE_0 == 3
		//RTC2CON = (RTC2CON & 0xF9) | ((mode << 1) & ~0xF9);		// Set "compareMode" bits
		RTC2CON = 0b00000110;		// IRQ resets the timer

		//hal_rtc_set_compare_value(0x7FFF);
		{
		uint8_t compare_status;                   
		compare_status = (RTC2CON & (BIT_1 | BIT_2)); // Save "compareMode" bits
		RTC2CON &= ~(BIT_1 | BIT_2);              // Disable compare
		RTC2CMP0 = LSB(value);                    // Write LSB
		RTC2CMP1 = MSB(value);                    // Write MSB
		RTC2CON |= compare_status;                // Update compare status 
		}
		
		//hal_rtc_start(true); 
		RTC2CON |= 1;
	}

	// Lock latch
	OPMCON |= 0x02;

	// Memory retention mode timer on, system reset on wakeup
	PWRDWN = 0x03; 
	
	while(1)
		;
}
*/


// An nRF24LE1 REGISTER RETENTION TIMER ON example application 

#include <reg24le1.h>

void mcu_init(void)
{
	P0DIR = 0x20;		// P0.5 is the button
	P0CON = 0x55;		// turn on the pullup for the button
	P1DIR = 0x00;

	P0 = 0x00;
	P1 = 0x00;
}

/*
void wakeup_tick() __interrupt INTERRUPT_TICK
{
}
*/

#define LSB(a) (((a) & 0xFFU))
#define MSB(a) (((a) & 0xFF00U) >> 8)

void main(void)
{
	mcu_init();

	CLKCTRL = 0x80;			// keep XOSC16M running in register retention
	CLKLFCTRL = 0x02;		// RTC2 synthesized from XOSC16M

	RTC2CMP = 0x600;		// set the capture value 
	
	RTC2CON = 0b00000111;	// compareMode => IRQ resets the timer
							// enable RTC2

	// Wait for the 32kHz to startup (change phase)
	while((CLKLFCTRL&0x80)==0x80);
	while((CLKLFCTRL&0x80)!=0x80);

	IEN1 = 0x20;
	EA = 1; 

	while (1)
	{
		PWRDWN = 0x04;		// register retention mode
		PWRDWN = 0x07;		// standby mode (wait for interrupt)
		PWRDWN = 0x00;		// clear PWRDWN

		P02 = !P02;
		
		if (P05 == 0)
		{
			P01 = !P01;
		}
	}
}
