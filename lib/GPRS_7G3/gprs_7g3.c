#include "gprs_7g3.h"
#include "rs485.h"
// #include "main.h"
#include "func.h"
#include "para.h"
#include "usart.h"
#include "string.h"


uint8_t gprs_rx_cnt = 0;
extern uint8_t bRxBuffer;
uint8_t gprs_resp_timer;
uint8_t gprsRxBuffer[256];
uint8_t gprsBuffer[128];
extern SYS_MODE sys_mode;


GPRS_STATE gprs_state;

GPRS_RX_STATE gprs_rx_state;


void gprs_Clear_Rx_Buff(void);

void gprs_Clear_Rx_Buff(void)
{
    gprs_rx_cnt = 0;
    memset(gprsRxBuffer, 0x00, sizeof(gprsRxBuffer));
}

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

            memset(gprsBuffer, 0x00, sizeof(gprsBuffer));
            memcpy(gprsBuffer, gprsRxBuffer, gprs_rx_cnt);

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
        RS485_Out(">>+++", 6);
        HAL_UART_Transmit(&h_gprs, "+++", 3, 0xffff);
        gprs_state = GPRS_WAITING_A;
        break;
    case GPRS_WAITING_A:
        if(gprsBuffer[0] == 'a'){
            RS485_Out(gprsBuffer, gprs_rx_cnt); 
            gprs_Clear_Rx_Buff();
            gprs_state = GPRS_REPLY_A;
        }
        break;
    case GPRS_REPLY_A:
        HAL_UART_Transmit(&h_gprs, "a", 1, 0xffff);
        gprs_state = GPRS_WAITING_A_OK;
        break;

    case GPRS_WAITING_A_OK:
        if(strncmp(gprsBuffer, "+ok", 3) == 0){
            RS485_Out(gprsBuffer, gprs_rx_cnt);
            gprs_Clear_Rx_Buff();
            gprs_state = GRPS_AT_MODE_READY;
            RS485_Out(">> Enter AT COMMAND MODE!", 26);
        }
        break;

    case GRPS_AT_MODE_READY:
        
        break;


    default:
        break;
    }
}

void gprs_Exit_At_Mode(void){

    switch (gprs_state)
    {

    case GPRS_SEND_AT_ENTM:
        gprs_Clear_Rx_Buff();
        HAL_UART_Transmit(&h_gprs, "at+entm\xd", 9, 0xffff);
        gprs_state = GPRS_WAITING_AT_ENTM;
        break;

    case GPRS_WAITING_AT_ENTM:
        if(strncmp(gprsBuffer, "at+entm\r\000\r\nOK\r\n", 15) == 0){
            RS485_Out(">> Exit AT COMMAND MODE!",  25);
            gprs_state = GPRS_READY;
        }
        break;

    default:
        break;
    }
}

void gprs_Send(uint8_t *pStr, uint8_t size){

    uint8_t _buff[size+1];

    memcpy(_buff, pStr, size);
    memcpy(_buff+size, "\xd", 1);

    HAL_UART_Transmit(&h_gprs, _buff, size+1, 0xffff);
    while (HAL_USART_GetState(&h_gprs) == HAL_UART_STATE_BUSY_TX);

}