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
#include "stm32f1xx_hal.h"

#include "stm32f1xx_ll_tim.h"
#include "stm32f1xx_ll_system.h"
#include "stm32f1xx_ll_gpio.h"
#include "stm32f1xx_ll_exti.h"
#include "stm32f1xx_ll_bus.h"
#include "stm32f1xx_ll_cortex.h"
#include "stm32f1xx_ll_rcc.h"
#include "stm32f1xx_ll_utils.h"
#include "stm32f1xx_ll_pwr.h"
#include "stm32f1xx_ll_dma.h"

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
void mainSysTicCallBack();
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define TIM_LED_PERIOD 1000-1
#define TIM_LED_PSC 72-1
#define ID_8_Pin LL_GPIO_PIN_13
#define ID_8_GPIO_Port GPIOC
#define ID_9_Pin LL_GPIO_PIN_14
#define ID_9_GPIO_Port GPIOC
#define ID_10_Pin LL_GPIO_PIN_15
#define ID_10_GPIO_Port GPIOC
#define ID_11_Pin LL_GPIO_PIN_0
#define ID_11_GPIO_Port GPIOA
#define ID_12_Pin LL_GPIO_PIN_1
#define ID_12_GPIO_Port GPIOA
#define ID_13_Pin LL_GPIO_PIN_2
#define ID_13_GPIO_Port GPIOA
#define AI_X_Pin LL_GPIO_PIN_3
#define AI_X_GPIO_Port GPIOA
#define AI_Y_Pin LL_GPIO_PIN_4
#define AI_Y_GPIO_Port GPIOA
#define ID_1_Pin LL_GPIO_PIN_5
#define ID_1_GPIO_Port GPIOA
#define ID_2_Pin LL_GPIO_PIN_6
#define ID_2_GPIO_Port GPIOA
#define ID_3_Pin LL_GPIO_PIN_7
#define ID_3_GPIO_Port GPIOA
#define ID_4_Pin LL_GPIO_PIN_0
#define ID_4_GPIO_Port GPIOB
#define ID_5_Pin LL_GPIO_PIN_1
#define ID_5_GPIO_Port GPIOB
#define ID_6_Pin LL_GPIO_PIN_10
#define ID_6_GPIO_Port GPIOB
#define ID_7_Pin LL_GPIO_PIN_11
#define ID_7_GPIO_Port GPIOB
#define W5500_CS_Pin LL_GPIO_PIN_12
#define W5500_CS_GPIO_Port GPIOB
#define W5500_SCK_Pin LL_GPIO_PIN_13
#define W5500_SCK_GPIO_Port GPIOB
#define W5500_MISO_Pin LL_GPIO_PIN_14
#define W5500_MISO_GPIO_Port GPIOB
#define W5500_MOSI_Pin LL_GPIO_PIN_15
#define W5500_MOSI_GPIO_Port GPIOB
#define W5500_INT_Pin LL_GPIO_PIN_8
#define W5500_INT_GPIO_Port GPIOA
#define W5500_INT_EXTI_IRQn EXTI9_5_IRQn
#define W5500_RST_Pin LL_GPIO_PIN_9
#define W5500_RST_GPIO_Port GPIOA
#define W5500_RST_EXTI_IRQn EXTI9_5_IRQn
#define USB_ReNum_Pin LL_GPIO_PIN_10
#define USB_ReNum_GPIO_Port GPIOA
#define OD_LED2G_Pin LL_GPIO_PIN_15
#define OD_LED2G_GPIO_Port GPIOA
#define OD_LED1G_Pin LL_GPIO_PIN_3
#define OD_LED1G_GPIO_Port GPIOB
#define OD_LED1R_Pin LL_GPIO_PIN_4
#define OD_LED1R_GPIO_Port GPIOB
#define UART_RTU_TX_Pin LL_GPIO_PIN_6
#define UART_RTU_TX_GPIO_Port GPIOB
#define UART_RTU_RX_Pin LL_GPIO_PIN_7
#define UART_RTU_RX_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */


#define MAIN_DBG_MCU_START()  DWT->CTRL |= 1<<DWT_CTRL_CYCCNTENA_Pos
#define MAIN_DBG_MCU_GET() DWT->CYCCNT


#define MAIN_ARRAY_LEN(A) (sizeof(A)/sizeof(A[0]))

// typedef union 
// {
//   uint32_t uid32[3];
//   uint8_t  uid8[12];
// }main_uid_typedef;



/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
