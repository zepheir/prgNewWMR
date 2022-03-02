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

GPRS_7G3 gprs_7g3;
GPRS_STATE gprs_state;

GPRS_RX_STATE gprs_rx_state;


void gprs_Clear_Rx_Buff(void);

void gprs_Clear_Rx_Buff(void)
{
    gprs_rx_cnt = 0;
    memset(gprsRxBuffer, 0x00, sizeof(gprsRxBuffer));
}


void gprs_Send(char *pStr){

    char _buff[128];
    uint8_t len;

    len = strlen(pStr);
    memset(_buff, 0x00, sizeof(_buff));
    memcpy(_buff,pStr, len);
    _buff[len] = '\r';

    HAL_UART_Transmit(&h_gprs, (uint8_t *)_buff, len+1, 0xffff);
    while (HAL_USART_GetState(&h_gprs) == HAL_UART_STATE_BUSY_TX);

}



// void gprs_Send(uint8_t *pStr, uint8_t size){

//     char _buff[64];
//     uint8_t _len;

//     // memcpy(_buff, pStr, size);
//     // memcpy(_buff+size, "\xd", 1);
//     memset(_buff, 0x00, sizeof(_buff));
//     memcpy(_buff,(char *)pStr, strlen(pStr));

//     _len=strlen(_buff);
//     // HAL_UART_Transmit(&h_gprs, _buff, size+1, 0xffff);
//     HAL_UART_Transmit(&h_gprs, (uint8_t *)_buff, _len, 0xffff);
//     while (HAL_USART_GetState(&h_gprs) == HAL_UART_STATE_BUSY_TX);

// }


void gprsReceiver(void)
{
    // 如果溢出了
    if (gprs_rx_cnt >= 255)
    {
        gprs_rx_cnt = 0;
        memset(gprsRxBuffer, 0x00, sizeof(gprsRxBuffer));
        //   HAL_UART_Transmit(&h_rs485, "GPRS RX over buff size", 23, 0xffff);
        RS485_Out("GPRS RX over buff size");
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
            RS485_Out((char *)&gprsRxBuffer);

            memset(gprsBuffer, 0x00, sizeof(gprsBuffer));
            memcpy(gprsBuffer, gprsRxBuffer, gprs_rx_cnt);

            gprs_rx_cnt = 0;
            memset(gprsRxBuffer, 0x00, sizeof(gprsRxBuffer));

            gprs_rx_state = GPRS_RX_READY;
        }
    }
}



void gprs_Enter_Setting(void){
    // uint8_t *pStr;

    switch (gprs_state)
    {
    case GPRS_READY:
        RS485_Out(">>+++");
        HAL_UART_Transmit(&h_gprs, (uint8_t *)"+++", 3, 0xffff);
        gprs_state = GPRS_WAITING_A;
        break;
    case GPRS_WAITING_A:
        if(gprsBuffer[0] == 'a'){
            RS485_Out((char *)gprsBuffer); 
            gprs_Clear_Rx_Buff();
            gprs_state = GPRS_REPLY_A;
        }
        break;
    case GPRS_REPLY_A:
        HAL_UART_Transmit(&h_gprs, (uint8_t *)"a", 1, 0xffff);
        gprs_state = GPRS_WAITING_A_OK;
        break;

    case GPRS_WAITING_A_OK:
        
        if(strstr((char *)gprsBuffer, "+ok")){
        // if(strncmp(gprsBuffer, "+ok", 3) == 0){
            RS485_Out((char *)gprsBuffer);
            gprs_Clear_Rx_Buff();
            gprs_state = GRPS_AT_MODE_READY;
            RS485_Out(">> Enter AT COMMAND MODE!");
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
        HAL_UART_Transmit(&h_gprs, (uint8_t *)"at+entm\xd", 9, 0xffff);
        gprs_state = GPRS_WAITING_AT_ENTM;
        break;

    case GPRS_WAITING_AT_ENTM:
        if(strncmp((char *)gprsBuffer, "at+entm\r\000\r\nOK\r\n", 15) == 0){
            RS485_Out(">> Exit AT COMMAND MODE!");
            gprs_state = GPRS_READY;
        }
        break;

    default:
        break;
    }
}


void gprs_Ini(void){
    char *pStr;
    // char _buff[64];

    switch (gprs_state)
    {
    case GPRS_INI:
        RS485_Out(">> GPRS INITIAL ...\r");
        gprs_state = GPRS_READ_IMEI;
        // gprs_Get_IMEI();
        RS485_Out(">> Read IMEI ...\r");
        gprs_Send("usr.cn#at+imei\r");
        gprs_state = GPRS_READ_IMEI_WAIT;
        break;

    case GPRS_READ_IMEI_WAIT:
        pStr = strstr((char *)gprsBuffer, "+IMEI:");
        if(pStr){
            RS485_Out(">> IMEI:");
            strcpy(gprs_7g3.imei, pStr);
            RS485_Out(gprs_7g3.imei);
            gprs_state = GPRS_GET_SERVER;
        }
        break;
    
    case GPRS_GET_SERVER:
        // memset(_buff, 0x00, sizeof(_buff));
        // strcpy(_buff, "usr.cn#at+socka?");

        RS485_Out("usr.cn#at+socka?");
        gprs_Send("usr.cn#at+socka?");
        gprs_state = GPRS_GET_SERVER_WAIT;
        break;
    
    case GPRS_GET_SERVER_WAIT:
        pStr = strstr((char *)gprsBuffer, "+SOCKA:");
        if(pStr){
            RS485_Out(">> SERVER:");
            RS485_Out(pStr);
            gprs_state = GPRS_READY;
        }
        break;
    
    default:
        break;
    }
}


void gprs_Remote_Req(void){
    switch (gprs_state)
    {
    case GPRS_READY:
        RS485_Out(">> GPRS Remote Start...");
        gprs_state = GPRS_REMOTE_REQ;
        break;

    case GPRS_REMOTE_REQ:
        RS485_Out(">> GPRS REMOTE SEND:");
        gprs_state = GPRS_REMOTE_REQ_WAIT;
        break;
    
    case GPRS_REMOTE_REQ_WAIT:
        RS485_Out(">> GPRS REMOTE REPLY:");
        gprs_state = GPRS_READY;
        break;
    
    default:
        break;
    }
}