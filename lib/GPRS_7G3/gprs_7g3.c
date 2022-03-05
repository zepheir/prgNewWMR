#include "gprs_7g3.h"
#include "rs485.h"
// #include "main.h"
#include "func.h"
#include "para.h"
#include "usart.h"
#include "string.h"
#include <stdio.h>
#include <stdlib.h>


uint8_t gprs_rx_cnt = 0;
extern uint8_t bRxBuffer;
uint8_t gprs_resp_timer;
uint8_t gprsRxBuffer[256];
uint8_t gprsBuffer[128];
extern PARA para;
// extern SYS_MODE sys_mode;

GPRS_7G3 gprs_7g3;
GPRS_STATE gprs_state;
GPRS_RX_STATE gprs_rx_state;
static uint8_t gprs_remote_timer = GPRS_REMOTE_TIMEOUT_MAX;

void gprs_Clear_Rx_Buff(void);
void gprs_Module_Timeout(void);

void gprs_Clear_Rx_Buff(void)
{
    gprs_rx_cnt = 0;
    memset(gprsRxBuffer, 0x00, sizeof(gprsRxBuffer));
}


void gprs_Send(char *pStr){

    char _buff[256];
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

            memset(gprsBuffer, 0x00, sizeof(gprsBuffer));
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

            RS485_Out(">> Enter AT COMMAND MODE!");
            memset(gprsBuffer, 0x00, sizeof(gprsBuffer));
            gprs_state = GRPS_AT_MODE_READY;
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
            memset(gprsBuffer, 0x00, sizeof(gprsBuffer));
            gprs_state = GPRS_READY;
        }
        break;

    default:
        break;
    }
}


void gprs_Ini(void){
    char *pStr;
    char _buff[3];

    switch (gprs_state)
    {
    case GPRS_INI:
        RS485_Out(">> GPRS INITIAL ...\r");
        if (HAL_GPIO_ReadPin(GPRS_NET_GPIO_Port, GPRS_NET_Pin) == GPIO_PIN_RESET)
        {
            gprs_state = GPRS_READ_IMEI;
        }
        break;
    
    case GPRS_READ_IMEI:
        // gprs_Get_IMEI();
        RS485_Out(">> Read IMEI ...\r");
        
        memset(gprsBuffer, 0x00, sizeof(gprsBuffer));
        gprs_Send("usr.cn#at+imei\r");
        gprs_state = GPRS_READ_IMEI_WAIT;
        break;

    case GPRS_READ_IMEI_WAIT:
        pStr = strstr((char *)gprsBuffer, "+IMEI:");
        if(pStr){
            gprs_remote_timer = GPRS_REMOTE_TIMEOUT_MAX;
            RS485_Out(">> IMEI:");
            memcpy(gprs_7g3.imei, pStr+6, 15);
            RS485_Out(gprs_7g3.imei);
            memset(gprsBuffer, 0x00, sizeof(gprsBuffer));
            gprs_state = GPRS_GET_CSQ;
        }
        else{
            gprs_Module_Timeout();
        }
        break;

    case GPRS_GET_CSQ:
        RS485_Out(">> GPRS Get CSQ ...");
        gprs_Send("usr.cn#at+csq\r");
        gprs_state = GPRS_GET_CSQ_WAIT;
        break;

    case GPRS_GET_CSQ_WAIT:
        pStr = strstr((char *)gprsBuffer, "+CSQ:");
        if(pStr){
            
            gprs_remote_timer = GPRS_REMOTE_TIMEOUT_MAX;
            memcpy(_buff, pStr+5,3);
            gprs_7g3.csq = atoi(_buff);

            memset(gprsBuffer, 0x00, sizeof(gprsBuffer));
            gprs_state = GPRS_GET_SERVER;
        }else{
            gprs_Module_Timeout();
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
            gprs_remote_timer = GPRS_REMOTE_TIMEOUT_MAX;
            RS485_Out(">> SERVER:");
            RS485_Out(pStr);

            memset(gprsBuffer, 0x00, sizeof(gprsBuffer));

            RS485_Out(">> INI OK!");
            gprs_state = GPRS_READY;
        }else{
            gprs_Module_Timeout();
        }
        break;
    
    default:
        break;
    }
}


void gprs_Remote_Req(void){
    // char *pStr;
    char _buff[256];
    uint32_t _data;

    switch (gprs_state)
    {
    case GPRS_READY:
        RS485_Out(">> GPRS Remote Start...");
        gprs_state = GPRS_REMOTE_REQ;
        break;

    case GPRS_REMOTE_REQ:

        memset(_buff, 0x00, sizeof(_buff));
        sprintf(_buff, "{\"TYPE\":\"REQ\", \"IMEI\":\"%s\"}", gprs_7g3.imei);
        gprs_Send(_buff);

        RS485_Out(">> GPRS REMOTE SEND:");
        RS485_Out(_buff);

        gprs_state = GPRS_REMOTE_REQ_WAIT;
        break;
    
    case GPRS_REMOTE_REQ_WAIT:

        if(strstr((char *)gprsBuffer, "READALL")){
            RS485_Out(">> GPRS REMOTE REPLY: READALL");
            gprs_remote_timer = GPRS_REMOTE_TIMEOUT_MAX;

            memset(_buff, 0x00, sizeof(_buff));
            sprintf(_buff, 
                "{ "
                    "\"TYPE\":\"DATA\","
                    "\"IMEI\":\"%s\","
                    "\"D0\":%lu,"
                    "\"D1\":%lu,"
                    "\"D2\":%lu,"
                    "\"D3\":%lu}",
                    gprs_7g3.imei, 
                    para.ch[CH1],
                    para.ch[CH2],
                    para.ch[CH3],
                    para.ch[CH4]
                    );
            RS485_Out(_buff);
            // gprs_Send("{\"TYPE\":\"DATA\", \"IMEI\":\"865374057342316\",\"D0\":10,\"D1\":10,\"D2\":10,\"D3\":10}");
            gprs_Send(_buff);

            memset(gprsBuffer, 0x00, sizeof(gprsBuffer));
            gprs_state = GPRS_REMOTE_SENDDATA_WAIT;
        }
        else if(strstr((char *)gprsBuffer, "SETDATA")){
            RS485_Out(">> GPRS REMOTE REPLY: SETDATA");
            gprs_remote_timer = GPRS_REMOTE_TIMEOUT_MAX;

            if(gprsBuffer[8] & 0x1){
                memcpy(&_data, gprsBuffer+9, sizeof(_data));
                para.ch[CH1] = _data;
            }
            if(gprsBuffer[8] & 0x2){
                memcpy(&_data, gprsBuffer+13, sizeof(_data));
                para.ch[CH2] = _data;
            }
            if(gprsBuffer[8] & 0x4){
                memcpy(&_data, gprsBuffer+17, sizeof(_data));
                para.ch[CH3] = _data;
            }
            if(gprsBuffer[8] & 0x8){
                memcpy(&_data, gprsBuffer+21, sizeof(_data));
                para.ch[CH4] = _data;
            }
            
            memset(_buff, 0x00, sizeof(_buff));
            sprintf(_buff, "D0:%lu, D1:%lu, D2:%lu, D3:%lu",
                    para.ch[CH1],
                    para.ch[CH2],
                    para.ch[CH3],
                    para.ch[CH4]);

            RS485_Out(_buff);
            
            memset(gprsBuffer, 0x00, sizeof(gprsBuffer));
            gprs_state = GPRS_READY;
        }
        else{

            gprs_Module_Timeout();
        }
            
        
        break;

    case GPRS_REMOTE_SENDDATA_WAIT:
        if (strstr((char *)gprsBuffer, "END"))
        {
            RS485_Out(">> GPRS REMOTE REPLY: END");
            gprs_remote_timer = GPRS_REMOTE_TIMEOUT_MAX;

            memset(gprsBuffer, 0x00, sizeof(gprsBuffer));
            gprs_state = GPRS_READY;
        }
        else{
            gprs_Module_Timeout();
        }

        break;
    
    default:
        break;
    }
}

void gprs_Module_Timeout(void)
{
    char *buff[64];

    if (gprs_remote_timer > 0)
    {
        gprs_remote_timer--;
        sprintf(buff, ">> GPRS MODULE/REMOTE REQ WAITING ...(%d)", gprs_remote_timer);
        RS485_Out(buff);
    }
    else
    {
        
        RS485_Out(">> GPRS MODUEL/REMOTE REQ TIMEOUT!!! 7S3 Moduel Reboot!!!");
        gprs_remote_timer = GPRS_REMOTE_TIMEOUT_MAX;
        memset(gprsBuffer, 0x00, sizeof(gprsBuffer));

        // reset gprs_7s3 module
        gprs_Send("usr.cn#at+z\r");
        
        gprs_state = GPRS_READY;
    }
}