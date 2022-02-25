#include "para.h"
#include "func.h"

// 参数定义
#define FILTER_MAX 10

extern PARA para;
extern FILTER filter;

void Para_Init(void)
{
    
    filter.ch1 = FILTER_MAX;
    filter.ch2 = FILTER_MAX;
    filter.ch3 = FILTER_MAX;
    filter.ch4 = FILTER_MAX;
    
    UserRead();

    para.data.ch1 = 1234;
    para.data.ch2 = 5678;
    para.data.ch3 = 3456;
    para.data.ch4 = 7890;

    UserErase();
    
    UserWrite();
}


