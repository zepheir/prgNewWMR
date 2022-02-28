#ifndef __GPRS_7G3_H__
#define __GPRS_7G3_H__

#include "main.h"

#define h_gprs huart2

typedef enum{
  GPRS_RX_READY,
  GPRS_RX_WAITING_RESP,
  GPRS_RX_TIMEOUT
} GPRS_RX_STATE;

typedef enum {
    GPRS_READY,
    // GPRS_RX_WAITING_RESP,
    // GPRS_TIMEOUT,
    GPRS_SEND_HELLO,
    GPRS_WAITING_A,
    GPRS_REPLY_A,
    GPRS_WAITING_A_OK,
    GRPS_AT_MODE_READY,
    GPRS_SEND_AT_ENTM,
    GPRS_WAITING_AT_ENTM
}GPRS_STATE;

#define GPRS_RESP_TIMER_MAX 5 // 500ms

void gprsReceiver(void);
void gprs_Receiver_TimeoutMode(void);
void gprs_Send(uint8_t *pStr, uint8_t size);
void gprs_Enter_Setting(void);
void gprs_Exit_At_Mode(void);


#endif // __GPRS_7G3_H__