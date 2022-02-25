#include "func.h"
#include "main.h"
#include "para.h"


//
extern PARA para;
extern FILTER filter;

/**
 * @brief 这个函数在main()里面的100ms循环里面,
 *          所以这个函数会每100ms执行一次
 * 
 */
void Update_State(void){
    ;
}

/**
 * @brief 这个函数会在main()里面的1s循环里面, 
 *          所以这个程序会每秒钟执行一次
 * 
 */
void Run(void){
    HAL_GPIO_TogglePin(LED_STAT_GPIO_Port, LED_STAT_Pin);

}

void UserErase(void)
{
    // HAL_StatusTypeDef halRet;
    FLASH_EraseInitTypeDef flashEraseInitType;
    uint32_t flashEraseRet;

    flashEraseInitType.NbPages = 1;
    flashEraseInitType.TypeErase = FLASH_TYPEERASE_PAGES;
    flashEraseInitType.PageAddress = FLASH_USER_START_ADDR;

    // halRet = HAL_FLASHEx_Erase(&flashEraseInitType, &flashEraseRet);
    HAL_FLASHEx_Erase(&flashEraseInitType, &flashEraseRet);

    // if(halRet !=HAL_OK || flashEraseRet != 0xffffffffU){
    //     return HAL_ERROR;
    // }

    // return HAL_OK;
}


void UserRead(void)
{
    int i;
    uint32_t _addr;

    _addr = FLASH_USER_START_ADDR;

    for ( i = 0; i < 4; i++)
    {
        para.dataAll[i] = *(__IO uint32_t *)_addr;
        _addr+=4;
    }
    
}

void UserWrite(void)
{
    uint32_t _addr;
    _addr = FLASH_USER_START_ADDR;

    HAL_FLASH_Unlock();

    UserErase();

    for ( int i = 0; i < 4; i++)
    {
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, _addr, para.dataAll[i]);
        _addr+=4;
    }
    

    HAL_FLASH_Lock();
}


