#include "rs485.h"
#include "gprs_7gx.h"
#include "func.h"
#include "para.h"
#include "usart.h"
#include "string.h"

uint8_t rs485_rx_cnt = 0;
extern uint8_t aRxBuffer;
uint8_t RxBuffer[256];

RS485_STATE rs485_state;
uint8_t rs485_resp_timer = RS485_RESP_TIME_MAX;

extern GPRS_STATE gprs_state;

// extern SYS_MODE sys_mode;

void RS485_Clear_Rx_Buff(void);

void RS485_Clear_Rx_Buff(void){
  rs485_rx_cnt = 0U;
  memset(RxBuffer, 0x00, sizeof(RxBuffer));
}

void RS485_Out(char *pStr){

    uint8_t _buff[256];
    uint8_t len;

    len = strlen(pStr);
    memset(_buff, 0x00, sizeof(_buff));
    memcpy(_buff, pStr, len);
    _buff[len] = '\r';

    HAL_UART_Transmit(&h_rs485, _buff, len+1, 0xffff);
    while (HAL_USART_GetState(&h_rs485) == HAL_UART_STATE_BUSY_TX);

}

void RS485_Receiver(void){
    // 如果溢出了
    if(rs485_rx_cnt >= 255){
      // rs485_rx_cnt = 0;
      // memset(RxBuffer, 0x00, sizeof(RxBuffer));
      RS485_Clear_Rx_Buff();

      RS485_Out("RS485 RX over buff size");
    //   HAL_UART_Transmit(&h_rs485, _buff, sizeof(_buff), 0xffff);
    }
    else{
      RxBuffer[rs485_rx_cnt++] = aRxBuffer; // 接受数据转存

      // // 在Normal模式下
      // if (sys_mode == SYS_MODE_NORMAL)
      // {
      //   if ((RxBuffer[rs485_rx_cnt - 1] == 0x0A) && (RxBuffer[rs485_rx_cnt - 2] == 0x0d))
      //   {

      //     RS485_Out((uint8_t *)&RxBuffer, rs485_rx_cnt);

      //     RS485_Clear_Rx_Buff();

      //   }
      // }
      // else if (sys_mode == SYS_MODE_DEBUG)
      // {
      //   rs485_state = RS485_WAITING_RESP;
      // }
      rs485_state = RS485_WAITING_RESP;
      
    }
}

void RS485_Receiver_TimeoutMode(void)
{
    if (rs485_state == RS485_WAITING_RESP)
    {
        if (rs485_resp_timer > 0)
        {
            rs485_resp_timer--;
        }
        else
        {
            rs485_resp_timer = RS485_RESP_TIME_MAX;
            rs485_state = RS485_TIMEOUT;

            RS485_Out((char *)&RxBuffer);

            // if (gprs_state == GRPS_AT_MODE_READY)
            // {
            //   gprs_Send((uint8_t *)&RxBuffer, rs485_rx_cnt);
            // }

            // gprs_Send((uint8_t *)&RxBuffer, rs485_rx_cnt);
            gprs_Send((char *)&RxBuffer );

            RS485_Clear_Rx_Buff();

            rs485_state = RS485_READY;
        }
    }
}


