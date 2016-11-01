#ifndef RF_PROTOCOL_H
#define RF_PROTOCOL_H

// we use the full, 5 byte address length
#define NRF_ADDR_SIZE	5

// define RF data rate
#define NRF_DATA_RATE	vRF_DR_1MBPS

// the channel number is hard-coded for the moment
#if SECOND_ADDR
# define CHANNEL_NUM		110
#else
# define CHANNEL_NUM		115
#endif

// these are the addresses
extern __code const uint8_t HeadAddr[NRF_ADDR_SIZE];
extern __code const uint8_t DongleAddr[NRF_ADDR_SIZE];

enum pos_packet_flags
{
	FLAG_RECENTER		= 0x01,
	FLAG_MAG_VALID		= 0x04,
	FLAG_MAG_SENSOR		= 0x08,		// 0 - AK8975A (MPU9150), 1 - HMC5883L
};

// the packet containing the position data of the tracker
typedef struct
{
	uint8_t   /* 1 */	tracker_ver;	// the protocol version
	uint8_t   /* 1 */	flags;			// bits defined in pos_packet_flags
	uint16_t  /* 2 */	mouse_buttons;	// the lower 12 bits contain the states of the mouse buttons
	int16_t   /* 8 */	quat[4];
	int16_t   /* 6 */	mag[3];			// the magnetometer readings
} tracker_position_packet_t;

// the packet containing other stuff which is not needed as often as the position packet
typedef struct
{
	uint8_t   /* 1 */	tracker_ver;		// the protocol version
	uint8_t   /* 1 */	flags;				// bits defined in mpu_packet_flags
	int16_t   /* 2 */	voltage;			// in units of 100th of a Volt, so 289 means 2.89V
	int16_t   /* 2 */	temperature;		// in units of 10th of a Celsius, so 289 means 28.9C
	int16_t   /* 2 */	shutdown_counter;	// when this reaches 0 - the tracker goes to sleep, -1 for when disabled
	int16_t   /* 6 */	mag_self_test[3];	// the self test
	//uint8_t   /* 5 */	tracker_id[5];		// the ID of the tracker
} tracker_aux_packet_t;

#endif		// RF_PROTOCOL_H
