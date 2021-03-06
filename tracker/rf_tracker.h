#ifndef RF_HEAD_H
#define RF_HEAD_H

void rf_tracker_init(void);

// LED status will be set to LED_STATUS_NOT_RECEIVED if no status has been received
#define LED_STATUS_NOT_RECEIVED	0xff

bool rf_tracker_send_message(const void* buff, const uint8_t num_bytes);
uint8_t rf_tracker_read_ack_payload(void* buff, const uint8_t buff_size);

#endif