#include "para.h"
#include "func.h"

// 参数定义


extern PARA para;
extern FILTER filter;
extern uint8_t pwr_check_filter;

void Para_Init(void)
{
    
    filter.data.ch1 = FILTER_MAX;
    filter.data.ch2 = FILTER_MAX;
    filter.data.ch3 = FILTER_MAX;
    filter.data.ch4 = FILTER_MAX;

    pwr_check_filter = FILTER_MAX;
    
    UserRead();

    para.data.ch1 = 1234;
    para.data.ch2 = 5678;
    para.data.ch3 = 3456;
    para.data.ch4 = 7890;

    UserErase();
    
    UserWrite();
}


