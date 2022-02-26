#include "para.h"
#include "func.h"

// 参数定义


extern PARA para;
extern FILTER filter;
extern uint8_t pwr_check_filter;
extern SYS_MODE sys_mode;

void Para_Init(void)
{
    uint32_t _addr = FLASH_USER_START_ADDR;

    filter.data.ch1 = FILTER_MAX;
    filter.data.ch2 = FILTER_MAX;
    filter.data.ch3 = FILTER_MAX;
    filter.data.ch4 = FILTER_MAX;
    pwr_check_filter = FILTER_MAX;

    // HAL_FLASH_Unlock();
    // UserErase();
    // HAL_FLASH_Lock();

    if (sys_mode == SYS_MODE_FACTORY_LOAD || *(__IO uint32_t *)(_addr + 0x1c) == 0xffffffff)
    {
        Para_Factory();
    }
    

    UserRead();

    // para.data.ch1 = 0x1234;
    // para.data.ch2 = 0x5678;
    // para.data.ch3 = 0x3456;
    // para.data.ch4 = 0x7890;
    
}



void Para_Factory(void)
{
    uint32_t _addr = FLASH_USER_START_ADDR;

    for (uint8_t i = 0; i < 4; i++)
    {
        para.dataAll[i] = 0;
    }

    HAL_FLASH_Unlock();

    UserErase();

    for (int i = 0; i < 4; i++)
    {
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, _addr, para.dataAll[i]);
        _addr += 4;
    } 

    _addr+=12;
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, _addr, 0x77777777);

    HAL_FLASH_Lock();
}
