#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "nrfdbg.h"

#include "reg24lu1.h"
#include "hw_defs.h"

#include "rf_protocol.h"
#include "reports.h"
#include "proc_packet.h"
#include "math_cordic.h"
#include "mdu.h"
#include "reports.h"
#include "dongle_settings.h"

bool should_recenter = true;
int32_t sample_cnt;
int16_t yaw_value;

FeatRep_RawMagSamples raw_mag_samples;

// we store this here to avoid passing it as an argument to the functions all the time
const __xdata FeatRep_DongleSettings* pSettings;

// These are indexes of the euler[] and center[] arrays.
// The roll and pitch are swapped compared to the MPU's orientation.
#define YAW		0
#define ROLL	1
#define PITCH	2

// stuff used by the functions below

int16_t euler[3];					// the resulting angles
__xdata tracker_position_packet_t* pckt;	// the packet we've received

// uses the current yaw value to calculate a drift compensation
void save_x_drift_comp(void)
{
	int32_t new_drift_per_1k;

	// get the current settings
	FeatRep_DongleSettings __xdata new_settings;
	memcpy(&new_settings, get_dongle_settings(), sizeof(FeatRep_DongleSettings));

	new_drift_per_1k = mul_16x16(yaw_value, 1024);
	new_drift_per_1k /= sample_cnt;

	new_settings.drift_per_1k += (int16_t) new_drift_per_1k;

	save_dongle_settings(&new_settings);

	recenter();
}

void recenter(void)
{
	should_recenter = true;
}

// convert the raw quaternions from the sensors into Euler angles
void quat2euler(__xdata int16_t* quat)
{
	int16_t qw, qx, qy, qz;
	int32_t qww, qxx, qyy, qzz;

	// calculate Yaw/Pitch/Roll

	// the CORDIC trig functions return angles in units already adjusted to the 16
	// bit signed integer range, so there's no need to scale the results

	qw = quat[0];
	qx = quat[1];
	qy = quat[2];
	qz = quat[3];

	qww = mul_16x16(qw, qw);	// these are MDU optimized 16 bit integer multiplications
	qxx = mul_16x16(qx, qx);
	qyy = mul_16x16(qy, qy);
	qzz = mul_16x16(qz, qz);

	// x - yaw
	euler[YAW] = -iatan2_cord(2 * (mul_16x16(qx, qy) + mul_16x16(qw, qz)), qww + qxx - qyy - qzz);

	// y - roll
	euler[ROLL] = -iasin_cord(-2 * (mul_16x16(qx, qz) - mul_16x16(qw, qy)));

	// z - pitch
	euler[PITCH] =  iatan2_cord(2 * (mul_16x16(qy, qz) + mul_16x16(qw, qx)), qww - qxx - qyy + qzz);
}

#define SAMPLES_FOR_RECENTER	8

// calculates and applies recentering offsets
// returns false if we are in the process of calulating new offsets and the euler angles are not valid
bool do_center(void)
{
	static int16_t center[3];
	static bool is_center_valid = false;
	static int16_t wrap_bias[3];
	uint8_t i;

	// do we need to calculate a new center
	if (should_recenter)
	{
		sample_cnt = -SAMPLES_FOR_RECENTER;
		is_center_valid = false;
		should_recenter = false;

		// remember the current angles to make a wrap bias
		// this fixes the problem with centering close to the -32768/32767 wrapping point
		memcpy(wrap_bias, euler, sizeof(wrap_bias));

		// clear the sums
		memset(center, 0, sizeof(center));
	}

	++sample_cnt;

	// accumulate the samples
	if (!is_center_valid)
		for (i = 0; i < 3; ++i)
			center[i] += euler[i] - wrap_bias[i];

	// enough samples?
	if (sample_cnt == 0)
	{
		for (i = 0; i < 3; ++i)
			center[i] = center[i] / SAMPLES_FOR_RECENTER + wrap_bias[i];

		is_center_valid = true;
	}

	// correct the euler angles
	if (is_center_valid)
		for (i = 0; i < 3; ++i)
			euler[i] -= center[i];

	return is_center_valid;
}

// applies the yaw drift
void do_drift(void)
{
	static int16_t center = 0;
	int16_t compensate = (sample_cnt * pSettings->drift_per_1k) >> 10;

	euler[YAW] -= compensate;

	// Save the current yaw angle after drift compensation and before
	// auto-centering or magnetometer compensation.
	// yaw_value is used for drift compensation calculations
	if (should_recenter)
		center = euler[YAW];

	yaw_value = euler[YAW] - center;
}

#define magX	mag[0]
#define magY	mag[1]
#define magZ	mag[2]

// this is our tilt compensated magnetic heading calculation
// and it takes a whopping 3.5ms to run -- optimization required
int16_t calc_mag_heading(__xdata int16_t* mag)
{
	int32_t Xh, Yh;
	int16_t sinroll, cosroll;
	int16_t sinpitch, cospitch;

	int16_t sinpitch_sinroll, sinpitch_cosroll;

	isincos_cord(euler[ROLL], &cosroll, &sinroll);
	isincos_cord(euler[PITCH], &cospitch, &sinpitch);

	Yh = mul_16x16(magZ, sinroll) + mul_16x16(magX, cosroll);

	Xh = mul_16x16(sinpitch, sinroll);
	sinpitch_sinroll = Xh / 0x8000;		// 0.573ms

	Xh = mul_16x16(sinpitch, cosroll);
	sinpitch_cosroll = Xh / 0x8000;

	Xh = mul_16x16(magY,cospitch) + mul_16x16(magX, sinpitch_sinroll) - mul_16x16(magZ, sinpitch_cosroll);

	return iatan2_cord(Yh, Xh);
}

#define DELTA_LOG_SIZE	6
#define DELTA_THLD		2000

void do_mag(__xdata int16_t* mag)
{
	static int8_t delta_ndx = -1;
	static int16_t mag_correction = 0;
	static int16_t delta_log[DELTA_LOG_SIZE];
	static int16_t delta_sum = 0;
	//static int16_t consec_count = 0;

	int16_t mag_heading, mag_delta;
	int16_t mag_calibrated[3];
	int32_t tcalib;
	uint8_t i, j;

	// if the mag is not calibrated - don't do anything
	if (pSettings->mag_matrix[0][0] == (1 << MAG_MATRIX_SCALE_BITS))
		return;

	//
	// apply the mag calibration
	//

	// first the offset
	for (i = 0; i < 3; i++)
		mag[i] -= pSettings->mag_offset[i];

	// now the matrix transform
	for (i = 0; i < 3; i++)
	{
		tcalib = 0;
		for (j = 0; j < 3; j++)
			tcalib += mul_16x16(pSettings->mag_matrix[i][j], mag[j]);

		mag_calibrated[i] = (tcalib >> MAG_MATRIX_SCALE_BITS);
	}

	// apply the correction calculated from the previous iterations
	euler[YAW] += mag_correction;

	mag_heading = calc_mag_heading(mag_calibrated);
	mag_delta = mag_heading - euler[YAW];

	if (abs(mag_delta) > 2000)
	{
		mag_correction += mag_delta;
		//consec_count = 0;
		delta_ndx = -1;
	}

	// reset the delta log?
	if (delta_ndx == -1)
	{
		for (i = 0; i < DELTA_LOG_SIZE; i++)
			delta_log[i] = 0;

		delta_ndx = 0;
		delta_sum = 0;
	}

	// update the sum
	delta_sum -= delta_log[delta_ndx];
	delta_sum += mag_delta;
	delta_log[delta_ndx] = mag_delta;

	if (++delta_ndx == DELTA_LOG_SIZE)
		delta_ndx = 0;

	{
		int16_t filtered_delta = delta_sum / DELTA_LOG_SIZE;
		const int16_t abs_filtered_delta = abs(filtered_delta);
		
#define VALUES_IN_SET		4

		__code const int16_t sens_tbl[NUM_MAG_SENS_VALUES][VALUES_IN_SET] =
		{
			{1500, 800, 380, 0},
			{ 900, 450, 220, 0},
			{ 600, 300, 150, 0},
			{ 400, 200, 100, 0},
			{ 200, 100,  50, 0},
			{ 100,  50,  25, 0},
		};

		__code const uint8_t divider_tbl[VALUES_IN_SET] = {4, 8, 12, 150};
		__code const uint16_t* pSensSet = sens_tbl[pSettings->mag_sens >= NUM_MAG_SENS_VALUES ? NUM_MAG_SENS_VALUES - 1 : pSettings->mag_sens];

		uint8_t c;
		for (c = 0; c < VALUES_IN_SET; ++c)
		{
			if (abs_filtered_delta > pSensSet[c])
			{
				filtered_delta /= divider_tbl[c];
				break;
			}
		}
		
		mag_correction += filtered_delta;
	}
}

#define SAMPLES_FOR_AUTOCENTER_BITS		6

void do_auto_center(void)
{
	static int16_t samples_in_zone = 0;
	static int16_t last_yaw = 0;
	static int32_t sum_yaw = 0;
	static int16_t autocenter_correction = 0;

	const int16_t yaw_limit = 2 * 300;

	// have we had a reset?
	if (sample_cnt == 0)
		autocenter_correction = 0;

	// apply the current auto-centering
	euler[YAW] += autocenter_correction;

	if (abs(euler[YAW]) < yaw_limit					// if we're looking ahead, give or take
			&&  abs(euler[YAW] - last_yaw) < 300	// and not moving
			&&  abs(euler[PITCH]) < 1000)			// and pitch is levelish
	{
		// start counting
		++samples_in_zone;
		sum_yaw += euler[YAW];
	} else {
		// stop counting
		samples_in_zone = 0;
		sum_yaw = 0;
	}

	last_yaw = euler[YAW];

	// if we stayed looking ahead-ish long enough then adjust yaw offset
	if (samples_in_zone >= (1<<SAMPLES_FOR_AUTOCENTER_BITS))
	{
		// center
		autocenter_correction -= sum_yaw >> SAMPLES_FOR_AUTOCENTER_BITS;
		samples_in_zone = 0;
		sum_yaw = 0;
	}
}

// do the axis response
void do_joystick_response(void)
{
	uint8_t i;

	if (!pSettings->is_linear)
	{
		for (i = 0; i < 3; ++i)
		{
			// this is only temporary. it will be replaced with
			// multi-point custom axis responses

			int32_t new_val = mul_16x16(euler[i], abs(euler[i]));
			new_val >>= 13;		// / 8192
			if (new_val > 32767)
				euler[i] = 32767;
			else if (new_val < -32768)
				euler[i] = -32768;
			else
				euler[i] = (int16_t) new_val;
		}
	}

	// apply the axis factors
	for (i = 0; i < 3; ++i)
	{
		int32_t new_val = mul_16x16(euler[i], pSettings->joy_fact[i]);

		if (new_val < -32768)
			euler[i] = -32768;
		else if (new_val > 32767)
			euler[i] = 32767;
		else
			euler[i] = (int16_t) new_val;
	}
}

// in order to implement a mouse, we need to send the coordinate delta,
// and to do that we need the last sent packet (to USB) as a reference
// for the delta and the packet just received from the tracker

int16_t ref_euler[3], prev_euler[3];
int16_t modulo[3];
bool init_ref_mouse = true;

void do_mouse(void)
{
	__xdata int16_t* pDest;
	uint8_t c;
	
	if (init_ref_mouse)
	{
		for (c = 0; c < 3; c++)
		{
			ref_euler[c] = euler[c];
			modulo[c] = 0;
		}
		
		init_ref_mouse = false;
	}
	
	// calc the deltas
	pDest = &usb_mouse_report.x;
	for (c = 0; c < 3; c++)
	{
		pDest[c] = euler[c] - ref_euler[c] + modulo[c];
		
		prev_euler[c] = euler[c];
	}

	// do the orientation swap
	if (pSettings->tracker_orient == 1)
	{
		// swap y and wheel
		int16_t t = usb_mouse_report.y;
		usb_mouse_report.y = usb_mouse_report.wheel;
		usb_mouse_report.wheel = t;
	}

	// do the sensitivity
	for (c = 0; c < 3; c++)
	{
		if (pSettings->mouse_sens[c] == 0)
			pDest[c] = 0;
		else
			pDest[c] /= (0x80 >> (pSettings->mouse_sens[c] - 1));
	}
}

// makes a new mouse reference
// we call this after a packet has been sent over USB
void reset_mouse_delta(void)
{
	int8_t c;
	for (c = 0; c < 3; c++)
	{
		// remember the modulo for the next packet
		
		// we want to skip the sens 0 (disabled)
		if (pSettings->mouse_sens[c] == 0)
		{
			modulo[c] = 0;
		} else {
			uint8_t divisor = (0x80 >> (pSettings->mouse_sens[c] - 1));
			modulo[c] = (prev_euler[c] - ref_euler[c]) % divisor;
		}
		
		ref_euler[c] = prev_euler[c];
	}
}

// called for every received position packet
bool process_packet(__xdata tracker_position_packet_t* pcktp)
{
	pckt = pcktp;

	// recenter if the button on the tracker was pressed
	if (pckt->flags & FLAG_RECENTER)
		recenter();

	// we're getting the settings pointer here, and use it in some of the functions above
	pSettings = get_dongle_settings();

	// convert quaternions to euler angles
	quat2euler(pckt->quat);

	// apply the yaw drift compensation
	do_drift();

	// handle the magnetometer data
	if (pckt->flags & FLAG_MAG_VALID)
	{
		// collect the raw mag samples
		if ((raw_mag_samples.samples_desc & 0x7f) < MAX_RAW_MAG_SAMPLES)
		{
			__xdata int16_t* pMag = &(raw_mag_samples.mag[raw_mag_samples.samples_desc & 0x7f].x);
			pMag[0] = pckt->mag[0];
			pMag[1] = pckt->mag[1];
			pMag[2] = pckt->mag[2];

			raw_mag_samples.samples_desc++;

			// select the magnetometer
			if (pckt->flags & FLAG_MAG_SENSOR)
				raw_mag_samples.samples_desc |= 0x80;
		}

		// magnetometer - do mag correction only if the mag data is present
		// and we don't already do some other autocentering
		if (pSettings->yc_method == ycm_auto  ||  pSettings->yc_method == ycm_mag_only)
			do_mag(pckt->mag);
	}

	// calc the mouse data
	do_mouse();

	// calc and/or apply the centering offset
	if (!do_center())
		return false;

	// do the auto-centering if required
	if (pSettings->yc_method == ycm_ac_only  ||  (pSettings->yc_method == ycm_auto  &&  (pckt->flags & FLAG_MAG_VALID) == 0))
		do_auto_center();

	// do the axis response transformations
	do_joystick_response();

	// copy the data into the USB report
	usb_joystick_report.x = euler[YAW];
	usb_joystick_report.y = euler[ROLL];
	usb_joystick_report.z = euler[PITCH];
	
	if (pSettings->tracker_orient == 1)
	{
		// swap pitch and roll
		int16_t t = usb_joystick_report.y;
		usb_joystick_report.y = usb_joystick_report.z;
		usb_joystick_report.z = t;
	}

	usb_joystick_report.buttons = pckt->kbd_buttons;

	return true;
}
