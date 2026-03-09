/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "stm32f4xx_hal.h"

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
#define B1_Pin GPIO_PIN_13
#define B1_GPIO_Port GPIOC
#define J1L1_Pin GPIO_PIN_0
#define J1L1_GPIO_Port GPIOA
#define J1L2_Pin GPIO_PIN_1
#define J1L2_GPIO_Port GPIOA
#define USART_TX_Pin GPIO_PIN_2
#define USART_TX_GPIO_Port GPIOA
#define USART_RX_Pin GPIO_PIN_3
#define USART_RX_GPIO_Port GPIOA
#define LD2_Pin GPIO_PIN_5
#define LD2_GPIO_Port GPIOA
#define PBJ1_Pin GPIO_PIN_6
#define PBJ1_GPIO_Port GPIOA
#define PBJ1_EXTI_IRQn EXTI9_5_IRQn
#define E_Pin GPIO_PIN_4
#define E_GPIO_Port GPIOC
#define B_Pin GPIO_PIN_1
#define B_GPIO_Port GPIOB
#define A_Pin GPIO_PIN_2
#define A_GPIO_Port GPIOB
#define J2L2_Pin GPIO_PIN_10
#define J2L2_GPIO_Port GPIOB
#define F_Pin GPIO_PIN_12
#define F_GPIO_Port GPIOB
#define D_Pin GPIO_PIN_13
#define D_GPIO_Port GPIOB
#define C_Pin GPIO_PIN_14
#define C_GPIO_Port GPIOB
#define DP_Pin GPIO_PIN_15
#define DP_GPIO_Port GPIOB
#define PBJ2_Pin GPIO_PIN_7
#define PBJ2_GPIO_Port GPIOC
#define PBJ2_EXTI_IRQn EXTI9_5_IRQn
#define J2L3_Pin GPIO_PIN_8
#define J2L3_GPIO_Port GPIOA
#define J2L4_Pin GPIO_PIN_9
#define J2L4_GPIO_Port GPIOA
#define J1L3_Pin GPIO_PIN_10
#define J1L3_GPIO_Port GPIOA
#define G_Pin GPIO_PIN_11
#define G_GPIO_Port GPIOA
#define TMS_Pin GPIO_PIN_13
#define TMS_GPIO_Port GPIOA
#define TCK_Pin GPIO_PIN_14
#define TCK_GPIO_Port GPIOA
#define SWO_Pin GPIO_PIN_3
#define SWO_GPIO_Port GPIOB
#define J2L1_Pin GPIO_PIN_4
#define J2L1_GPIO_Port GPIOB
#define J1L4_Pin GPIO_PIN_5
#define J1L4_GPIO_Port GPIOB
#define PBSTART_Pin GPIO_PIN_8
#define PBSTART_GPIO_Port GPIOB
#define PBSTART_EXTI_IRQn EXTI9_5_IRQn

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
