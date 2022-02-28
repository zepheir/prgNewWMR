#include "gprs_7g3.h"
#include "rs485.h"
#include "main.h"
#include "func.h"
#include "para.h"
#include "usart.h"
#include "string.h"


uint8_t gprs_rx_cnt = 0;
extern uint8_t bRxBuffer;
uint8_t gprs_resp_timer;
uint8_t gprsRxBuffer[256];
extern SYS_MODE sys_mode;

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
    GRPS_WAITING_AT_ENTM
}GPRS_STATE;
GPRS_STATE gprs_state;

GPRS_RX_STATE gprs_rx_state;


void gprsReceiver(void)
{
    // 如果溢出了
    if (gprs_rx_cnt >= 255)
    {
        gprs_rx_cnt = 0;
        memset(gprsRxBuffer, 0x00, sizeof(gprsRxBuffer));
        //   HAL_UART_Transmit(&h_rs485, "GPRS RX over buff size", 23, 0xffff);
        RS485_Out("GPRS RX over buff size", 23);
    }
    else
    {
        gprsRxBuffer[gprs_rx_cnt++] = bRxBuffer;

        gprs_rx_state = GPRS_RX_WAITING_RESP;
    }
}


void gprs_Receiver_TimeoutMode(void){
    if (gprs_rx_state == GPRS_RX_WAITING_RESP)
    {
        if (gprs_resp_timer > 0)
        {
            gprs_resp_timer--;
        }
        else
        {
            gprs_resp_timer = GPRS_RESP_TIMER_MAX;
            gprs_rx_state = GPRS_RX_TIMEOUT;

            // HAL_UART_Transmit(&huart1, (uint8_t *)&RxBuffer, rs485_rx_cnt, 0xffff);
            // while (HAL_USART_GetState(&h_rs485) == HAL_UART_STATE_BUSY_TX);
            RS485_Out(&gprsRxBuffer, gprs_rx_cnt);

            gprs_rx_cnt = 0;
            memset(gprsRxBuffer, 0x00, sizeof(gprsRxBuffer));

            gprs_rx_state = GPRS_RX_READY;
        }
    }
}



void gprs_Enter_Setting(void){
    
    switch (gprs_state)
    {
    case GPRS_READY:
        RS485_Out(">>+++", 5);
        HAL_UART_Transmit(&h_gprs, "+++", 3, 0xffff);
        gprs_state = GPRS_WAITING_A;
        break;
    case GPRS_WAITING_A:
        if(gprsRxBuffer[0] == "a"){
            RS485_Out(">>a", 3);
            // gprs_state = GPRS_REPLY_A;
        }
        break;

    default:
        break;
    }
}