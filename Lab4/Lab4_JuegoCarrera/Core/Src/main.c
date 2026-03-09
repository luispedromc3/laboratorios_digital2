/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdint.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim6;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
uint8_t contador1 = 0;
uint8_t contador2 = 0;
uint8_t bandera_ganador = 0;
uint8_t bandera_start = 0;
uint8_t contador_tim = 0;
uint8_t bandera_carrera = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_TIM6_Init(void);
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
  MX_USART2_UART_Init();
  MX_TIM6_Init();
  /* USER CODE BEGIN 2 */
HAL_TIM_Base_Start_IT(&htim6);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	while(bandera_carrera == 0){

	}

	if(bandera_ganador){
		//HAL_GPIO_WritePin(B_GPIO_Port, B_Pin, 1);
		//HAL_GPIO_WritePin(C_GPIO_Port, C_Pin, 1);
	}else{
		if(contador1 == 1){
		    	HAL_GPIO_WritePin(J1L1_GPIO_Port, J1L1_Pin, 1);
		    	HAL_GPIO_WritePin(J1L2_GPIO_Port, J1L2_Pin, 0);
		    	HAL_GPIO_WritePin(J1L3_GPIO_Port, J1L3_Pin, 0);
		    	HAL_GPIO_WritePin(J1L4_GPIO_Port, J1L4_Pin, 0);
		    } else if (contador1 == 2){
		    	HAL_GPIO_WritePin(J1L1_GPIO_Port, J1L1_Pin, 0);
		    	HAL_GPIO_WritePin(J1L2_GPIO_Port, J1L2_Pin, 1);
		    	HAL_GPIO_WritePin(J1L3_GPIO_Port, J1L3_Pin, 0);
		    	HAL_GPIO_WritePin(J1L4_GPIO_Port, J1L4_Pin, 0);
		    }else if (contador1 == 3){
		    	HAL_GPIO_WritePin(J1L1_GPIO_Port, J1L1_Pin, 0);
		    	HAL_GPIO_WritePin(J1L2_GPIO_Port, J1L2_Pin, 0);
		    	HAL_GPIO_WritePin(J1L3_GPIO_Port, J1L3_Pin, 1);
		    	HAL_GPIO_WritePin(J1L4_GPIO_Port, J1L4_Pin, 0);
		    }else if (contador1 == 4){
		    	HAL_GPIO_WritePin(J1L1_GPIO_Port, J1L1_Pin, 0);
		    	HAL_GPIO_WritePin(J1L2_GPIO_Port, J1L2_Pin, 0);
		    	HAL_GPIO_WritePin(J1L3_GPIO_Port, J1L3_Pin, 0);
		    	HAL_GPIO_WritePin(J1L4_GPIO_Port, J1L4_Pin, 1);
		    }else if (contador1>=5){
		    	bandera_ganador = 1;
		    	HAL_GPIO_WritePin(J1L1_GPIO_Port, J1L1_Pin, 1);
		    	HAL_GPIO_WritePin(J1L2_GPIO_Port, J1L2_Pin, 1);
		    	HAL_GPIO_WritePin(J1L3_GPIO_Port, J1L3_Pin, 1);
		    	HAL_GPIO_WritePin(J1L4_GPIO_Port, J1L4_Pin, 1);
		    	HAL_GPIO_WritePin(J2L1_GPIO_Port, J2L1_Pin, 0);
		    	HAL_GPIO_WritePin(J2L2_GPIO_Port, J2L2_Pin, 0);
		    	HAL_GPIO_WritePin(J2L3_GPIO_Port, J2L3_Pin, 0);
		    	HAL_GPIO_WritePin(J2L4_GPIO_Port, J2L4_Pin, 0);

		    	HAL_GPIO_WritePin(A_GPIO_Port, A_Pin, 0);
				HAL_GPIO_WritePin(B_GPIO_Port, B_Pin, 0);
				HAL_GPIO_WritePin(C_GPIO_Port, C_Pin, 0);
				HAL_GPIO_WritePin(D_GPIO_Port, D_Pin, 0);
				HAL_GPIO_WritePin(E_GPIO_Port, E_Pin, 0);
				HAL_GPIO_WritePin(F_GPIO_Port, F_Pin, 0);
				HAL_GPIO_WritePin(G_GPIO_Port, G_Pin, 0);

				HAL_GPIO_WritePin(B_GPIO_Port, B_Pin, 1);
				HAL_GPIO_WritePin(C_GPIO_Port, C_Pin, 1);
				bandera_start = 0;
				bandera_carrera =0;
		    }

		    if(contador2 == 1){
		        	HAL_GPIO_WritePin(J2L1_GPIO_Port, J2L1_Pin, 1);
		        	HAL_GPIO_WritePin(J2L2_GPIO_Port, J2L2_Pin, 0);
		        	HAL_GPIO_WritePin(J2L3_GPIO_Port, J2L3_Pin, 0);
		        	HAL_GPIO_WritePin(J2L4_GPIO_Port, J2L4_Pin, 0);
		        } else if (contador2 == 2){
		        	HAL_GPIO_WritePin(J2L1_GPIO_Port, J2L1_Pin, 0);
		        	HAL_GPIO_WritePin(J2L2_GPIO_Port, J2L2_Pin, 1);
		        	HAL_GPIO_WritePin(J2L3_GPIO_Port, J2L3_Pin, 0);
		        	HAL_GPIO_WritePin(J2L4_GPIO_Port, J2L4_Pin, 0);
		        }else if (contador2 == 3){
		        	HAL_GPIO_WritePin(J2L1_GPIO_Port, J2L1_Pin, 0);
		        	HAL_GPIO_WritePin(J2L2_GPIO_Port, J2L2_Pin, 0);
		        	HAL_GPIO_WritePin(J2L3_GPIO_Port, J2L3_Pin, 1);
		        	HAL_GPIO_WritePin(J2L4_GPIO_Port, J2L4_Pin, 0);
		        }else if (contador2 == 4){
		        	HAL_GPIO_WritePin(J2L1_GPIO_Port, J2L1_Pin, 0);
		        	HAL_GPIO_WritePin(J2L2_GPIO_Port, J2L2_Pin, 0);
		        	HAL_GPIO_WritePin(J2L3_GPIO_Port, J2L3_Pin, 0);
		        	HAL_GPIO_WritePin(J2L4_GPIO_Port, J2L4_Pin, 1);
		        }else if(contador2 >=5){
		        	bandera_ganador = 1;
		        	HAL_GPIO_WritePin(J2L1_GPIO_Port, J2L1_Pin, 1);
		        	HAL_GPIO_WritePin(J2L2_GPIO_Port, J2L2_Pin, 1);
		        	HAL_GPIO_WritePin(J2L3_GPIO_Port, J2L3_Pin, 1);
		        	HAL_GPIO_WritePin(J2L4_GPIO_Port, J2L4_Pin, 1);
		        	HAL_GPIO_WritePin(J1L1_GPIO_Port, J1L1_Pin, 0);
		        	HAL_GPIO_WritePin(J1L2_GPIO_Port, J1L2_Pin, 0);
		        	HAL_GPIO_WritePin(J1L3_GPIO_Port, J1L3_Pin, 0);
		        	HAL_GPIO_WritePin(J1L4_GPIO_Port, J1L4_Pin, 0);

		        	HAL_GPIO_WritePin(A_GPIO_Port, A_Pin, 0);
					HAL_GPIO_WritePin(B_GPIO_Port, B_Pin, 0);
					HAL_GPIO_WritePin(C_GPIO_Port, C_Pin, 0);
					HAL_GPIO_WritePin(D_GPIO_Port, D_Pin, 0);
					HAL_GPIO_WritePin(E_GPIO_Port, E_Pin, 0);
					HAL_GPIO_WritePin(F_GPIO_Port, F_Pin, 0);
					HAL_GPIO_WritePin(G_GPIO_Port, G_Pin, 0);
					//2 ABGED
					HAL_GPIO_WritePin(A_GPIO_Port, A_Pin, 1);
					HAL_GPIO_WritePin(B_GPIO_Port, B_Pin, 1);
					HAL_GPIO_WritePin(G_GPIO_Port, G_Pin, 1);
					HAL_GPIO_WritePin(E_GPIO_Port, E_Pin, 1);
					HAL_GPIO_WritePin(D_GPIO_Port, D_Pin, 1);
					bandera_start = 0;
					bandera_carrera =0;
		        }
	}

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 64;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV8;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM6_Init(void)
{

  /* USER CODE BEGIN TIM6_Init 0 */

  /* USER CODE END TIM6_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM6_Init 1 */

  /* USER CODE END TIM6_Init 1 */
  htim6.Instance = TIM6;
  htim6.Init.Prescaler = 1600-1;
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 2497-1;
  htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM6_Init 2 */

  /* USER CODE END TIM6_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, J1L1_Pin|J1L2_Pin|LD2_Pin|J2L3_Pin
                          |J2L4_Pin|J1L3_Pin|G_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(E_GPIO_Port, E_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, B_Pin|A_Pin|J2L2_Pin|F_Pin
                          |D_Pin|C_Pin|DP_Pin|J2L1_Pin
                          |J1L4_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : B1_Pin PBJ2_Pin */
  GPIO_InitStruct.Pin = B1_Pin|PBJ2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : J1L1_Pin J1L2_Pin LD2_Pin J2L3_Pin
                           J2L4_Pin J1L3_Pin G_Pin */
  GPIO_InitStruct.Pin = J1L1_Pin|J1L2_Pin|LD2_Pin|J2L3_Pin
                          |J2L4_Pin|J1L3_Pin|G_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PBJ1_Pin */
  GPIO_InitStruct.Pin = PBJ1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(PBJ1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : E_Pin */
  GPIO_InitStruct.Pin = E_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(E_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : B_Pin A_Pin J2L2_Pin F_Pin
                           D_Pin C_Pin DP_Pin J2L1_Pin
                           J1L4_Pin */
  GPIO_InitStruct.Pin = B_Pin|A_Pin|J2L2_Pin|F_Pin
                          |D_Pin|C_Pin|DP_Pin|J2L1_Pin
                          |J1L4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PBSTART_Pin */
  GPIO_InitStruct.Pin = PBSTART_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(PBSTART_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	if (GPIO_Pin == PBSTART_Pin){
		bandera_start = 1;
		bandera_ganador = 0;
		contador_tim =0;
		HAL_GPIO_WritePin(A_GPIO_Port, A_Pin, 0);
		HAL_GPIO_WritePin(B_GPIO_Port, B_Pin, 0);
		HAL_GPIO_WritePin(C_GPIO_Port, C_Pin, 0);
		HAL_GPIO_WritePin(D_GPIO_Port, D_Pin, 0);
		HAL_GPIO_WritePin(E_GPIO_Port, E_Pin, 0);
		HAL_GPIO_WritePin(F_GPIO_Port, F_Pin, 0);
		HAL_GPIO_WritePin(G_GPIO_Port, G_Pin, 0);
		HAL_GPIO_WritePin(DP_GPIO_Port, DP_Pin, 0);
		HAL_GPIO_WritePin(J1L1_GPIO_Port, J1L1_Pin, 0);
		HAL_GPIO_WritePin(J1L2_GPIO_Port, J1L2_Pin, 0);
		HAL_GPIO_WritePin(J1L3_GPIO_Port, J1L3_Pin, 0);
		HAL_GPIO_WritePin(J1L4_GPIO_Port, J1L4_Pin, 0);
		HAL_GPIO_WritePin(J2L1_GPIO_Port, J2L1_Pin, 0);
		HAL_GPIO_WritePin(J2L2_GPIO_Port, J2L2_Pin, 0);
		HAL_GPIO_WritePin(J2L3_GPIO_Port, J2L3_Pin, 0);
		HAL_GPIO_WritePin(J2L4_GPIO_Port, J2L4_Pin, 0);
	} else if(GPIO_Pin == PBJ2_Pin){
		contador2++;
	}else if(GPIO_Pin == PBJ1_Pin){
		contador1++;
		//HAL_GPIO_TogglePin(J1L1_GPIO_Port, J2L2_Pin);
	}

}
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	//HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
	if(bandera_start){
		contador_tim++;
		// 5 AFGCD
		if (contador_tim <=3){
			HAL_GPIO_WritePin(A_GPIO_Port, A_Pin, 1);
			HAL_GPIO_WritePin(F_GPIO_Port, F_Pin, 1);
			HAL_GPIO_WritePin(D_GPIO_Port, D_Pin, 1);
			HAL_GPIO_WritePin(G_GPIO_Port, G_Pin, 1);
			HAL_GPIO_WritePin(C_GPIO_Port, C_Pin, 1);
		}

		if(contador_tim == 4){
			//4 FGBC
			HAL_GPIO_WritePin(A_GPIO_Port, A_Pin, 0);
			HAL_GPIO_WritePin(B_GPIO_Port, B_Pin, 0);
			HAL_GPIO_WritePin(C_GPIO_Port, C_Pin, 0);
			HAL_GPIO_WritePin(D_GPIO_Port, D_Pin, 0);
			HAL_GPIO_WritePin(E_GPIO_Port, E_Pin, 0);
			HAL_GPIO_WritePin(F_GPIO_Port, F_Pin, 0);
			HAL_GPIO_WritePin(G_GPIO_Port, G_Pin, 0);

			HAL_GPIO_WritePin(F_GPIO_Port, F_Pin, 1);
			HAL_GPIO_WritePin(G_GPIO_Port, G_Pin, 1);
			HAL_GPIO_WritePin(B_GPIO_Port, B_Pin, 1);
			HAL_GPIO_WritePin(C_GPIO_Port, C_Pin, 1);
		}else if(contador_tim == 8){
			HAL_GPIO_WritePin(A_GPIO_Port, A_Pin, 0);
			HAL_GPIO_WritePin(B_GPIO_Port, B_Pin, 0);
			HAL_GPIO_WritePin(C_GPIO_Port, C_Pin, 0);
			HAL_GPIO_WritePin(D_GPIO_Port, D_Pin, 0);
			HAL_GPIO_WritePin(E_GPIO_Port, E_Pin, 0);
			HAL_GPIO_WritePin(F_GPIO_Port, F_Pin, 0);
			HAL_GPIO_WritePin(G_GPIO_Port, G_Pin, 0);
			//3 ABGCD
			HAL_GPIO_WritePin(A_GPIO_Port, A_Pin, 1);
			HAL_GPIO_WritePin(B_GPIO_Port, B_Pin, 1);
			HAL_GPIO_WritePin(G_GPIO_Port, G_Pin, 1);
			HAL_GPIO_WritePin(C_GPIO_Port, C_Pin, 1);
			HAL_GPIO_WritePin(D_GPIO_Port, D_Pin, 1);
		} else if(contador_tim == 12){
			HAL_GPIO_WritePin(A_GPIO_Port, A_Pin, 0);
			HAL_GPIO_WritePin(B_GPIO_Port, B_Pin, 0);
			HAL_GPIO_WritePin(C_GPIO_Port, C_Pin, 0);
			HAL_GPIO_WritePin(D_GPIO_Port, D_Pin, 0);
			HAL_GPIO_WritePin(E_GPIO_Port, E_Pin, 0);
			HAL_GPIO_WritePin(F_GPIO_Port, F_Pin, 0);
			HAL_GPIO_WritePin(G_GPIO_Port, G_Pin, 0);
			//2 ABGED
			HAL_GPIO_WritePin(A_GPIO_Port, A_Pin, 1);
			HAL_GPIO_WritePin(B_GPIO_Port, B_Pin, 1);
			HAL_GPIO_WritePin(G_GPIO_Port, G_Pin, 1);
			HAL_GPIO_WritePin(E_GPIO_Port, E_Pin, 1);
			HAL_GPIO_WritePin(D_GPIO_Port, D_Pin, 1);
		} else if(contador_tim == 16){
			HAL_GPIO_WritePin(A_GPIO_Port, A_Pin, 0);
			HAL_GPIO_WritePin(B_GPIO_Port, B_Pin, 0);
			HAL_GPIO_WritePin(C_GPIO_Port, C_Pin, 0);
			HAL_GPIO_WritePin(D_GPIO_Port, D_Pin, 0);
			HAL_GPIO_WritePin(E_GPIO_Port, E_Pin, 0);
			HAL_GPIO_WritePin(F_GPIO_Port, F_Pin, 0);
			HAL_GPIO_WritePin(G_GPIO_Port, G_Pin, 0);

			HAL_GPIO_WritePin(B_GPIO_Port, B_Pin, 1);
			HAL_GPIO_WritePin(C_GPIO_Port, C_Pin, 1);
		} else if (contador_tim == 20){
			//ABCDEF
			HAL_GPIO_WritePin(A_GPIO_Port, A_Pin, 0);
			HAL_GPIO_WritePin(B_GPIO_Port, B_Pin, 0);
			HAL_GPIO_WritePin(C_GPIO_Port, C_Pin, 0);
			HAL_GPIO_WritePin(D_GPIO_Port, D_Pin, 0);
			HAL_GPIO_WritePin(E_GPIO_Port, E_Pin, 0);
			HAL_GPIO_WritePin(F_GPIO_Port, F_Pin, 0);
			HAL_GPIO_WritePin(G_GPIO_Port, G_Pin, 0);
			HAL_GPIO_WritePin(A_GPIO_Port, A_Pin, 1);
			HAL_GPIO_WritePin(B_GPIO_Port, B_Pin, 1);
			HAL_GPIO_WritePin(C_GPIO_Port, C_Pin, 1);
			HAL_GPIO_WritePin(D_GPIO_Port, D_Pin, 1);
			HAL_GPIO_WritePin(E_GPIO_Port, E_Pin, 1);
			HAL_GPIO_WritePin(F_GPIO_Port, F_Pin, 1);
			HAL_GPIO_WritePin(DP_GPIO_Port, DP_Pin, 1);
			bandera_carrera = 1;
			contador1 = 0;
			contador2 = 0;
		}
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
#ifdef USE_FULL_ASSERT
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
