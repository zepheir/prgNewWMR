#ifndef _PARA_H
#define _PARA_H

#include "main.h"

#define CH1 0
#define CH2 1
#define CH3 2
#define CH4 3

// #define FLASH_PAGE_SIZE        ((uint32_t)0x00000400)  // FLASH_PAGE_SIZE is defined in "stm32f0xx_hal_flash_ex.h"
#define FLASH_USER_START_ADDR  ((uint32_t)0x0800F800)  
#define FLASH_USER_END_ADDR    ((uint32_t)0x08010000) 

#define FILTER_MAX 8


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
    uint32_t ch[4];
    _PARA data;
} PARA;

// filter 不需要保存
typedef struct
{
    uint8_t ch1;
    uint8_t ch2;
    uint8_t ch3;
    uint8_t ch4;
} _FILTER;

typedef union 
{
    uint8_t ch[4];
    _FILTER data;
} FILTER;

typedef enum {
    READY = 0,
    TRIGGED,
    FILTERING,
    TIMEOUT
} CH_STATE;


//
typedef enum {
    SYS_MODE_NORMAL,
    SYS_MODE_FACTORY_LOAD,
    SYS_MODE_DEBUG
} SYS_MODE;




// 参数初始化
void Para_Init(void);

// 恢复出厂设置
void Para_Factory(void);

#endif