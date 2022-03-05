#include "oled.h"
#include "i2c.h"

uint8_t u8x8_byte_hw_i2c(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr){
    // u8g2/u8x8 will never send more than 32 bytes between START_TRANSFER and END_TRANSFER
    static uint8_t buffer[128];
    static uint8_t buf_idx;
    uint8_t *data;

    switch (msg)
    {
    case U8X8_MSG_BYTE_INIT:
        MX_I2C1_Init();
        break;

    case U8X8_MSG_BYTE_START_TRANSFER:
        buf_idx = 0;
        break;

    case U8X8_MSG_BYTE_SEND:
        data = (uint8_t *)arg_ptr;

        while (arg_int > 0)
        {
            buffer[buf_idx++] = *data;
            data++;
            arg_int--;
        }

        break;
    case U8X8_MSG_BYTE_END_TRANSFER:
        if(HAL_I2C_Master_Transmit(&hi2c1, (OLED_ADDRESS), buffer, buf_idx, 1000) != HAL_OK){
            return 0;
        }
        break;

    case U8X8_MSG_BYTE_SET_DC:

        break;

    default:
        return 0;
        break;
    }

    return 1;
}

void delay_us(uint32_t time)
{
    uint32_t i = 6 * time;
    while (i--)
    {
        ;
    }
}

uint8_t u8x8_gpio_and_delay(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr){
    switch(msg)
    {
        case U8X8_MSG_DELAY_100NANO:
            __NOP();
            break;

        case U8X8_MSG_DELAY_10MICRO:
            for( uint16_t n = 0; n<320; n++){
                __NOP();
            }
            break;

        case U8X8_MSG_DELAY_MILLI:
            HAL_Delay(1);
            break;

        case U8X8_MSG_DELAY_I2C:        // arg_int is the i2c speed in 100kHz, e.g. 4 = 400 kHz
            delay_us(5);                
            break;                      // arg_int=1 delay by 5us, arg_int = 4: delay by 1.25us
        case U8X8_MSG_GPIO_I2C_CLOCK:   // arg_int=0 Output low at i2c clock pin
            break;                      // arg_int=1 Input dir with pullup high for i2c clock pin
        case U8X8_MSG_GPIO_I2C_DATA:    // arg_int=0 Output low at i2c data pin
            break;                      // arg_int=1 Input dir with pullup high for i2c data pin
        case U8X8_MSG_GPIO_MENU_SELECT:
            u8x8_SetGPIOResult(
                u8x8,   // get menu select pin state
                0);
            break;
        case U8X8_MSG_GPIO_MENU_NEXT:
            u8x8_SetGPIOResult(
                u8x8,   // get menu home pin state 
                0
            );
            break;
        case U8X8_MSG_GPIO_MENU_PREV:
            u8x8_SetGPIOResult(
                u8x8,   // get menu home pin state 
                0
            );
            break;
        case U8X8_MSG_GPIO_MENU_HOME:
            u8x8_SetGPIOResult(
                u8x8,   // get menu home pin state 
                0
            );
            break;

        default:
            u8x8_SetGPIOResult(u8x8, 1);
            break;
    }

    return 1;
}
void u8g2Init(u8g2_t *u8g2){
    u8g2_Setup_ssd1306_128x64_noname_f(u8g2, U8G2_R0, u8x8_byte_hw_i2c, u8x8_gpio_and_delay);
    u8g2_InitDisplay(u8g2);
    u8g2_SetPowerSave(u8g2, 0);
    u8g2_ClearBuffer(u8g2);
}