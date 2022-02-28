#ifndef __GPRS_7G3_H__
#define __GPRS_7G3_H__

#define h_gprs huart2

typedef enum{
  GPRS_RX_READY,
  GPRS_RX_WAITING_RESP,
  GPRS_RX_TIMEOUT
} GPRS_RX_STATE;

#define GPRS_RESP_TIMER_MAX 5 // 500ms

void gprsReceiver(void);
void gprs_Receiver_TimeoutMode(void);

void gprs_Enter_Setting(void);


#endif // __GPRS_7G3_H__