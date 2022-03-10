#ifndef __GPRS_7G3_H__
#define __GPRS_7G3_H__

#include "main.h"

#define h_gprs huart2
#define GPRS_PINS_FILTER_MAX    50 // 5000ms
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
    GPRS_TYPE_CHECK,
    GPRS_TYPE_CHECK_WAIT,
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
    GPRS_FACTORY_SETTING,
    GPRS_SET_UATEN,
    GPRS_SET_UATEN_WAIT,
    GPRS_SET_HEARTEN_OFF,
    GPRS_SET_HEARTEN_OFF_WAIT,
    GPRS_SET_SERVER,
    GPRS_SET_SERVER_WAIT,
    GPRS_FACTORY_SETTING_OK,
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
  enum {
    TYPE_NONE,
    TYPE_7G3,
    TYPE_7G4
  } type;
  enum {
    GPRS_WORK_OFF,
    GPRS_WORK_ON
  } work;
  uint8_t work_filter;
  enum {
    GPRS_NET_OFF,
    GPRS_NET_ON
  } net;
  uint8_t net_filter;
  enum {
    GPRS_LINK_OFF,
    GPRS_LINK_ON
  } link;
  uint8_t link_filter;
} GPRS;


#define GPRS_RESP_TIMER_MAX 5 // 500ms

void gprsReceiver(void);
void gprs_Receiver_TimeoutMode(void);
void gprs_Send(char *pStr);

// void gprs_Get_IMEI(void);
void gprs_Enter_Setting(void);
void gprs_Exit_At_Mode(void);

void gprs_Ini(void);
void gprs_state_update(void);
void gprs_Remote_Req(void);

void gprs_Factory_Setting(void);


#endif // __GPRS_7G3_H__