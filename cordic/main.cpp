// These are tests for the cordic trigonometric functions in ../dongle/math_cordic.c

// we need this so that the STD library defines M_PI for us
#define _USE_MATH_DEFINES

#include <stdio.h>
#include <math.h>
#include <stdint.h>

extern "C"
{
#include "../dongle/math_cordic.h"
}

// this creates the atanTable needed in math_cordic.c
void create_atan_table()
{
	float val = 1;
	for (int i = 0; i < CORDIC_TABLE_SIZE; i++)
	{
		double atanVal = atanf(val) * (ANGLE_PI / M_PI);
		int atanValInt = int(atanVal + 0.5);

		printf("%d,\t// %i\tatan(%g) * (CORDIC_RANGE / M_PI)\n", atanValInt, i, val);
		val /= 2;
	}
}

void calc_cordic_gain()
{
	double atanarg = 1;
	double cordic_gain = 1;
	for (int i = 0; i < CORDIC_TABLE_SIZE; i++)
	{
		cordic_gain *= cos(atan(atanarg));
		atanarg /= 2;
	}

	printf("CORDIC_GAIN = %18.16f\n", cordic_gain);
}

void test_sincos()
{
	int16_t s, c;

	for (int32_t angle = -ANGLE_PI; angle <= ANGLE_PI; angle += ANGLE_PI/45)
	{
		isincos_cord(angle, &c, &s);

		double rad_angle = angle / (double)ANGLE_PI * M_PI;
		int32_t sc = int32_t(sin(rad_angle) * ANGLE_PI + .5);
		int32_t cc = int32_t(cos(rad_angle) * ANGLE_PI + .5);

		printf("a=%4d sc=%6d s=%6d cc=%6d c=%6d sdif=%6d cdif=%6d\n", angle * 180 / ANGLE_PI, s, sc, c, cc, s-sc, c-cc);
	}
}

void test_sincos_mc()
{
	int16_t s, c;
	int32_t angle;

	for (angle = -ANGLE_PI; angle <= ANGLE_PI; angle += ANGLE_PI/45)
	{
		isincos_cord(angle, &c, &s);

		printf("a=%6ld s=%6d c=%6d\n", angle, s, c);
	}
}

#define ATAN_RANGE	100000
#define ATAN_STEP	(ATAN_RANGE/100)

void test_atan()
{
	for (int32_t x = -ATAN_RANGE; x < ATAN_RANGE; x += ATAN_STEP)
	{
		for (int32_t y = -ATAN_RANGE; y < ATAN_RANGE; y += ATAN_STEP)
		{
			int32_t res = int32_t(atan2((double) x, (double) y) / M_PI * ANGLE_PI + .5);
			int32_t resc = iatan2_cord(x, y);

			printf("math=%8i  cord=%8i  diff=%6i\n", res, resc, resc - res);
		}
	}
}

void main()
{
	test_sincos();
}
