#ifndef _PARA_H
#define _PARA_H

#include "main.h"

// #define FLASH_PAGE_SIZE        ((uint32_t)0x00000400)  // FLASH_PAGE_SIZE is defined in "stm32f0xx_hal_flash_ex.h"
#define FLASH_USER_START_ADDR  ((uint32_t)0x0800F800)  
#define FLASH_USER_END_ADDR    ((uint32_t)0x08010000) 

//定义需保存数据的结构
typedef struct 
{
    uint32_t ch1;
    uint32_t ch2;
    uint32_t ch3;
    uint32_t ch4;
} _PARA;

typedef union 
{
    uint32_t dataAll[4];
    _PARA data;
} PARA;

// filter 不需要保存
typedef struct
{
    uint8_t ch1;
    uint8_t ch2;
    uint8_t ch3;
    uint8_t ch4;
} FILTER;



//
void EEPROM_Write_Words(uint16_t addr, uint8_t *buffer, uint8_t len);
void EEPROM_Read_Bytes(uint16_t addr, uint8_t *buffer, uint16_t len);
//
void EEPROM_Save_Word(uint16_t addr, uint16_t data);

uint8_t Para_Save_Byte(uint16_t addr, uint8_t data);
uint8_t Para_Save_Int(uint16_t addr, uint8_t data);
uint8_t Para_Save_Float(uint16_t addr, float data);

// 参数初始化
void Para_Init(void);

// 恢复出厂设置
void Para_Factory(void);

#endif