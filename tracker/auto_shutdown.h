#ifndef AUTO_SHUTDOWN_H
#define AUTO_SHUTDOWN_H

void init_auto_shutdown(void);
int16_t check_auto_shutdown(const int16_t gyro[3]);

void ISR_TICK() __interrupt INTERRUPT_TICK;

#endif	// AUTO_SHUTDOWN_H