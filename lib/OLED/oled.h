#ifndef __OLED_H__
#define __OLED_H__
#ifdef __cplusplus
extern "C"
{
#endif

// includes
#include "main.h"
#include "u8g2.h"

// private defines
#define u8  unsigned char
#define MAX_LEN 128
#define OLED_ADDRESS  0X78  // oled模块从机地址
#define OLED_CMD      0X00  // 写命令
#define OLED_DATA     0X40  // 写数据

uint8_t u8x8_byte_hw_i2c(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);
uint8_t u8x8_gpio_and_delay(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);
void u8g2Init(u8g2_t *u8g2);

#ifdef __cplusplus
}
#endif

#endif // __OLED_H__