#include <algorithm>
#include <string>
#include <vector>

#include <stdio.h>
#include <stdint.h>
#include <math.h>

extern "C"
{
#include "../dongle/math_cordic.h"
};

int16_t data[][5] = {
{-257,455,864,   -2,63},
{-253,487,871,   -2,63},
{-240,480,878,   -2,61},
{-249,428,837,   2,61},
{-204,477,864,   2,69},
{-235,486,837,   18,59},
{-255,422,843,   754,-51},
{-379,476,781,   1518,13},
{-434,479,760,   2402,93},
{-497,516,781,   3050,115},
{-575,502,712,   3778,49},
{-576,423,635,   4366,-39},
{-580,482,670,   4758,-43},
{-633,466,670,   5058,-53},
{-651,454,622,   5490,-101},
{-681,450,615,   5786,-91},
{-721,504,552,   6066,97},
{-708,504,608,   6338,203},
{-678,502,539,   6358,247},
{-721,504,538,   6378,259},
{-694,470,552,   6002,81},
{-701,483,545,   5622,93},
{-652,467,656,   5110,19},
{-610,484,670,   4698,21},
{-611,497,691,   4090,111},
{-520,498,712,   3346,213},
{-479,515,733,   2866,277},
{-422,478,733,   2142,361},
{-412,511,781,   1686,421},
{-355,474,795,   1346,457},
{-340,519,864,   1082,489},
{-321,511,837,   710,523},
{-289,490,837,   610,461},
{-315,504,830,   498,409},
{-252,474,906,   -62,93},
{-222,472,871,   -314,91},
{-160,448,878,   -946,71},
{-113,478,934,   -1310,69},
{-90,490,878,   -1942,77},
{-52,468,927,   -2654,93},
{11,437,927,   -3042,101},
{44,481,879,   -3554,113},
{118,456,913,   -4030,125},
{143,455,893,   -4638,143},
{190,478,906,   -4974,153},
{191,458,851,   -5326,165},
{221,456,872,   -5558,169},
{241,435,844,   -5678,173},
{253,441,851,   -5858,177},
{268,492,893,   -5950,179},
{277,440,886,   -6006,179},
{231,488,837,   -5582,175},
{204,451,893,   -5310,169},
{137,449,872,   -4678,159},
{76,459,865,   -3946,141},
{20,482,865,   -3126,125},
{-58,461,871,   -2022,107},
{-99,451,871,   -1558,97},
{-143,473,864,   -978,85},
{-143,473,844,   -846,93},
{-185,463,878,   -490,95},
{-252,474,871,   -194,93},
{-259,481,864,   -78,95},
{-252,474,885,   38,93},
{-210,484,802,   30,93},
{-258,468,830,   30,93},
{-276,463,850,   38,77},
{-249,428,837,   182,-143},
{-267,423,843,   242,-409},
{-286,431,837,   226,-787},
{-306,367,892,   194,-1267},
{-254,311,885,   154,-1897},
{-256,239,926,   98,-2681},
{-234,172,919,   90,-3339},
{-291,111,912,   126,-3903},
{-291,111,912,   170,-4425},
{-238,36,871,   206,-4827},
{-277,-8,905,   230,-5269},
{-258,-22,884,   230,-5447},
{-272,-1,912,   230,-5523},
{-289,-20,884,   230,-5663},
{-280,-60,912,   226,-5893},
{-313,-123,933,   222,-6127},
{-241,-114,905,   222,-6365},
{-277,-105,974,   214,-6577},
{-286,-164,912,   210,-6859},
{-275,-151,926,   206,-6731},
{-272,-99,905,   230,-6179},
{-270,-28,912,   242,-5587},
{-304,27,954,   242,-5025},
{-264,70,905,   162,-3955},
{-222,178,947,   106,-3169},
{-305,249,905,   166,-2127},
{-275,345,892,   210,-1417},
{-276,365,913,   242,-989},
{-254,409,913,   274,-499},
{-276,476,906,   282,-251},
{-240,480,878,   82,59},
{-208,439,871,   58,105},
{-303,517,816,   450,375},
{-291,509,823,   458,377},
{-271,482,830,   478,423},
{-274,528,844,   510,815},
{-312,569,823,   562,1577},
{-269,651,747,   614,2291},
{-248,702,740,   550,3137},
{-274,737,664,   498,3809},
{-282,763,574,   522,4525},
{-297,810,574,   530,5283},
{-276,867,470,   550,5935},
{-283,874,450,   554,6333},
{-315,902,408,   562,6599},
{-289,881,394,   570,6885},
{-335,930,394,   558,7155},
{-285,907,381,   570,7333},
{-287,933,401,   574,7511},
{-289,966,367,   574,7749},
{-298,921,353,   570,7357},
{-280,926,422,   558,6629},
{-301,882,505,   534,5521},
{-341,839,588,   514,4661},
{-285,711,664,   494,3443},
{-286,724,699,   514,3171},
{-331,675,705,   522,2789},
{-293,653,733,   534,2559},
{-292,633,712,   558,2363},
{-318,661,740,   558,2049},
{-304,530,781,   702,1163},
{-321,511,802,   718,985},
{-304,530,830,   742,693},
{-324,557,837,   754,493},
{-291,522,837,   738,441},
{-333,512,823,   590,369},
{-300,458,843,   410,285},
{-234,473,850,   22,39},
{-277,489,837,   18,43},
{-283,483,864,   18,49},
{-227,453,878,   18,51},
};

const char* dmag(int ndx, int sign)
{
	if (sign == 1)
	{
		if (ndx == 0)
			return "mag[0]";
		else if (ndx == 1)
			return "mag[1]";
		else if (ndx == 2)
			return "mag[2]";
	} else {
		if (ndx == 0)
			return "-mag[0]";
		else if (ndx == 1)
			return "-mag[1]";
		else if (ndx == 2)
			return "-mag[2]";
	}

	return "???";
}

const char* dang(int ndx, int sign)
{
	if (sign == 1)
	{
		if (ndx == 3)
			return "euler[ROLL]";
		else if (ndx == 4)
			return "euler[PITCH]";
	} else {
		if (ndx == 3)
			return "-euler[ROLL]";
		else if (ndx == 4)
			return "-euler[PITCH]";
	}

	return "???";
}

std::string dtrig(long double (*func) (long double), int ndx, int sign)
{
	std::string retval;

	if (func == sinl)
	{
		if (sign == -1)
			retval = "sin(-";
		else
			retval = "sin(";

		if (ndx == 3)
			retval += "roll)";
		else if (ndx == 4)
			retval += "pitch)";

	} else if (func == cosl) {

		// cos(x) == cos(-x)

		if (ndx == 3)
			retval = "cos(roll)";
		else if (ndx == 4)
			retval = "cos(pitch)";
	}

	return retval;
}

std::string int2str(const int64_t i)
{
	const int BUFF_SIZE = 64;
	char buffer[BUFF_SIZE];
	sprintf_s(buffer, BUFF_SIZE, "%ld", i);
	return buffer;
}

std::string d1t(int sgn, int mndx, int msgn, long double (*f1) (long double), int angndx)
{
	std::string rv;

	if (sgn*msgn == -1)
		rv = "-";
	else
		rv = "+";

	rv += "mul_16x16(mag[" + int2str(mndx) + "],";
	rv += (f1 == sinl ? "sin" : "cos");
	rv += (angndx == 3 ? "roll" : "pitch");
	rv += ")";

	return rv;
}

std::string d2t(int sgn, int mndx, int msgn, long double (*f1) (long double), int angndx, int angsgn)
{
	std::string rv;

	if (sgn*msgn*angsgn == -1)
		rv = "-";
	else
		rv = "+";

	rv += "mul_16x16(mag[" + int2str(mndx) + "],";
	rv += (f1 == sinl ? "sin" : "cos");
	rv += (angndx == 3 ? "roll" : "pitch");
	rv += ")";

	return rv;
}

int32_t run_func(uint32_t func_decl, bool print_decl, bool print_vals = false)
{
	uint8_t xndx, yndx, zndx;
	uint8_t rollndx, pitchndx;
	int8_t xsign, ysign, zsign;
	int8_t rollsign, pitchsign;

	switch (func_decl % 6)
	{
	case 0:
		xndx = 0;
		yndx = 1;
		zndx = 2;
		break;
	case 1:
		xndx = 0;
		yndx = 2;
		zndx = 1;
		break;
	case 2:
		xndx = 1;
		yndx = 0;
		zndx = 2;
		break;
	case 3:
		xndx = 1;
		yndx = 2;
		zndx = 0;
		break;
	case 4:
		xndx = 2;
		yndx = 0;
		zndx = 1;
		break;
	case 5:
		xndx = 2;
		yndx = 1;
		zndx = 0;
		break;
	}

	func_decl /= 6;

	if (func_decl & 1)
	{
		rollndx  = 3;
		pitchndx = 4;
	} else {
		rollndx  = 4;
		pitchndx = 3;
	}
	func_decl >>= 1;

	xsign = (func_decl & 1) ? -1 : 1;
	func_decl >>= 1;

	ysign = (func_decl & 1) ? -1 : 1;
	func_decl >>= 1;

	zsign = (func_decl & 1) ? -1 : 1;
	func_decl >>= 1;

	rollsign = (func_decl & 1) ? -1 : 1;
	func_decl >>= 1;

	pitchsign = (func_decl & 1) ? -1 : 1;
	func_decl >>= 1;

	int8_t sgn[5];

	int c;
	for (c = 0; c < 5; ++c)
	{
		sgn[c] = (func_decl & 1) ? -1 : 1;
		func_decl >>= 1;
	}

	long double (*f[7]) (long double);

	for (c = 0; c < 7; c++)
	{
		if (func_decl & 1)
			f[c] = sinl;
		else
			f[c] = cosl;

		func_decl >>= 1;
	}

	// avoid things that are impossible
	if (f[0] == f[1])
		return 0xffffff;

	if (f[3] == f[5]  &&  f[4] == f[6]  &&  f[3] == f[4])
		return 0xffffff;

	if (print_decl)
	{
		printf("int16_t %s%s_%s%s, %s%s_%s%s; ", 
					(f[3] == sinl ? "sin" : "cos"),
					(pitchndx == 3 ? "roll" : "pitch"),
					(f[4] == sinl ? "sin" : "cos"),
					(rollndx == 3 ? "roll" : "pitch"),
					(f[5] == sinl ? "sin" : "cos"),
					(pitchndx == 3 ? "roll" : "pitch"),
					(f[6] == sinl ? "sin" : "cos"),
					(rollndx == 3 ? "roll" : "pitch"));

		if (rollndx == 3  &&  rollsign == -1  ||  pitchndx == 3  &&  pitchsign == -1)
			printf("isincos_cord(-euler[ROLL], &cosroll, &sinroll); ");
		else
			printf("isincos_cord(euler[ROLL], &cosroll, &sinroll); ");

		if (rollndx == 4  &&  rollsign == -1  ||  pitchndx == 4  &&  pitchsign == -1)
			printf("isincos_cord(-euler[PITCH], &cospitch, &sinpitch); ");
		else
			printf("isincos_cord(euler[PITCH], &cospitch, &sinpitch); ");

		printf("Yh = %s%s; ",
					d1t(sgn[0], zndx, zsign, f[0], rollndx).c_str(),
					d1t(sgn[1], yndx, ysign, f[1], rollndx).c_str());

		printf("Xh = mul_16x16(%s%s, %s%s);", 
					(f[3] == sinl ? "sin" : "cos"),
					(pitchndx == 3 ? "roll" : "pitch"),
					(f[4] == sinl ? "sin" : "cos"),
					(rollndx == 3 ? "roll" : "pitch"));

		printf("%s%s_%s%s = Xh / 0x8000; ",
					(f[3] == sinl ? "sin" : "cos"),
					(pitchndx == 3 ? "roll" : "pitch"),
					(f[4] == sinl ? "sin" : "cos"),
					(rollndx == 3 ? "roll" : "pitch"));

		printf("Xh = mul_16x16(%s%s, %s%s); ", 
					(f[5] == sinl ? "sin" : "cos"),
					(pitchndx == 3 ? "roll" : "pitch"),
					(f[6] == sinl ? "sin" : "cos"),
					(rollndx == 3 ? "roll" : "pitch"));

		printf("%s%s_%s%s = Xh / 0x8000; ",
					(f[5] == sinl ? "sin" : "cos"),
					(pitchndx == 3 ? "roll" : "pitch"),
					(f[6] == sinl ? "sin" : "cos"),
					(rollndx == 3 ? "roll" : "pitch"));

		printf("Xh = %s %s mul_16x16(mag[%i], %s%s_%s%s) %s mul_16x16(mag[%i], %s%s_%s%s);\n",
					d1t(sgn[2], xndx, xsign, f[2], pitchndx).c_str(),
					
					(sgn[3] * ysign == -1 ? "-" : "+"),
					yndx,
					(f[3] == sinl ? "sin" : "cos"),
					(pitchndx == 3 ? "roll" : "pitch"),
					(f[4] == sinl ? "sin" : "cos"),
					(rollndx == 3 ? "roll" : "pitch"),

					(sgn[4] * zsign == -1 ? "-" : "+"),
					zndx,
					(f[5] == sinl ? "sin" : "cos"),
					(pitchndx == 3 ? "roll" : "pitch"),
					(f[6] == sinl ? "sin" : "cos"),
					(rollndx == 3 ? "roll" : "pitch"));

		/*
		printf("Yh = %s%s*%s %s %s*%s; ",
					(sgn[0]*zsign == -1) ? "-" : "",
					dmag(zndx, 1),
					dtrig(f[0], rollndx, rollsign).c_str(),

					(sgn[1]*ysign == -1) ? "-" : "+",
					dmag(yndx, 1),
					dtrig(f[1], rollndx, rollsign).c_str()
					);

		printf("Xh = %s%s*%s %s %s*%s*%s %s %s*%s*%s;\n",
					(sgn[2]*xsign == -1) ? "-" : "",
					dmag(xndx, 1),
					dtrig(f[2], pitchndx, pitchsign).c_str(),

					(sgn[3]*ysign == -1) ? "-" : "+",

					dmag(yndx, 1),	
					dtrig(f[3], pitchndx, pitchsign).c_str(),
					dtrig(f[4], rollndx, rollsign).c_str(),

					(sgn[4]*zsign == -1) ? "-" : "+",

					dmag(zndx, 1),
					dtrig(f[5], pitchndx, pitchsign).c_str(),
					dtrig(f[6], rollndx, rollsign).c_str()
					);
					*/
	}

	double Xh, Yh;
	double res = 0;
	double offset;
	for (int c = 0; c < sizeof(data) / sizeof(data[0]); ++c)
	{
		const int16_t* d = data[c];

		const double x = xsign * d[xndx];
		const double y = ysign * d[yndx];
		const double z = zsign * d[zndx];
		const double pitch = d[pitchndx] * pitchsign / 10430.06;
		const double roll  = d[rollndx] * rollsign / 10430.06;

		Yh = sgn[0] * z * f[0](roll) + sgn[1] * y * f[1](roll);
		Xh = sgn[2] * x * f[2](pitch) + sgn[3] * y * f[3](pitch) * f[4](roll) + sgn[4] * z * f[5](pitch) * f[6](roll);

		double heading = atan2(Yh, Xh) * 10430.06;

		if (c == 0)
			offset = heading;

		if (print_vals)
			printf("%i\n", int32_t(heading - offset));

		res += abs(heading - offset);
	}
	
	return int32_t(res);
}

#define ROLL	1
#define PITCH	2

#define mul_16x16(a, b)		(a)*(b)

void test()
{
	//int offset;
	int sum = 0;
	double Yhd, Xhd;
	int32_t Yh, Xh;
	//for (int c = 0; c < sizeof(data) / sizeof(data[0]); ++c)
		int c = 117;
	{
		int16_t* euler = data[c] + 2;
		int16_t* mag = data[c];

		double roll = euler[ROLL] / 10430.06;
		double pitch = euler[PITCH] / 10430.06;

		Yhd = mag[1]*sin(roll)*0x7fff - mag[2]*cos(roll)*0x7fff;
		Xhd = mag[0]*sin(pitch)*0x7fff + mag[2]*sin(pitch)*sin(roll)*0x7fff + mag[1]*sin(pitch)*cos(roll)*0x7fff;

		int32_t r = int32_t(atan2(Yhd, Xhd) * 10430.06 + 0.5);

		/*
		if (c == 0)
			offset = r;

		sum += abs(r - offset);
		*/

		int16_t cosroll, sinroll, cospitch, sinpitch;

int16_t sinpitch_sinroll, sinpitch_cosroll;

isincos_cord(euler[ROLL], &cosroll, &sinroll);
isincos_cord(euler[PITCH], &cospitch, &sinpitch);

Yh = +mul_16x16(mag[1],sinroll)-mul_16x16(mag[2],cosroll);

Xh = mul_16x16(sinpitch, sinroll);
sinpitch_sinroll = Xh / 0x8000;

Xh = mul_16x16(sinpitch, cosroll);
sinpitch_cosroll = Xh / 0x8000;

Xh = mul_16x16(mag[0],sinpitch) + mul_16x16(mag[2], sinpitch_sinroll) + mul_16x16(mag[1], sinpitch_cosroll);

		int32_t r2 = iatan2_cord(Yh, Xh);

		printf("%i %i\n", c, r - r2);

		printf("%f %f\n", sin(pitch)*sin(roll)*0x7fff, sin(pitch)*cos(roll)*0x7fff);
		printf("%i %i\n", sinpitch_sinroll, sinpitch_cosroll);
	}
}

int main()
{
	//test();
	//puts("----------------------------");
	//run_func(860601, true, false);

	int best = 0xffffff;
	int curr;
	for (int i = 0; i < 1572864; ++i)
	{
		curr = run_func(i, false);
		if (curr < 60000)
		{
			printf("%5i: ", curr);
			run_func(i, true);
			best = curr;
		}
	}
	
	return 0;
}
