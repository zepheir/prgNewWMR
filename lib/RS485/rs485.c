#include "rs485.h"
#include "func.h"
#include "para.h"
#include "usart.h"
#include "string.h"

uint8_t rs485_rx_cnt = 0;
extern uint8_t aRxBuffer;
uint8_t RxBuffer[256];

RS485_STATE rs485_state;
uint8_t rs485_resp_timer = RS485_RESP_TIME_MAX;

extern SYS_MODE sys_mode;

void RS485_Receiver(void){
    // 如果溢出了
    if(rs485_rx_cnt >= 255){
      rs485_rx_cnt = 0;
      memset(RxBuffer, 0x00, sizeof(RxBuffer));
      uint8_t _buff[] = "RS485 RX over buff size";
      RS485_Out(_buff, sizeof(_buff));
    //   HAL_UART_Transmit(&h_rs485, _buff, sizeof(_buff), 0xffff);
    }
    else{
      RxBuffer[rs485_rx_cnt++] = aRxBuffer; // 接受数据转存

      // 在Normal模式下
      if (sys_mode == SYS_MODE_NORMAL)
      {
        if ((RxBuffer[rs485_rx_cnt - 1] == 0x0A) && (RxBuffer[rs485_rx_cnt - 2] == 0x0d))
        {
          HAL_UART_Transmit(&h_rs485, (uint8_t *)&RxBuffer, rs485_rx_cnt, 0xffff);
          while (HAL_USART_GetState(&h_rs485) == HAL_UART_STATE_BUSY_TX)
            ;
          rs485_rx_cnt = 0;
          memset(RxBuffer, 0x00, sizeof(RxBuffer));
        }
      }
      else if (sys_mode == SYS_MODE_DEBUG)
      {
        rs485_state = RS485_WAITING_RESP;
      }
      
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

            // HAL_UART_Transmit(&huart1, (uint8_t *)&RxBuffer, rs485_rx_cnt, 0xffff);
            // while (HAL_USART_GetState(&h_rs485) == HAL_UART_STATE_BUSY_TX);
            RS485_Out(&RxBuffer, rs485_rx_cnt);

            rs485_rx_cnt = 0;
            memset(RxBuffer, 0x00, sizeof(RxBuffer));

            rs485_state = RS485_READY;
        }
    }
}


void RS485_Out(uint8_t str[], uint8_t size){

    uint8_t _buff[size+1];

    memcpy(_buff, str, size);
    memcpy(_buff+size, "\xd", 1);

    HAL_UART_Transmit(&h_rs485, _buff, size+1, 0xffff);
    while (HAL_USART_GetState(&h_rs485) == HAL_UART_STATE_BUSY_TX);

    // HAL_UART_Transmit(&h_rs485, "\x0d\x0a", 7, 0xffff);
    // while (HAL_USART_GetState(&h_rs485) == HAL_UART_STATE_BUSY_TX);
}