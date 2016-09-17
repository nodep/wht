#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "reg24lu1.h"

#include "reports.h"

#include "../tracker/rf_protocol.h"
#include "../tracker/tracker_settings.h"
#include "rf_dngl.h"
#include "nrfutils.h"

#include "usb.h"
#include "usb_regs.h"
#include "hw_defs.h"

#include "nrfdbg.h"
#include "proc_packet.h"
#include "dongle_settings.h"

#define NUM_COUNTER_PACKETS	10
uint8_t total_packets[NUM_COUNTER_PACKETS];		// holds the total count of the last second of packets

uint16_t ovf_counter, last_toggle;			// counter of T2 overflows: 98.3ms periods

uint16_t battery_voltage, temperature, shutdown_counter;
bool joystick_packet_ready = false;

// if the device is enabled to send joystick and mouse movement changes
bool is_enabled = true;

uint8_t tracker_ver = 0;

// Handles USB set reports. These are requests from the PC
// to the dongle to save settings or execute commands.
void on_set_report(void)
{
	if (out0buf[0] == DONGLE_SETTINGS_REPORT_ID)
	{
		// save the dongle settings to flash
		const FeatRep_DongleSettings __xdata * pSettings = get_dongle_settings();

		FeatRep_DongleSettings new_settings;
		memcpy(&new_settings, out0buf, sizeof(FeatRep_DongleSettings));
		new_settings.drift_per_1k = pSettings->drift_per_1k;

		// save the data structure we've just received
		save_dongle_settings(&new_settings);

	} else if (out0buf[0] == COMMAND_REPORT_ID) {

		uint8_t command = out0buf[1];

		if (command == CMD_CALIBRATE  ||
			(command >= CMD_DISABLE_SLEEP  &&  command <= CMD_SLEEP_LONGEST))
		{
			// tell the head tracker to execute the command
			rf_dngl_queue_ack_payload(&command, 1);

		} else if (command == CMD_RECENTER) {

			uint8_t c;
			uint16_t total = 0;

			recenter();

			// if asked to recenter, but there are no packets from the tracker
			// reset the report
			for (c = 0; c < NUM_COUNTER_PACKETS; ++c)
				total += total_packets[c];

			if (total == 0)
			{
				memset(&usb_joystick_report, 0, sizeof(usb_joystick_report));
				joystick_packet_ready = true;
			}

		} else if (command == CMD_SAVE_DRIFT) {
			save_x_drift_comp();
		} else if (command == CMD_DISABLE_TOGGLE) {

			if (ovf_counter - last_toggle > 5)
			{
				is_enabled = !is_enabled;
				last_toggle = ovf_counter;
			}
		}
	}
}

// these are request for data from the PC

void on_get_report(void)
{
	// This requests the HID report we defined with the HID report descriptor.
	// This is usually requested over EP1 IN, but can be requested over EP0 too.
	if (usbReqHidGetSetReport.reportID == JOYSTICK_REPORT_ID)
	{
		memcpy(in0buf, &usb_joystick_report, sizeof(usb_joystick_report));

		// send the data on it's way
		in0bc = sizeof(usb_joystick_report);

	} else if (usbReqHidGetSetReport.reportID == DONGLE_SETTINGS_REPORT_ID) {

		// copy the data into the buffer
		memcpy(in0buf, get_dongle_settings(), sizeof(FeatRep_DongleSettings));
		in0buf[0] = DONGLE_SETTINGS_REPORT_ID;

		// send the data on it's way
		in0bc = sizeof(FeatRep_DongleSettings);

	} else if (usbReqHidGetSetReport.reportID == TRACKER_SETTINGS_REPORT_ID) {

		// tell the head tracker to send us it's settings
		uint8_t ack_payload = CMD_READ_TRACKER_SETTINGS;
		uint8_t pckt_cnt, bytes_rcvd;
		tracker_settings_t tracker_settings;
		FeatRep_TrackerSettings __xdata * pReport = (FeatRep_TrackerSettings __xdata *) in0buf;

		rf_dngl_queue_ack_payload(&ack_payload, 1);

		pReport->report_id = CMD_READ_TRACKER_SETTINGS;
		pReport->has_tracker_responded = 0;

		// now wait for that data
		for (pckt_cnt = 0; pckt_cnt < 5; ++pckt_cnt)
		{
			bytes_rcvd = rf_dngl_recv(&tracker_settings, sizeof(tracker_settings));
			if (bytes_rcvd == sizeof(tracker_settings))
			{
				pReport->has_tracker_responded = 1;
				pReport->is_calibrated = tracker_settings.is_calibrated;
				pReport->gyro_bias[0] = tracker_settings.gyro_bias[0];
				pReport->gyro_bias[1] = tracker_settings.gyro_bias[1];
				pReport->gyro_bias[2] = tracker_settings.gyro_bias[2];
				pReport->accel_bias[0] = tracker_settings.accel_bias[0];
				pReport->accel_bias[1] = tracker_settings.accel_bias[1];
				pReport->accel_bias[2] = tracker_settings.accel_bias[2];
				pReport->sleep_mode = tracker_settings.sleep_mode;

				break;
			}

			delay_ms(20);
		}

		// send the data
		in0bc = sizeof(FeatRep_TrackerSettings);

	} else if (usbReqHidGetSetReport.reportID == STATUS_REPORT_ID) {

		FeatRep_Status __xdata * pResult = (FeatRep_Status __xdata *) in0buf;
		uint8_t c;
		uint16_t total = 0;

		pResult->report_id = STATUS_REPORT_ID;

		for (c = 0; c < NUM_COUNTER_PACKETS; ++c)
			total += total_packets[c];

		pResult->num_packets = total;
		pResult->sample_cnt = sample_cnt > 0 ? sample_cnt : 0;
		pResult->yaw_value = yaw_value;

		pResult->battery_voltage = battery_voltage;
		pResult->temperature = temperature;
		pResult->shutdown_counter = shutdown_counter;
		pResult->is_enabled = is_enabled ? 1 : 0;

		// send the data
		in0bc = sizeof(FeatRep_Status);

	} else if (usbReqHidGetSetReport.reportID == RAW_MAG_REPORT_ID) {

		// copy the raw data to the USB buffer
		raw_mag_samples.report_id = RAW_MAG_REPORT_ID;
		memcpy(in0buf, &raw_mag_samples, sizeof(raw_mag_samples));

		// send the data on it's way
		in0bc = sizeof(raw_mag_samples);

		raw_mag_samples.samples_desc = 0;

	} else if (usbReqHidGetSetReport.reportID == PROT_VER_REPORT_ID) {

		// write the values directly into the buffer
		in0buf[0] = PROT_VER_REPORT_ID;
		in0buf[1] = CURR_PROTOCOL_VER;
		in0buf[2] = tracker_ver;

		// send the data on it's way
		in0bc = sizeof(FeatRep_ProtocolVersion);
	}
}

void main(void)
{
	uint8_t raw_packet[32], bytes_read;
	uint8_t last_timer_capture;
	uint8_t total_packets_ndx;
	uint8_t curr_packets;
	bool mouse_packet_ready = false;

	P0DIR = 0x00;	// all outputs
	P0ALT = 0x00;	// all GPIO default behavior
	P0 = 0;			// all low

	// timer init
	T2CON =	0b10000001;		// start 1/24 timer
	CCEN =	0b11000000;		// capture on write to CCL3
	last_timer_capture = 0;
	ovf_counter = 0;

	memset(total_packets, 0, sizeof(total_packets));
	curr_packets = total_packets_ndx = 0;

	raw_mag_samples.samples_desc = 0;

	LED_off();

	dbgInit();
	dputs("\nI live...");

	usbInit();

	rf_dngl_init();

	reset_main_reports();

	for (;;)
	{
		usbPoll();	// handles USB events
		dbgPoll();	// send chars from the UART TX buffer

		// enabled/disabled indicator on the nRFburn
#if DBG_MODE != 0
		if (is_enabled)
			LED_DBG2 = 1;
		else
			LED_DBG2 = 0;
#endif

		// check the timer
		CCL3 = 1;	// capture CCH3 and check for overflow
		if (last_timer_capture > CCH3)
		{
			total_packets[total_packets_ndx] = curr_packets;

			ovf_counter++;
			total_packets_ndx++;
			if (total_packets_ndx == NUM_COUNTER_PACKETS)
				total_packets_ndx = 0;

			curr_packets = 0;
		}
		last_timer_capture = CCH3;

		// try to read the recv buffer, then process the received data
		bytes_read = rf_dngl_recv(raw_packet, sizeof raw_packet);
		if (bytes_read  &&  raw_packet[0] == CURR_PROTOCOL_VER)
		{
			LED_on();
			
			// positional packet?
			if (bytes_read == sizeof(tracker_position_packet_t))
			{
				joystick_packet_ready |= process_packet((tracker_position_packet_t*) raw_packet);
				mouse_packet_ready = true;

				curr_packets++;
			}

			// aux packet?
			if (bytes_read == sizeof(tracker_aux_packet_t))
			{
				tracker_aux_packet_t* aux_packet = (tracker_aux_packet_t*) raw_packet;

				temperature = aux_packet->temperature;
				battery_voltage = aux_packet->voltage;
				shutdown_counter = aux_packet->shutdown_counter;
			}

		} else {
			LED_off();
		}

		// is the mouse active?
		if ((get_dongle_settings()->active_devices & 2)  &&  is_enabled)
		{
			// send the report if the endpoint is not busy
			if (mouse_packet_ready  &&  (in2cs & 0x02) == 0)
			{
				// copy the mouse report into the endpoint buffer
				memcpy(in2buf, &usb_mouse_report, sizeof(usb_mouse_report));

				// send the data on it's way
				in2bc = sizeof(usb_mouse_report);

				// remember the new mouse reference
				reset_mouse_delta();

				mouse_packet_ready = false;
			}
		} else if (mouse_packet_ready) {

			reset_mouse_delta();

			mouse_packet_ready = false;
		}

		// is the joystick active?
		if ((get_dongle_settings()->active_devices & 1)  &&  is_enabled)
		{
			// check if the endpoint is not busy
			if (joystick_packet_ready  &&  (in1cs & 0x02) == 0)
			{
				// copy the joystick report into the endpoint buffer
				memcpy(in1buf, &usb_joystick_report, sizeof(usb_joystick_report));

				// send the data on it's way
				in1bc = sizeof(usb_joystick_report);

				joystick_packet_ready = false;
			}
		}
	}
}
