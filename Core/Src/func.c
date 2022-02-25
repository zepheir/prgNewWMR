#include "func.h"
#include "main.h"
#include "para.h"


//
extern PARA para;
extern FILTER filter;
extern CH_STATE ch_state[4];
extern CH_STATE pwr_check_state;
extern uint8_t pwr_check_filter;
//


// private functions define
void DI_Filter(void);

static uint16_t flash_addr_offset;

// private functions define
uint8_t get_flash_offset_address(void);


/**
 * @brief 这个函数在main()里面的100ms循环里面,
 *          所以这个函数会每100ms执行一次
 * 
 */
void Update_State(void){
    if(pwr_check_state == TRIGGED){
        pwr_check_state = FILTERING;
    }
    if (ch_state[CH1] == TRIGGED)
    {
        para.ch[CH1]++;
        ch_state[CH1] = FILTERING;
    }
    if (ch_state[CH2] == TRIGGED)
    {
        para.ch[CH2]++;
        ch_state[CH2] = FILTERING;
    }
    if (ch_state[CH3] == TRIGGED)
    {
        para.ch[CH3]++;
        ch_state[CH3] = FILTERING;
    }
    if (ch_state[CH4] == TRIGGED)
    {
        para.ch[CH4]++;
        ch_state[CH4] = FILTERING;
    }

    DI_Filter();
}

/**
 * @brief 这个函数会在main()里面的1s循环里面, 
 *          所以这个程序会每秒钟执行一次
 * 
 */
void Run(void){
    HAL_GPIO_TogglePin(LED_STAT_GPIO_Port, LED_STAT_Pin);

}

void DI_Filter(void){

    for (uint8_t i = 0; i < 4; i++)
    {
        if (ch_state[i] == FILTERING){

            if(filter.ch[i] > 0){
                filter.ch[i]--;
            }
            else{
                filter.ch[i] = FILTER_MAX;
                ch_state[i] = READY;
            }
        }
    }

    if(pwr_check_state == FILTERING){
        if(pwr_check_filter > 0){
            pwr_check_filter--;
        }else{
            pwr_check_filter=FILTER_MAX;
            pwr_check_state = READY;
        }
    }
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

uint8_t get_flash_offset_address(void){
    uint32_t _addr = FLASH_USER_START_ADDR;
    uint16_t i;

    // 总page的大小为为1024, 每32个作为一个单元, 一共为32个单元

    for ( i = 0; i < 32; i++)
    {
        if (*(__IO uint32_t *)(_addr + i * 32U + 0x1c) == 0xffffffff)
        {
            break;
        }
    }

    // if (i == 0)
    // {
    //     return 31;
    // }else{
    //     return i-1;
    // }
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

    if (flash_addr_offset < 31)
    {
        flash_addr_offset++;
        _addr += flash_addr_offset * 32U;
        HAL_FLASH_Unlock();

        for (int i = 0; i < 4; i++)
        {
            HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, _addr, para.dataAll[i]);
            _addr += 4;
        }

        _addr+=12;
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, _addr, 0x77777777);

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

        _addr+=12;
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, _addr, 0x77777777);

        HAL_FLASH_Lock();
    }

}


