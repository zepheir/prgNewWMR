#ifndef __RS485_H__
#define __RS485_H__

#include "main.h"

#define h_rs485 huart1
#define RS485_RESP_TIME_MAX 3 // 300ms

typedef enum{
  RS485_READY,
  RS485_WAITING_RESP,
  RS485_TIMEOUT
} RS485_STATE;

void RS485_Receiver(void);
void RS485_Receiver_TimeoutMode(void);
void RS485_Out(uint8_t str[], uint8_t size);

#endif // __RS485_H__