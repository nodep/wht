#ifndef TRACKER_H
#define TRACKER_H

// LEDs on the head tracker PCB
#define LED_RED		P00
#define LED_BLUE	P01		// yellow LED on the first revision
#define LED_GREEN	P02

// the MPU IRQ line and the recenter button
#define MPU_IRQ			P06
#define RECENTER_BTN	P05

// the HMC5883L DRDY line
#define DRDY		P14

// the ISP programmer lines (used for debugging)
#if DBG_MODE
  #define FMISO		P10
  #define FCSN		P11
  #define FMOSI		P07
#endif

enum e_board_version
{
	bv_cr123a,	// the board powered by a CR123A
	
	bv_nimh,	// the board powered by a single NiMH with an MCP1640 booster
				// this version has the pin P1.6 pulled to the ground
				// it comes in two varieties: AA and AAA
};

extern uint8_t board_version;

#endif	// TRACKER_H
