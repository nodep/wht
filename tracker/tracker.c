#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include <reg24le1.h>
#include <nrf24l.h>
#include <nrfutils.h>
#include <nrfdbg.h>

#include "i2c.h"
#include "rf_protocol.h"
#include "../dongle/reports.h"
#include "sensors.h"
#include "sleeping.h"
#include "rf_tracker.h"
#include "auto_shutdown.h"
#include "tracker_settings.h"
#include "tracker.h"

uint8_t board_version;

void hw_init()
{
	uint8_t c;

	// check which version of the board we are running on
	P1CON = 0b01010110;		// enable pullup on P1.6
	delay_ms(10);			// wait a little
	board_version = (P16 ? bv_cr123a : bv_nimh);	// read the pin
	P1CON = 0b01110110;		// disable pullup on P1.6

	P0DIR = 0xf0;		// P0.0 P0.1 P0.2 are the LEDs and they are outputs
                        // P0.3 is the UART TX - output
						// P0.5 is the push button - input
						// P0.6 is the MPU interrupt pin - input

	P0CON = 0b01010101;	// turn on the pullup for the recenter button

#if DBG_MODE > 0  &&  MOUSE_BUTTONS == 0
	// enable FMISO, FMOSI and FCSN as debug output pins
	P1DIR = 0xfc;
	P0DIR = 0x70;
#endif

	// cycle the LEDs
	LED_RED		= 0;
	LED_BLUE	= 0;
	LED_GREEN	= 0;

	for (c = 0; c < 3; ++c)
	{
		LED_RED		= 1;
		delay_ms(40);
		LED_RED		= 0;
		LED_BLUE	= 1;
		delay_ms(40);
		LED_BLUE	= 0;
		LED_GREEN	= 1;
		delay_ms(40);
		LED_GREEN	= 0;
		LED_BLUE	= 1;
		delay_ms(40);
		LED_BLUE	= 0;
	}

	dbgInit();

	dputs("init started");

	LED_BLUE = 1;

	i2c_init();
	mpu_init();

#if AUTO_SHUTDOWN
	init_auto_shutdown();
#endif

	dbgFlush();

	rf_tracker_init();	// init the radio

	init_sleep();		// we need to wake up from RFIRQ

	LED_BLUE = 0;

	dputs("init OK");
}

/*
// This is a test for a new kind of gyro/accel calibration function.
void test_bias(void)
{
	int32_t g[3];
	int32_t a[3];
	int16_t cnt = 0;
	uint8_t more;
	mpu_packet_t pckt;

	while (1)
	{
		if (cnt == 0)
		{
			g[0] = 0;
			g[1] = 0;
			g[2] = 0;

			a[0] = 0;
			a[1] = 0;
			a[2] = 0;
		}

		// wait for the interrupt
		while (MPU_IRQ)
			dbgPoll();
		while (!MPU_IRQ);

		dmp_read_fifo(&pckt, &more);

		a[0] += pckt.accel[0];
		a[1] += pckt.accel[1];
		a[2] += pckt.accel[2];

		g[0] += pckt.gyro[0];
		g[1] += pckt.gyro[1];
		g[2] += pckt.gyro[2];

		if (cnt == 200)
		{
			dprintf("%li   %li   %li\n", a[0] / cnt, a[1] / cnt, a[2] / cnt - 16384);
			cnt = 0;
		} else {
			++cnt;
		}
	}
}
*/

// returns the battery voltage in 10mV units
// for instance: get_battery_voltage() returning 278 equals a voltage of 2.78V
uint16_t get_battery_voltage(void)
{
	uint16_t res;

	// config the ADC

	ADCCON3 = 0b01100000;	// 8 bits, right justify

	ADCCON2 = 0b00000111;	// longest input acquisition window
							// 6us ADC power-down delay

	if (board_version == bv_cr123a)
	{
		ADCCON1 = 0b10111000;	// power up ADC
								// 1/3 VDD
								// internal 1.2V reference
								// starts the conversion
	} else {
		ADCCON1 = 0b10010001;	// power up ADC
								// AIN4
								// reference VDD (3.3V)
								// starts the conversion
	}

	// wait for the conversion to finish
	delay_us(3);	// wait for busy bit to stabilize
	while (ADCCON1 & 0x40)
		;

	res = ADCDATL;

#define MAX_VOLTAGE		327		// 3.27V
//#define MAX_VOLTAGE		298		// 2.98V the prototype

	if (board_version == bv_cr123a)
		res = (res * 72) / 51;
	else
		res = (res * MAX_VOLTAGE) / 255;

	return res;
}

// check for an ACK payload and reacts to it
uint8_t check_ack(void)
{
	uint8_t ack;

	if (rf_tracker_read_ack_payload(&ack, 1))
	{
		if (ack == CMD_CALIBRATE)
		{
			mpu_calibrate_bias();
			return 1;
		} else if (ack == CMD_READ_TRACKER_SETTINGS) {
			rf_tracker_send_message(get_tracker_settings(), sizeof(tracker_settings_t));
		} else if (ack >= CMD_DISABLE_SLEEP  &&  ack <= CMD_SLEEP_LONGEST) {
			const tracker_settings_t __xdata * pSettings = get_tracker_settings();
			if (pSettings->sleep_mode != ack)
			{
				// save the new settings
				tracker_settings_t new_settings;
				memcpy(&new_settings, pSettings, sizeof(new_settings));
				new_settings.sleep_mode = ack;
				save_tracker_settings(&new_settings);
				
				// reset the sleep state
				init_auto_shutdown();
			}
		}
	}

	return 0;
}

#if MOUSE_BUTTONS

uint16_t scan_mouse_buttons(void)
{
	const uint16_t stab_delay = 150;

	// P05 FSCK
	// P07 FMOSI
	// P10 FMISO
	// P11 FCSN

	uint16_t button_states = 0;

	//
	// P05 output - P07, P10, P11 inputs
	//

	P0DIR = 0b11010111;		// P05 is output (and P03 - TX)
	P1DIR = 0b11111111;		// all inputs

	// pullups
	P0CON = 0b01010111;		// P07
	P1CON = 0b01010000;		// P10
	P1CON = 0b01010001;		// P11

	P05 = 0;		// drive low

	delay_us(stab_delay);

	if (P07 == 0)
		button_states |= 0x00100;	// SW9
	if (P10 == 0)
		button_states |= 0x00040;	// SW7
	if (P11 == 0)
		button_states |= 0x00010;	// SW5

	// pulldowns
	P0CON = 0b00110111;		// P07
	P1CON = 0b00110000;		// P10
	P1CON = 0b00110001;		// P11

	P05 = 1;		// drive high

	delay_us(stab_delay);

	if (P07 == 1)
		button_states |= 0x00200;	// SW10
	if (P10 == 1)
		button_states |= 0x00080;	// SW8
	if (P11 == 1)
		button_states |= 0x00020;	// SW6

	P0CON = 0b01110111;		// P07 disable pull up/down
	
	//
	// P07 output - P10, P11 inputs
	//

	P0DIR = 0b01110111;		// P07 is output (and P03 - TX)
	P1DIR = 0b11111111;		// all inputs

	// pullups
	P1CON = 0b01010000;		// P10
	P1CON = 0b01010001;		// P11

	P07 = 0;		// drive low

	delay_us(stab_delay);

	if (P10 == 0)
		button_states |= 0x00800;	// SW12
	if (P11 == 0)
		button_states |= 0x00002;	// SW2

	// pulldowns
	P1CON = 0b00110000;		// P10
	P1CON = 0b00110001;		// P11

	P07 = 1;		// drive high

	delay_us(stab_delay);

	if (P10 == 1)
		button_states |= 0x00400;	// SW11
	if (P11 == 1)
		button_states |= 0x00001;	// SW1

	P1CON = 0b01110000;		// P10 disable pull up/downs
	
	//
	// P10 output - P11 input
	//

	P0DIR = 0b11110111;		// P03 TX is output
	P1DIR = 0b11111110;		// P10 output, P11 input

	// pullups
	P1CON = 0b01010001;		// P11

	P10 = 0;		// drive low

	delay_us(stab_delay);

	if (P11 == 0)
		button_states |= 0x00008;	// SW4

	// pulldowns
	P1CON = 0b00110001;		// P11

	P10 = 1;		// drive high

	delay_us(stab_delay);

	if (P11 == 1)
		button_states |= 0x00004;	// SW3

	P1CON = 0b00110001;		// P11 disable pull up/downs


	// leave everything the way it was
	P0DIR = 0xf0;		// P0.0 P0.1 P0.2 are the LEDs and they are outputs
                        // P0.3 is the UART TX - output
						// P0.5 is the push button - input
						// P0.6 is the MPU interrupt pin - input
						
	P1DIR = 0xff;		// all inputs

	return button_states;
}

#endif	// MOUSE_BUTTONS


#define LED_PCKT_TOTAL		150
#define LED_PCKT_LED_ON		2

// we send 1 aux packet for every AUX_SEND_EVERY positional packets
#define AUX_SEND_EVERY		50

int main(void)
{
	uint8_t rf_pckt_ok = 0, rf_pckt_lost = 0;
	uint8_t aux_counter = 0;
	uint8_t send_recenter_cnt = 0;

	bool read_result;
	tracker_position_packet_t pos_pckt;
	tracker_aux_packet_t aux_pckt;
	mpu_readings_t mpu_rd;

	pos_pckt.tracker_ver = CURR_PROTOCOL_VER;
	aux_pckt.tracker_ver = CURR_PROTOCOL_VER;
	
	pos_pckt.mouse_buttons = 0;

	hw_init();

	for (;;)
	{
		// Waits for the interrupt from the MPU.
		// Instead of polling, I should put the MCU to sleep and then have it awaken by the MPU.
		// However, I have not succeeded in making the wakeup work reliably.
		// I might try putting the MCU to sleep and then waking it up with a timer...
		while (MPU_IRQ)
			dbgPoll();
		while (!MPU_IRQ)
			;

		// read all the packets in the MPU fifo
		read_result = dmp_read_fifo(&mpu_rd);

		if (read_result)
		{
			aux_pckt.shutdown_counter = check_auto_shutdown(mpu_rd.gyro);

			// set the recenter flag
#if MOUSE_BUTTONS
			pos_pckt.flags = send_recenter_cnt;
			pos_pckt.mouse_buttons |= scan_mouse_buttons();
			
			{
				uint8_t c;
				uint16_t btn = pos_pckt.mouse_buttons;
				for (c = 0; c < 12; c++)
				{
					if (btn & 1)
						putchar('O');
					else
						putchar('.');
					btn >>= 1;
				}
				dputs("");
			}
#else
			pos_pckt.flags = (RECENTER_BTN == 0  ||  send_recenter_cnt != 0 ? FLAG_RECENTER : 0);
#endif

			// copy the quaternions
			memcpy(pos_pckt.quat, mpu_rd.quat, sizeof(pos_pckt.quat));

			// add the compass data if we have it
			mpu_read_compass(&mpu_rd);

			// set the magnetometer data
			if (mpu_rd.mag_sensor)
			{
				pos_pckt.flags |= FLAG_MAG_VALID;

				if (mpu_rd.mag_sensor == 2)		// is HMC?
					pos_pckt.flags |= FLAG_MAG_SENSOR;

				memcpy(pos_pckt.mag, mpu_rd.mag, sizeof(pos_pckt.mag));
			}

			// send the position packet
			if (rf_tracker_send_message(&pos_pckt, sizeof(pos_pckt)))
			{
				++rf_pckt_ok;

				send_recenter_cnt = check_ack();
				pos_pckt.mouse_buttons = 0;		// reset the mouse button states
			} else {
				++rf_pckt_lost;
			}

			// update the LEDs
			if (rf_pckt_lost + rf_pckt_ok == LED_PCKT_TOTAL)
			{
				if (rf_pckt_ok > rf_pckt_lost)
					LED_GREEN = 1;
				else
					LED_RED = 1;

			} else if (rf_pckt_lost + rf_pckt_ok == LED_PCKT_TOTAL + LED_PCKT_LED_ON) {
				LED_RED = 0;
				LED_GREEN = 0;

				rf_pckt_ok = rf_pckt_lost = 0;
			}

			// send the aux packet?
			if (++aux_counter >= AUX_SEND_EVERY)
			{
				aux_pckt.flags = 0;

				aux_pckt.voltage = get_battery_voltage();
				aux_pckt.temperature = mpu_read_temperature();

				// disable the self test stuff for now
				//if (mpu_rd.mag_sensor == 2)
				//{
				//	aux_pckt.flags |= FLAG_MAG_VALID | FLAG_MAG_SENSOR;
				//	hmc_self_test(aux_pckt.mag_self_text);
				//} else if (mpu_rd.mag_sensor == 1) {
				//	aux_pckt.flags |= FLAG_MAG_VALID;
				//	akm_self_test(aux_pckt.mag_self_text);
				//}

				// send the packet and check the ACK
				if (rf_tracker_send_message(&aux_pckt, sizeof(aux_pckt)))
				{
					send_recenter_cnt += check_ack();
					aux_counter = 0;
				}
			}
		}
	}
}
