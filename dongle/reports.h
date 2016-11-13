#pragma once

// we need to tell MS VC++ to pack the struct members as close as possible
#ifdef _MSC_VER
# pragma pack(push)
# pragma pack(1)
#endif

// *****************************************************************
// *****************************************************************
// *****************************************************************

// this is the main HID joystick report
// it contains the axis data we're sending to the PC.

#define JOYSTICK_REPORT_ID		1

typedef struct
{
	uint8_t	report_id;		// == JOYSTICK_REPORT_ID

	int16_t	x;
	int16_t	y;
	int16_t	z;
	
	uint16_t buttons;

} hid_joystick_report_t;

extern hid_joystick_report_t	usb_joystick_report;

// *****************************************************************
// *****************************************************************
// *****************************************************************

// this is the HID mouse report

#define MOUSE_REPORT_ID			2

typedef struct
{
	uint8_t		report_id;		// == MOUSE_REPORT_ID

	int16_t		x;
	int16_t		y;
	int16_t		wheel;

} hid_mouse_report_t;

// the HID joystick report
extern hid_mouse_report_t	usb_mouse_report;

// inits the joystick and mouse reports
void reset_main_reports(void);

// *****************************************************************
// *****************************************************************
// *****************************************************************

#define DONGLE_SETTINGS_REPORT_ID		3

#define MAG_MATRIX_SCALE_BITS			12

enum yaw_correction_method_e
{
	ycm_auto,		// magnetometer if present, autocenter and yaw drift compensation if no magnetometer
	ycm_ac_only,	// only autocenter and yaw drift comp
	ycm_mag_only,	// only magnetometer
	ycm_none,		// no drift compensation - just let it drift
};

// data direction: dongle <-> PC
typedef struct
{
	uint8_t		report_id;		// == DONGLE_SETTINGS_REPORT_ID

	// what are we impersonating?
	uint8_t		active_devices;	// 1 == joystick, 2 == mouse, 3 == both
	
	uint8_t		tracker_orient;	// 0 == on/off switch is to the right
								// 1 == on/off switch is facing forward

	uint8_t		yc_method;		// one of yaw_correction_method_e values

	// joystick axis response
	uint8_t		is_linear;		// 1 == linear, 0 == exponential

	// joystick axis response factors
	int8_t		joy_fact[3];

	uint8_t		mouse_sens[3];

	// drift compensation per 1024 samples
	int16_t		drift_per_1k;

	// magnetometer calibration and sensitivity
	int16_t		mag_offset[3];
	int16_t		mag_matrix[3][3];
	uint8_t		mag_sens;

} FeatRep_DongleSettings;

// *****************************************************************
// *****************************************************************
// *****************************************************************

#define COMMAND_REPORT_ID		4

enum tracker_commands_t
{
	CMD_CALIBRATE				= 1,	// PC -> dongle -> tracker
	CMD_READ_TRACKER_SETTINGS	= 2,	// dongle -> tracker

	CMD_RECENTER				= 3,	// PC -> dongle
	CMD_SAVE_DRIFT				= 4,	// PC -> dongle
	CMD_DISABLE_TOGGLE			= 5,	// PC -> dongle
	
	// these deal with sleeping: search sleep_mode
	CMD_DISABLE_SLEEP			= 6,
	CMD_SLEEP_AFTER_10_SECONDS	= 7,
	CMD_SLEEP_AFTER_30_SECONDS	= 8,
	CMD_SLEEP_AFTER_1MIN		= 9,
	CMD_SLEEP_AFTER_2MIN		= 10,
	CMD_SLEEP_AFTER_5MIN		= 11,
	CMD_SLEEP_AFTER_10MIN		= 12,
	CMD_SLEEP_AFTER_30MIN		= 13,
	CMD_SLEEP_AFTER_60MIN		= 14,

	CMD_SLEEP_SHORTEST			= 7,
	CMD_SLEEP_LONGEST			= 14,
};

#define NUM_MAG_SENS_VALUES		6
#define NUM_MOUSE_SENS_VALUES	9

// direction: PC -> dongle
typedef struct
{
	uint8_t		report_id;		// == COMMAND_REPORT_ID
	uint8_t		command;
} FeatRep_Command;

// *****************************************************************
// *****************************************************************
// *****************************************************************

#define TRACKER_SETTINGS_REPORT_ID		5

// direction: dongle -> PC
typedef struct
{
	uint8_t		report_id;		// == TRACKER_SETTINGS_REPORT_ID

	uint8_t		has_tracker_responded;

	uint8_t		is_calibrated;	// if gyro and accel have been calibrated

	int16_t		gyro_bias[3];
	int16_t		accel_bias[3];

	uint8_t		sleep_mode;

} FeatRep_TrackerSettings;

// *****************************************************************
// *****************************************************************
// *****************************************************************

#define STATUS_REPORT_ID				6

// direction: dongle -> PC
typedef struct
{
	uint8_t		report_id;		// == STATUS_REPORT_ID

	uint8_t		num_packets;	// number of packets received in the last second

	int32_t		sample_cnt;		// number of samples since reset
	int32_t		yaw_value;		// current yaw value

	uint16_t	battery_voltage;	// in units of 100th of a Volt, so 289 means 2.89V
	uint16_t	temperature;		// in units of 10th of a Celsius, so 289 means 28.9C
	int16_t		shutdown_counter;	// when this reaches 0 - the tracker goes to sleep
	
	uint8_t		is_enabled;			// 0 when joystick and the mouse are disabled

} FeatRep_Status;

// *****************************************************************
// *****************************************************************
// *****************************************************************

#define RAW_MAG_REPORT_ID		7

// direction: dongle -> PC
struct mag_point_t
{
	int16_t	x, y, z;

#ifdef __cplusplus
	// the C++ member functions - used in WHTConfig

	mag_point_t()
		: x(0), y(0), z(0)
	{}

	mag_point_t(const int16_t nx, const int16_t ny, const int16_t nz)
		: x(nx), y(ny), z(nz)
	{}

	bool operator < (const mag_point_t& rhs) const
	{
		return x == rhs.x ? (y == rhs.y ? z < rhs.z : y < rhs.y) : x < rhs.x;
	}

	bool operator == (const mag_point_t& rhs) const
	{
		return x == rhs.x
				&&  y == rhs.y
				&&  z == rhs.z;
	}

	float CalcLength() const
	{
		return sqrt(float(x*x + y*y + z*z));
	}
#endif
};

#define MAX_RAW_MAG_SAMPLES		10

typedef struct
{
	uint8_t				report_id;		// == RAW_MAG_REPORT_ID

	uint8_t				samples_desc;	// MSbit -> 0 for AKM, 1 for HMC
	struct mag_point_t	mag[MAX_RAW_MAG_SAMPLES];

} FeatRep_RawMagSamples;

// *****************************************************************
// *****************************************************************
// *****************************************************************

#define PROT_VER_REPORT_ID	8

// used to recognize different versions of the software/firmware on the PC-dongle-tracker
enum protocol_version_e
{
	pv_unknown = 0,		// pre protocol version enabled dongles
	pv_initial = 1,		// version 1 of the tracker and dongle
	pv_with_mouse = 2,	// version 2 supports mouse output
	pv_ila = 3,			// version 3 added mag sens, swap x and wheel and sleep mode upgrade
						// and versioning actually starts working from here
};

#define CURR_PROTOCOL_VER		pv_ila

// direction: dongle -> PC
typedef struct
{
	uint8_t		report_id;		// == PROT_VER_REPORT_ID

	uint8_t		dongle_ver;
	uint8_t		tracker_ver;

} FeatRep_ProtocolVersion;

// *****************************************************************
// *****************************************************************
// *****************************************************************

#ifdef _MSC_VER
# pragma pack(pop)
#endif
