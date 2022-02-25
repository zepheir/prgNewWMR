#include "func.h"
#include "main.h"
#include "para.h"


//
extern PARA para;
extern FILTER filter;

static uint16_t flash_addr_offset;

// private functions define
uint8_t get_flash_offset_address(void);


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

/**
 * @brief 擦除参数保存页
 * 
 */
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

uint8_t get_flash_offset_address(void){
    uint32_t _addr = FLASH_USER_START_ADDR;
    uint16_t i;

    // 总page的大小为为1024, 每32个作为一个单元, 一共为32个单元

    for ( i = 0; i < 32; i++)
    {
        if (*(__IO uint32_t *)(_addr + i * 32U) == 0xffffffff)
        {
            break;
        }
    }
    
    return i-1;

}

/**
 * @brief 读取参数
 * 
 */
void UserRead(void)
{
    int i;
    uint32_t _addr;

    uint8_t _offset = get_flash_offset_address();
    flash_addr_offset = _offset;

    _addr = FLASH_USER_START_ADDR + _offset * 32U;

    for ( i = 0; i < 4; i++)
    {
        para.dataAll[i] = *(__IO uint32_t *)_addr;
        _addr+=4;
    }
    
}

/**
 * @brief 写入数据
 * 
 */
void UserWrite(void)
{
    uint32_t _addr = FLASH_USER_START_ADDR;

    if (flash_addr_offset < 127)
    {
        flash_addr_offset++;
        _addr += flash_addr_offset * 32U;
        HAL_FLASH_Unlock();

        for (int i = 0; i < 4; i++)
        {
            HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, _addr, para.dataAll[i]);
            _addr += 4;
        }

        HAL_FLASH_Lock();
    }
    else {
        flash_addr_offset = 0;

        HAL_FLASH_Unlock();

        UserErase();

        for (int i = 0; i < 4; i++)
        {
            HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, _addr, para.dataAll[i]);
            _addr += 4;
        } 

        HAL_FLASH_Lock();
    }

}


