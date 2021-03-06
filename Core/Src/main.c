/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2022 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
// #include <stdio.h>
#include <string.h>
#include "func.h"
#include "para.h"
#include "rs485.h"
#include "gprs_7gx.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
// #define h_rs485 huart1
// #define h_gprs huart2


/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

SYS_MODE sys_mode;

uint8_t aRxBuffer;
uint8_t bRxBuffer;

PARA para;
FILTER filter;
CH_STATE ch_state[4];
CH_STATE pwr_check_state;
uint8_t pwr_check_filter;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  uint32_t _timeout_100ms = 0, _timeout_1s = 0;

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */

  // sys_mode = SystemModeSelect();
  sys_mode = SYS_MODE_INI;

  Para_Init();

  HAL_UART_Receive_IT(&h_rs485, (uint8_t *)&aRxBuffer, 1);
  HAL_UART_Receive_IT(&h_gprs, (uint8_t *)&bRxBuffer, 1);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    // 100ms
    if ((HAL_GetTick() - _timeout_100ms) >= 100)
    {
      _timeout_100ms = HAL_GetTick();

      Update_State();
    }

    // 1s
    if ((HAL_GetTick() - _timeout_1s) >= 1000)
    {
      _timeout_1s = HAL_GetTick();

      Run();


    }
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL4;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_I2C1;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_HSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  UNUSED(GPIO_Pin);

  if (GPIO_Pin == PWR_CHECK_Pin)
  {
    if (pwr_check_state == READY)
    {
      pwr_check_state = TRIGGED;
      UserWrite();
      HAL_UART_Transmit(&h_rs485, (uint8_t *)"PWR_TRIGGED\r", 12, 0xffff);
      // exit_trig_state = PWR_TRIGGED;
    }
  }
  else if (GPIO_Pin == DI_3_Pin)
  {

    if (ch_state[CH1] == READY)
    {
      ch_state[CH1] = TRIGGED;
      HAL_UART_Transmit(&h_rs485, (uint8_t *)"DI1_TRIGGED\r", 12, 0xffff);
    }
    // exit_trig_state = DI1_TRIGGED;
  }
  else if (GPIO_Pin == DI_4_Pin)
  {
    if (ch_state[CH2] == READY)
    {
      ch_state[CH2] = TRIGGED;
      HAL_UART_Transmit(&h_rs485, (uint8_t *)"DI2_TRIGGED\r", 12, 0xffff);
    }
    // exit_trig_state = DI2_TRIGGED;
  }
  else if (GPIO_Pin == DI_1_Pin)
  {
    if (ch_state[CH3] == READY)
    {
      ch_state[CH3] = TRIGGED;
      HAL_UART_Transmit(&h_rs485, (uint8_t *)"DI3_TRIGGED\r", 12, 0xffff);
    }
  }
  else if (GPIO_Pin == DI_2_Pin)
  {
    if (ch_state[CH4] == READY)
    {
      ch_state[CH4] = TRIGGED;
      HAL_UART_Transmit(&h_rs485, (uint8_t *)"DI4_TRIGGED\r", 12, 0xffff);
    }
  }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(huart);
  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_UART_RxCpltCallback can be implemented in the user file
   */
  
  if(huart == &h_rs485) {

    RS485_Receiver();

    HAL_UART_Receive_IT(&h_rs485, (uint8_t *)&aRxBuffer, 1);
  
  }else if( huart == &h_gprs){

    gprsReceiver();

    HAL_UART_Receive_IT(&h_gprs, (uint8_t *)&bRxBuffer, 1);
  }
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

