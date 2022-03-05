#include "func.h"
#include "main.h"
#include "usart.h"
#include "rs485.h"
#include "gprs_7g3.h"
#include "oled.h"
#include "u8g2.h"
#include <stdio.h>
#include <stdlib.h>

typedef enum{
    HIGH,
    FALLED,
    LOW,
    RAISED
} GPIO_STATE;

static uint16_t remote_req_timer = REMOTE_REQ_TIMER_MAX;

static GPIO_STATE SW1_state = HIGH;
static uint8_t led_count=10;
extern SYS_MODE sys_mode;

//
extern PARA para;
extern FILTER filter;
extern CH_STATE ch_state[4];
extern CH_STATE pwr_check_state;
extern uint8_t pwr_check_filter;

extern uint8_t rs485_resp_timer;
extern RS485_STATE rs485_state;
extern uint8_t rs485_rx_cnt;
extern uint8_t aRxBuffer;
extern uint8_t RxBuffer[256];

extern GPRS_STATE gprs_state;

u8g2_t u8g2;

//


// private functions define
void SW1_State_Change(void);
void DI_Filter(void);
void Led_Blink_Normal_Mode(void);
void Led_Blink_Debug_Mode(void);
void Led_Blink_REQ_Mode(void);
void Led_Blink_AT_Mode(void);

static uint16_t flash_addr_offset;

// private functions define
uint8_t get_flash_offset_address(void);


void Init(void){
    
    Para_Init();

    // u8g2Init(&u8g2);
}

/**
 * @brief 这个函数在main()里面的100ms循环里面,
 *          所以这个函数会每100ms执行一次
 * 
 */
void Update_State(void){

    if(sys_mode != SYS_MODE_REMOTE && sys_mode != SYS_MODE_INI){
        sys_mode = SystemModeSelect();
    }
    

    SW1_State_Change();

    if (SW1_state == RAISED)
    {
        // HAL_UART_Transmit(&huart1, "SW1_RAISED\r", 11, 0xffff);
        RS485_Out("SW1_RAISED\r");
        //
        if(sys_mode == SYS_MODE_DEBUG){
            gprs_state = GPRS_SEND_AT_ENTM;
        }

    }
    else if (SW1_state == FALLED)
    {
        // HAL_UART_Transmit(&huart1, "SW1_FALLED\r", 11, 0xffff);
        RS485_Out("SW1_FALLED\r");
        
        if(sys_mode == SYS_MODE_DEBUG){
            gprs_state = GPRS_READY;
        }else if(sys_mode == SYS_MODE_NORMAL){
            sys_mode = SYS_MODE_REMOTE;
        }
        
    }

    DI_Filter();


    // 
    switch (sys_mode)
    {

    case SYS_MODE_INI:
        Led_Blink_REQ_Mode();
        break;
        
    case  SYS_MODE_NORMAL:
        Led_Blink_Normal_Mode();



        break;
    case SYS_MODE_DEBUG:
        if(gprs_state == GRPS_AT_MODE_READY){
            Led_Blink_AT_Mode();
        }else{
            Led_Blink_Debug_Mode();
        }

        break;

    case SYS_MODE_REMOTE:
        Led_Blink_REQ_Mode();
        break;

    default:
        break;
    }

    // rs485 timeout mode
    RS485_Receiver_TimeoutMode();

    gprs_Receiver_TimeoutMode();
}

/**
 * @brief 这个函数会在main()里面的1s循环里面, 
 *          所以这个程序会每秒钟执行一次
 * 
 */
void Run(void)
{
    char *_buff[64];

    switch (sys_mode)
    {

    case SYS_MODE_INI:
        if (gprs_state < GPRS_READY)
        {
            gprs_Ini();
        }
        else if (gprs_state == GPRS_READY)
        {
            // sys_mode = SystemModeSelect();
            sys_mode = SYS_MODE_REMOTE;
        }
        
        break;

    case  SYS_MODE_NORMAL:
        // if (gprs_state < GPRS_READY){
        //     gprs_Ini();
        // }

        if(remote_req_timer>0){
            remote_req_timer--;
            sprintf(_buff, ">> remote req timer: %d", remote_req_timer);
            RS485_Out(_buff);
        }else{
            remote_req_timer = REMOTE_REQ_TIMER_MAX;
            sys_mode = SYS_MODE_REMOTE;
        }
        
        break;
    
    case SYS_MODE_DEBUG:
        if (SW1_state == LOW)
        {
            gprs_Enter_Setting();
        }else {
            gprs_Exit_At_Mode();
        }
        
        break;

    case SYS_MODE_REMOTE:
        // RS485_Out(">> SYS remote mode:");
        gprs_Remote_Req();
        if (gprs_state == GPRS_READY)
        {
            remote_req_timer = REMOTE_REQ_TIMER_MAX;
            sys_mode = SYS_MODE_NORMAL;
        }
        
        break;

    default:
        break;
    }


    //  OLED draw something
    // u8g2_SendBuffer(&u8g2);
    // u8g2_DrawBox(&u8g2, 0,0,20,20);
    // u8g2_DrawBox(&u8g2, 20,20,20,20);
    // u8g2_SendBuffer(&u8g2);
    // u8g2_DrawFrame(&u8g2, 10, 40, 20, 20);
    // u8g2_SendBuffer(&u8g2);
    // u8g2_SetFont(&u8g2, u8g2_font_DigitalDiscoThin_tf);
    // u8g2_DrawStr(&u8g2, 20, 10, "free");
    // u8g2_SendBuffer(&u8g2);


}

void DI_Filter(void){

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



SYS_MODE SystemModeSelect(void)
{
    SYS_MODE _mode;

    uint8_t sw;

    sw = HAL_GPIO_ReadPin(SW2_GPIO_Port, SW2_Pin);
    sw = sw | (HAL_GPIO_ReadPin(SW3_GPIO_Port, SW3_Pin) << 1);
    // sw = sw | (HAL_GPIO_ReadPin(SW3_GPIO_Port, SW3_Pin) << 2);

    switch (sw)
    {
    case 0x1: // 3 - 2 
        _mode = SYS_MODE_DEBUG;
        break;
 
    case 0x0: // 3 - 3
        _mode = SYS_MODE_FACTORY_LOAD;
        break;
    
    default:
        _mode = SYS_MODE_NORMAL;
        break;
    }

    return _mode;
}

void SW1_State_Change(void){
    if (HAL_GPIO_ReadPin(SW1_GPIO_Port, SW1_Pin) == GPIO_PIN_RESET)
    {
        if (SW1_state == HIGH || SW1_state == RAISED)
        {
            SW1_state = FALLED;
        }else if(SW1_state == FALLED){
            SW1_state = LOW;
        }
        
    }else{

        if (SW1_state == LOW || SW1_state == FALLED)
        {
            SW1_state = RAISED;
        }else if(SW1_state == RAISED){
            SW1_state = HIGH;
        }
    }
    
}

void Led_Blink_Normal_Mode(void)
{
    if (led_count > 0)
    {
        led_count--;
    }
    else
    {
        led_count = 20;
    }

    if (led_count > 18)
    {
        HAL_GPIO_WritePin(LED_STAT_GPIO_Port, LED_STAT_Pin, GPIO_PIN_SET);
    }
    else
    {
        HAL_GPIO_WritePin(LED_STAT_GPIO_Port, LED_STAT_Pin, GPIO_PIN_RESET);
    }
}

void Led_Blink_Debug_Mode(void)
{
    if (led_count > 0)
    {
        led_count--;
    }
    else
    {
        led_count = 10;
    }

    if (led_count > 5)
    {
        HAL_GPIO_WritePin(LED_STAT_GPIO_Port, LED_STAT_Pin, GPIO_PIN_SET);
    }
    else
    {
        HAL_GPIO_WritePin(LED_STAT_GPIO_Port, LED_STAT_Pin, GPIO_PIN_RESET);
    }
}

void Led_Blink_AT_Mode(void){

    HAL_GPIO_TogglePin(LED_STAT_GPIO_Port, LED_STAT_Pin);
}

void Led_Blink_REQ_Mode(void){

    HAL_GPIO_TogglePin(LED_STAT_GPIO_Port, LED_STAT_Pin);
}