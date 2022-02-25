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


