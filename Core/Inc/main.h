/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED_STAT_Pin GPIO_PIN_1
#define LED_STAT_GPIO_Port GPIOA
#define UART_TX_Pin GPIO_PIN_2
#define UART_TX_GPIO_Port GPIOA
#define UART_RX_Pin GPIO_PIN_3
#define UART_RX_GPIO_Port GPIOA
#define GPRS_WORK_Pin GPIO_PIN_4
#define GPRS_WORK_GPIO_Port GPIOA
#define GPRS_NET_Pin GPIO_PIN_5
#define GPRS_NET_GPIO_Port GPIOA
#define GPRS_LINKA_Pin GPIO_PIN_6
#define GPRS_LINKA_GPIO_Port GPIOA
#define GPRS_LINKB_Pin GPIO_PIN_7
#define GPRS_LINKB_GPIO_Port GPIOA
#define SW1_Pin GPIO_PIN_0
#define SW1_GPIO_Port GPIOB
#define SW2_Pin GPIO_PIN_1
#define SW2_GPIO_Port GPIOB
#define SW3_Pin GPIO_PIN_2
#define SW3_GPIO_Port GPIOB
#define EN_12V_Pin GPIO_PIN_11
#define EN_12V_GPIO_Port GPIOB
#define DI_1_Pin GPIO_PIN_12
#define DI_1_GPIO_Port GPIOB
#define DI_1_EXTI_IRQn EXTI4_15_IRQn
#define DI_2_Pin GPIO_PIN_13
#define DI_2_GPIO_Port GPIOB
#define DI_2_EXTI_IRQn EXTI4_15_IRQn
#define DI_3_Pin GPIO_PIN_14
#define DI_3_GPIO_Port GPIOB
#define DI_3_EXTI_IRQn EXTI4_15_IRQn
#define DI_4_Pin GPIO_PIN_15
#define DI_4_GPIO_Port GPIOB
#define DI_4_EXTI_IRQn EXTI4_15_IRQn
#define RS485_TX_Pin GPIO_PIN_9
#define RS485_TX_GPIO_Port GPIOA
#define RS485_RX_Pin GPIO_PIN_10
#define RS485_RX_GPIO_Port GPIOA
#define RS485_DE_Pin GPIO_PIN_12
#define RS485_DE_GPIO_Port GPIOA
#define PWR_CHECK_Pin GPIO_PIN_3
#define PWR_CHECK_GPIO_Port GPIOB
#define PWR_CHECK_EXTI_IRQn EXTI2_3_IRQn
#define SCL_Pin GPIO_PIN_6
#define SCL_GPIO_Port GPIOB
#define SDA_Pin GPIO_PIN_7
#define SDA_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */
// uint8_t gprs_rx_cnt = 0;
// uint8_t bRxBuffer;
// uint8_t gprsRxBuffer[256];
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
