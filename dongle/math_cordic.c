#include <stdint.h>

// VC2010 does not support stdbool.h in C
// we need this because of the cordic test project in ../cordic
#ifdef _MSC_VER
# define bool uint8_t
# define __code
#else
# include <stdbool.h>
#endif

#include "math_cordic.h"

// These are CORDIC implementations of the atan2 and asin functions.
// These take up about 2.5kb less flash than the standard C math library
// versions. The return units are not radians, they are scaled and use the entire
// 16 bit range of int16_t.

int16_t __code atanTable[CORDIC_TABLE_SIZE] =
{
8192,	// 0		== atan(1) * (ANGLE_PI / PI)
4836,   // 1		== atan(0.5) * (ANGLE_PI / PI)
2555,   // 2		== atan(0.25) * (ANGLE_PI / PI)
1297,   // 3		== atan(0.125) * (ANGLE_PI / PI)
651,    // 4		== ...
326,    // 5
163,    // 6
81,     // 7
41,     // 8
20,     // 9
10,     // 10
5,      // 11
3,      // 12
1,      // 13
};

void isincos_cord(int32_t angle, int16_t* rcos, int16_t* rsin)
{
	uint8_t i;
	int32_t xi, yi;
	int32_t rotation_angle = 0;
	int32_t x = SINCOS_GAIN;
	int32_t y = 0;

	bool is_gt_pi_2 = angle > ANGLE_PI/2;
	bool is_lt_mpi_2 = angle < -ANGLE_PI/2;

	// correct the angle range
	if (is_gt_pi_2)
		rotation_angle = ANGLE_PI - rotation_angle;
	else if (is_lt_mpi_2)
		rotation_angle = -ANGLE_PI + rotation_angle;

	for (i = 0; i < CORDIC_TABLE_SIZE; i++)
	{
		xi = x >> i;
		yi = y >> i;

		if (angle > rotation_angle)
		{
			x -= yi;
			y += xi;
			rotation_angle += atanTable[i];
		} else {
			x += yi;
			y -= xi;
			rotation_angle -= atanTable[i];
		}
	}

	if (is_gt_pi_2  ||  is_lt_mpi_2)
	{
		x = -x;
		y = -y;
	}

	// make sure there's no overflows
	if (x > SINCOS_RANGE)
		x = SINCOS_RANGE;
	else if (x < -SINCOS_RANGE)
		x = -SINCOS_RANGE;

	if (y > SINCOS_RANGE)
		y = SINCOS_RANGE;
	else if (y < -SINCOS_RANGE)
		y = -SINCOS_RANGE;

	*rcos = x;
	*rsin = y;
}


// We are doing the double iteration variant of the CORDIC algorithm
// This is needed to keep the accuracy good enough for our purposes,
// but comes at a price - it's about 3 times slower than the math
// library implementation.
int16_t iasin_cord(int32_t xf)
{
    uint8_t cnt;
    int16_t result = 0;
    int32_t x = ASIN_GAIN;
	int32_t y = 0;
    int32_t xi, yi;

    for (cnt = 0; cnt < CORDIC_TABLE_SIZE; cnt++)
    {
		xi = x >> cnt;
		yi = y >> cnt;

        if (y < xf)
		{
			x -= yi;
			y += xi;

			xi = x >> cnt;
			yi = y >> cnt;

			x -= yi;
			y += xi;

			result += 2 * atanTable[cnt];
		} else {
			x += yi;
			y -= xi;

			xi = x >> cnt;
			yi = y >> cnt;

			x += yi;
			y -= xi;

			result -= 2 * atanTable[cnt];
		}

		xf += xf >> (cnt << 1);
    }

    return result;
}

int16_t iatan2_cord(int32_t x, int32_t y)
{
	int16_t result;
	int32_t xi, yi;
	uint8_t cnt;

	if (y < 0)
	{
		result = 0x7fff;

		if (x == 0)
			return result;

		y = -y;
		x = -x;
	} else {
		result = 0;
	}

	for (cnt = 0; cnt < CORDIC_TABLE_SIZE; cnt++)
	{
		yi = y >> cnt;
		xi = x >> cnt;

		if (x < 0)
		{
			y -= xi;
			x += yi;

			result -= atanTable[cnt];
		} else {
			y += xi;
			x -= yi;

			result += atanTable[cnt];
		}
	}

	return result;
}
