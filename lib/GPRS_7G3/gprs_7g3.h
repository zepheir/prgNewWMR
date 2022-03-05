#ifndef __GPRS_7G3_H__
#define __GPRS_7G3_H__

#include "main.h"

#define h_gprs huart2
#define GPRS_REMOTE_TIMEOUT_MAX 60
#define TCP_SERVER	"121.199.16.44"
#define TCP_PORT		"6969"

typedef enum{
  GPRS_RX_READY,
  GPRS_RX_WAITING_RESP,
  GPRS_RX_TIMEOUT
} GPRS_RX_STATE;

typedef enum {
    GPRS_INI,
    GPRS_READ_IMEI,
    GPRS_READ_IMEI_WAIT,
    GPRS_GET_CSQ,
    GPRS_GET_CSQ_WAIT,
    GPRS_GET_SERVER,
    GPRS_GET_SERVER_WAIT,
    GPRS_READY,
    // GPRS_RX_WAITING_RESP,
    // GPRS_TIMEOUT,
    GPRS_SEND_HELLO,
    GPRS_WAITING_A,
    GPRS_REPLY_A,
    GPRS_WAITING_A_OK,
    GRPS_AT_MODE_READY,
    GPRS_SEND_AT_ENTM,
    GPRS_WAITING_AT_ENTM,
    GPRS_REMOTE_REQ,
    GPRS_REMOTE_REQ_WAIT,
    GPRS_REMOTE_SENDDATA,
    GPRS_REMOTE_SENDDATA_WAIT,
    GPRS_REMOTE_SETDATA,
    GPRS_REMOTE_TIMEOUT
}GPRS_STATE;

typedef struct 
{
  char imei[15];
  int16_t csq; // 信号强度
} GPRS_7G3;


#define GPRS_RESP_TIMER_MAX 5 // 500ms

void gprsReceiver(void);
void gprs_Receiver_TimeoutMode(void);
void gprs_Send(char *pStr);

// void gprs_Get_IMEI(void);
void gprs_Enter_Setting(void);
void gprs_Exit_At_Mode(void);

void gprs_Ini(void);
void gprs_Remote_Req(void);


#endif // __GPRS_7G3_H__