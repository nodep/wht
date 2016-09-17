#pragma once

// this is the LED on the generic dongle
#define LED_on()		P03 = 1
#define LED_off()		P03 = 0
#define LED_toggle()	P03 = !P03

// these pins are connected to the debug LEDs on the nRFburn programmer
#define LED_DBG1		P04
#define LED_DBG2		P05
