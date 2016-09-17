#pragma once

#define CORDIC_TABLE_SIZE 	14

// units in which asin and atan return their angles
#define ANGLE_PI			0x7fff

#define CORDIC_GAIN			0.6072529365170104
#define SINCOS_RANGE		ANGLE_PI
#define SINCOS_GAIN			((int32_t)(SINCOS_RANGE * CORDIC_GAIN))

// sincos returns both the sine and cosine of an angle
void isincos_cord(int32_t angle, int16_t* rcos, int16_t* rsin);

// returns angle in the range -ANGLE_PI -> ANGLE_PI
int16_t iatan2_cord(int32_t y, int32_t x);

// asin parameter range: -ASIN_GAIN -> ASIN_GAIN
#define ASIN_GAIN			0x10000000

int16_t iasin_cord(int32_t x);
