#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include <compiler_mcs51.h>

#include "reports.h"
#include "dongle_settings.h"
#include "reg24lu1.h"
#include "nrfdbg.h"

// 512 bytes each
#define DATA0_ADDR		((FeatRep_DongleSettings __xdata *) 0x7c00)
#define DATA0_PAGE_NUM	62
#define DATA1_ADDR		((FeatRep_DongleSettings __xdata *) 0x7e00)
#define DATA1_PAGE_NUM	63

#define DATA_PAGE_SIZE		0x200
// how many settings blocks can we store in the two pages
#define BLOCKS_CAPACITY		((DATA_PAGE_SIZE*2) / sizeof(FeatRep_DongleSettings))

__xdata FeatRep_DongleSettings default_settings =
{
	0,		// report_id		is_empty

	1,		// active_devices	1 == joystick, 2 == mouse, 3 == both
	1,		// tracker_orient	0 == on/off switch is to the right
			//					1 == on/off switch is facing forward
	0,		// yc_method
	0,		// is_linear

	{ 12, 12, 12},	// joystick axis factors
	
	{NUM_MOUSE_SENS_VALUES / 2,  NUM_MOUSE_SENS_VALUES / 2,  0},		// mouse sensitivity
	
	0,				// x_drift_comp

	{0, 0, 0},		// mag offset
	{
		// mag calibration identity matrix
		{(1<<MAG_MATRIX_SCALE_BITS),                          0,                          0},
		{                         0, (1<<MAG_MATRIX_SCALE_BITS),                          0},
		{                         0,                          0, (1<<MAG_MATRIX_SCALE_BITS)},
	},

	/*
	// my tracker
	{503,-395,20},		// mag_offset
	{	// matrix
		{ 5678, -194,   -7},
		{ -194, 5696, -126},
		{   -7, -126, 6376},
	},
	*/
	
	NUM_MAG_SENS_VALUES / 2,		// mag sensitivity
};

void flash_page_erase(uint8_t pn)
{
    CKCON = 0x01;   // See nRF24LU1p AX PAN

    FCR = 0xAA;		// Enable flash write operation
    FCR = 0x55;
    WEN = 1;

    FCR = pn;			// Write the page address to FCR to start the page erase operation

    while (RDYN == 1)	// Wait for the erase operation to finish
        ;

    WEN = 0;
    CKCON = 0x02;
}

void flash_bytes_write(uint16_t a, uint8_t __xdata * p, uint16_t n)
{
    uint8_t __xdata * __data pb;

    CKCON = 0x01;

    FCR = 0xAA;		// enable flash write operation
    FCR = 0x55;
    WEN = 1;

    pb = (uint8_t __xdata *) a;	// write the bytes directly to the flash
    while (n--)
    {
        *pb++ = *p++;

        // wait for the write operation to finish
        while (RDYN == 1)
            ;
    }

    WEN = 0;
    CKCON = 0x02;
}

int8_t get_ndx_of_current_settings(void)
{
	const FeatRep_DongleSettings __xdata * pStart = DATA0_ADDR;
	uint8_t cnt;

	// if no settings have been saved yet
	if (pStart->report_id == 0xff)
		return -1;

	// find the last valid settings block (with valid == 0x00) in the page
	for (cnt = 0; cnt < BLOCKS_CAPACITY - 1  &&  pStart[cnt + 1].report_id != 0xff; ++cnt)
		;

	return cnt;
}

const FeatRep_DongleSettings __xdata * get_dongle_settings(void)
{
	const FeatRep_DongleSettings __xdata * pStart = DATA0_ADDR;
	int8_t ndx;

	ndx = get_ndx_of_current_settings();

	// no settings yet, so make a default one
	if (ndx == -1)
	{
		save_dongle_settings(&default_settings);
		ndx = 0;
	}

	return pStart + ndx;
}

void save_dongle_settings(const FeatRep_DongleSettings __xdata * pNewSettings)
{
	// get the next empty slot
	FeatRep_DongleSettings save_img;
	int8_t new_ndx = get_ndx_of_current_settings() + 1;

	// if all slots are full
	if (new_ndx == BLOCKS_CAPACITY)
	{
		// erase the two pages and start from the beginning
		flash_page_erase(DATA0_PAGE_NUM);
		flash_page_erase(DATA1_PAGE_NUM);

		new_ndx = 0;
	}

	memcpy(&save_img, pNewSettings, sizeof(save_img));

	save_img.report_id = 0x00;		// this is not really the report ID, it's just a flag that
									// shows if the block is used or not

	// save the new settings
	flash_bytes_write((uint16_t)(DATA0_ADDR + new_ndx), (uint8_t __xdata *) &save_img, sizeof(save_img));
}
