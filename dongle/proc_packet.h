#pragma once

bool process_packet(__xdata tracker_position_packet_t* pckt);

// drift compensation configuration
void save_x_drift_comp(void);

// recenter requested
void recenter(void);

// restets the mouse delta calculation
void reset_mouse_delta(void);

extern int32_t sample_cnt;
extern int16_t yaw_value;

extern FeatRep_RawMagSamples raw_mag_samples;
