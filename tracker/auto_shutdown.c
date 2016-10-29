#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include <reg24le1.h>
#include <nrfdbg.h>
#include <nrfutils.h>

#include "tracker.h"
#include "sensors.h"
#include "tracker_settings.h"
#include "../dongle/reports.h"

int16_t pg[3];
int16_t stat_cnt;
uint8_t tick_cnt;

void init_auto_shutdown(void)
{
	stat_cnt = -1;
	tick_cnt = 0;
}

void shutdown(void)
{
	dbgFlush();
	
	mpu_shutdown();
	
	// blink all the LEDs
	LED_RED		= 1;
	LED_BLUE	= 1;
	LED_GREEN	= 1;
	delay_ms(250);
	LED_RED		= 0;
	LED_BLUE	= 0;
	LED_GREEN	= 0;
	
	CLKCTRL = 0x80;			// keep XOSC16M running in register retention
	CLKLFCTRL = 0x02;		// RTC2 synthesized from XOSC16M

	RTC2CMP = 0x1000;		// set the capture value - sleeps for about 0.12s
	
	RTC2CON = 0b00000111;	// compareMode => IRQ resets the timer
							// enable RTC2

	// wait for the 32kHz to startup (change phase)
	while (CLKLFCTRL&0x80);
	while ((CLKLFCTRL&0x80)!=0x80);

	IEN1 |= 0x20;			// enable interrupt
	
	do {
		PWRDWN = 0x04;		// go to register retention mode
		
		// we're sleeping here and waiting for the tick to wake us up
		// ...
		
		// we've awoken and waiting for the clocks to stabilize
		PWRDWN = 0x07;		// standby mode (wait for interrupt)
		PWRDWN = 0x00;		// clear PWRDWN
		
	} while (P05 == 1);		// while button not pressed

	// unleash the watchdogs to reset the MCU
	WDSV = 0x10;	// LSByte
	WDSV = 0x00;	// MSByte
	
	// this will reset the MCU along with all the peripherals
	while (1)
		;
}

#define SLEEP_ENTRIES		(CMD_SLEEP_LONGEST - CMD_SLEEP_SHORTEST + 1)
#define GYRO_THRESHOLD		20

__xdata int16_t sleep_timings[SLEEP_ENTRIES] =
{
	10,			// CMD_SLEEP_AFTER_10_SECONDS
	30,			// CMD_SLEEP_AFTER_30_SECONDS
	60,			// CMD_SLEEP_AFTER_1MIN
	2*60,		// CMD_SLEEP_AFTER_2MIN
	5*60,		// CMD_SLEEP_AFTER_5MIN
	10*60,		// CMD_SLEEP_AFTER_10MIN
	30*60,		// CMD_SLEEP_AFTER_30MIN
	60*60,		// CMD_SLEEP_AFTER_60MIN
};

int16_t check_auto_shutdown(const int16_t gyro[3])
{
	const tracker_settings_t __xdata * pSettings = get_tracker_settings();
	
	uint8_t time_ndx;
	
	if (pSettings->sleep_mode == CMD_DISABLE_SLEEP)
		return -1;
	
	time_ndx = pSettings->sleep_mode - CMD_SLEEP_SHORTEST;
	
	// not the first call?
	if (stat_cnt != -1)
	{
		// movement above threshold?
		if (pg[0] - gyro[0] > GYRO_THRESHOLD
				|| pg[1] - gyro[1] > GYRO_THRESHOLD
				|| pg[2] - gyro[2] > GYRO_THRESHOLD)
		{
			stat_cnt = sleep_timings[time_ndx];
			tick_cnt = 0;
		} else {
			if (++tick_cnt == 50)
			{
				stat_cnt --;
				tick_cnt = 0;
			}
		}
	} else {
		stat_cnt = sleep_timings[time_ndx];
		tick_cnt = 0;
	}

	// remember the gyro state for the next iteration
	memcpy(pg, gyro, sizeof(pg));

	if (stat_cnt == 0)
	{
		//dputs("nighty night");
		
		// the MCU will not return from this call, it will be reset
		// when the recenter button is pressed
		shutdown();
	}
	
	return stat_cnt;
}

void ISR_TICK() __interrupt INTERRUPT_TICK
{}
