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
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "math.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#define NOTE_B3  247
#define NOTE_C5  523
#define NOTE_D5  587
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_G5  784
#define NOTE_A5  880
#define NOTE_C4  262
#define NOTE_D4  294
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_G4  392
#define NOTE_A4  440
#define NOTE_CS4 277   // Do# 4
#define NOTE_GS4 415   // Sol# 4
#define NOTE_AS4 466   // La# 4
#define NOTE_CS5 554   // Do# 5
#define NOTE_GS3 208   // Sol# 3
#define NOTE_AS3 233   // La# 3
#define NOTE_FS4 370   // Fa# 4
#define NOTE_DS4 311   // Re# 4
#define NOTE_B4  494   // Si 4
#define NOTE_DS5 622   // Re# 5
#define NOTE_FS5 740   // Fa# 5
#define NOTE_A3  220   // La 3
#define NOTE_C3  131   // Do 3
#define NOTE_D3  147   // Re 3
#define NOTE_E3  165   // Mi 3
#define NOTE_F3  175   // Fa 3
#define NOTE_G3  196   // Sol 3
#define NOTE_REST 0
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define TIM_FREQ 84000000

#define sinsize 128
#define TIM_ARR_VAL 100
#define Pi 3.1415926
#define TIM_PSC_VAL 1
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
DAC_HandleTypeDef hdac;
DMA_HandleTypeDef hdma_dac2;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim6;

UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_usart2_rx;

/* USER CODE BEGIN PV */
char estado[50];
uint8_t temp[2];
uint8_t val_can = 0;
uint32_t Ysen[128];
int fur_elise_freq[42] = {
    659, 622, 659, 622, 659, 494, 587, 523, 440,
    0,
    262, 330, 440,
    0,
    330, 415, 494,
    0,
    330, 659, 622, 659, 622, 659, 494, 587, 523, 440,
    0,
    262, 330, 440,
    0,
    330, 494, 440,
    0
};

int fur_elise_dur[42] = {
    180, 180, 180, 180, 180, 180, 180, 180, 360,
    120,
    180, 180, 360,
    120,
    180, 180, 360,
    120,
    180, 180, 180, 180, 180, 180, 180, 180, 360,
    120,
    180, 180, 360,
    120,
    180, 180, 400,
    200
};
int pirates_freq[34] = {
    440, 523, 587, 587, 587,
    659, 698, 659, 587,
    523, 440, 440,
    587, 659, 698, 659,
    587, 523, 587, 659,
    440, 0,
    440, 523, 587, 587,
    659, 698, 659, 587,
    523, 440, 440, 0
};

int pirates_dur[34] = {
    200, 200, 300, 150, 150,
    300, 200, 200, 300,
    200, 300, 200,
    200, 200, 300, 200,
    200, 200, 200, 400,
    300, 150,
    200, 200, 300, 150,
    300, 200, 200, 300,
    200, 400, 300, 500
};
int pirates_freq_dac[] = {
    NOTE_A4, NOTE_C5, NOTE_D5, NOTE_D5, NOTE_D5,
    NOTE_E5, NOTE_F5, NOTE_E5, NOTE_D5,
    NOTE_C5, NOTE_A4, NOTE_A4,
    NOTE_D5, NOTE_E5, NOTE_F5, NOTE_E5,
    NOTE_D5, NOTE_C5, NOTE_D5, NOTE_E5,
    NOTE_A4, NOTE_REST,
    NOTE_A4, NOTE_C5, NOTE_D5, NOTE_D5,
    NOTE_E5, NOTE_F5, NOTE_E5, NOTE_D5,
    NOTE_C5, NOTE_A4, NOTE_A4, NOTE_REST
};
int pirates_dur_dac[] = {
    200, 200, 300, 150, 150,
    300, 200, 200, 300,
    200, 300, 200,
    200, 200, 300, 200,
    200, 200, 200, 400,
    300, 150,
    200, 200, 300, 150,
    300, 200, 200, 300,
    200, 400, 300, 500
};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_TIM1_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_DAC_Init(void);
static void MX_TIM6_Init(void);
/* USER CODE BEGIN PFP */
int presForFrecuency(int frecuency);
void playTone(int *tone, int *duration, int *pause, int size);
void noTone(void);
void noToneDAC(void);
void generarSin(void);
void playToneDAC(int *tone, int *duration, int *pause, int Nsize);
int calcularARR(int freq);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int calcularARR(int freq){
    if (freq == 0)
        return 0;
    int TF = sinsize * freq;
    return ((TIM_FREQ / ((TIM_PSC_VAL + 1) * TF)) - 1);
}
void generarSin(void){
	for(int x=0; x<sinsize; x++){
		Ysen[x] = (uint32_t)((sin(x * 2.0 * Pi / sinsize) + 1.0) * (4095.0 / 2.0));
	}
}
void playToneDAC(int *tone, int *duration, int *pause, int Nsize){
    for (int i = 0; i < Nsize; i++) {
        int arr = calcularARR(tone[i]);  // ✅ usar calcularARR()
        int dur = duration[i];
        int pauseBetweenTones = 0;
        if (pause != NULL)
            pauseBetweenTones = pause[i] - duration[i];


        if (tone[i] != NOTE_REST) {
            __HAL_TIM_SET_AUTORELOAD(&htim6, arr);
            __HAL_TIM_SET_COUNTER(&htim6, 0);
            HAL_DAC_Start_DMA(&hdac, DAC_CHANNEL_2,
                (uint32_t*)Ysen, sinsize, DAC_ALIGN_12B_R);
            HAL_TIM_Base_Start(&htim6);
        } else {
            noToneDAC();
        }
        HAL_Delay(dur);
        noToneDAC();
        HAL_Delay(pauseBetweenTones);
    }
}
int presForFrequency(int frequency){
	if (frequency == 0)
		return 0;
	return((TIM_FREQ / (TIM_ARR_VAL * frequency)) - 1);
}
void noTone(void){
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, 0);
}
void playTone(int *tone, int *duration, int*pause, int size){
	for(int i = 0; i < size; i++){
	int prescaler = presForFrequency(tone[i]);
	int dur = duration[i];
	int pauseBetweenTones = 0;
	if(pause != NULL)
       pauseBetweenTones = pause[i] - duration[i];

	__HAL_TIM_SET_PRESCALER(&htim1, prescaler);
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, TIM_ARR_VAL / 2);
	HAL_Delay(dur);
	noTone();
	HAL_Delay(pauseBetweenTones);
	}
}

void playTonePRE(const int *tone, const int *duration, const int*pause, int size){
	for(int i = 0; i < size; i++){
	int prescaler = presForFrequency(tone[i]);
	int dur = duration[i]*2;
	int pauseBetweenTones = 0;
	if(pause != NULL)
       pauseBetweenTones = pause[i] - duration[i];

	__HAL_TIM_SET_PRESCALER(&htim1, prescaler);
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, TIM_ARR_VAL / 2);
	HAL_Delay(dur);
	noTone();

	HAL_Delay(pauseBetweenTones);
	}
}
void noToneDAC(void){
    HAL_DAC_Stop_DMA(&hdac, DAC_CHANNEL_2); // ✅
    HAL_TIM_Base_Stop(&htim6);              // ✅
}
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
  MX_DMA_Init();
  MX_TIM1_Init();
  MX_USART2_UART_Init();
  MX_DAC_Init();
  MX_TIM6_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4);
  HAL_UART_Receive_DMA(&huart2, temp, 1);
  HAL_DAC_Start(&hdac, DAC_CHANNEL_2);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  strcpy(estado, "----------Menu de canciones----------\n");
  HAL_UART_Transmit(&huart2, estado, strlen(estado), 100);
  strcpy(estado, "Opcion 1: Para Elisa de Bethoven\n");
  HAL_UART_Transmit(&huart2, estado, strlen(estado), 100);
  strcpy(estado, "Opcion 2: Piratas del caribe \n");
  HAL_UART_Transmit(&huart2, estado, strlen(estado), 100);
  strcpy(estado, "Seleccione la opcion deseada (ingrese el numero): ");
  HAL_UART_Transmit(&huart2, estado, strlen(estado), 100);
  val_can = 0;

  generarSin();
  while (1)
  {

	  if (val_can == 1){
		  playTonePRE(fur_elise_freq, fur_elise_dur,NULL,(sizeof(fur_elise_freq)/sizeof(fur_elise_freq[0])));
		  val_can = 0;
		  strcpy(estado, "\n");
		  HAL_UART_Transmit(&huart2, estado, strlen(estado), 100);
		  strcpy(estado, "Seleccione la opcion deseada (ingrese el numero): ");
		  HAL_UART_Transmit(&huart2, estado, strlen(estado), 100);
	  } else if(val_can == 2){
		  //playTonePRE(pirates_freq, pirates_dur,NULL,(sizeof(pirates_freq)/sizeof(pirates_freq[0])));
		  playToneDAC(pirates_freq_dac, pirates_dur_dac, NULL, sizeof(pirates_freq_dac)/sizeof(pirates_freq_dac[0]));
		  val_can = 0;
		  strcpy(estado, "\n");
		  HAL_UART_Transmit(&huart2, estado, strlen(estado), 100);
		  strcpy(estado, "Seleccione la opcion deseada (ingrese el numero): ");
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
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief DAC Initialization Function
  * @param None
  * @retval None
  */
static void MX_DAC_Init(void)
{

  /* USER CODE BEGIN DAC_Init 0 */

  /* USER CODE END DAC_Init 0 */

  DAC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN DAC_Init 1 */

  /* USER CODE END DAC_Init 1 */

  /** DAC Initialization
  */
  hdac.Instance = DAC;
  if (HAL_DAC_Init(&hdac) != HAL_OK)
  {
    Error_Handler();
  }

  /** DAC channel OUT2 config
  */
  sConfig.DAC_Trigger = DAC_TRIGGER_T6_TRGO;
  sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
  if (HAL_DAC_ConfigChannel(&hdac, &sConfig, DAC_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN DAC_Init 2 */

  /* USER CODE END DAC_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 0;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 100-1;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 50;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */
  HAL_TIM_MspPostInit(&htim1);

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
  htim6.Init.Prescaler = 0;
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 65535;
  htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
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
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);
  /* DMA1_Stream6_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream6_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream6_IRQn);

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

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	HAL_UART_Receive_IT(&huart2, temp, 1);
	if (temp[0] == '1'){
		val_can = 1;
	}else if(temp[0] == '2'){
		val_can = 2;
	}else{
		val_can = 0;
	}

}
void UART_DMAReceiveCplt(DMA_HandleTypeDef *hdma){
	//HAL_UART_Transmit(&huart2, prueba, strlen(prueba), 100);
	HAL_UART_Receive_DMA(&huart2, temp, 1);
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
