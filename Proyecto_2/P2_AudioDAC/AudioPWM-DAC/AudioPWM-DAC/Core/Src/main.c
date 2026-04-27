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
#include "NOTES_PSC.h"
#include <string.h>
#include "math.h"


/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define TOTAL_NOTES (sizeof(mk_notes)/sizeof(uint16_t))
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
UART_HandleTypeDef huart3;
UART_HandleTypeDef huart6;
DMA_HandleTypeDef hdma_usart2_rx;
DMA_HandleTypeDef hdma_usart3_rx;
DMA_HandleTypeDef hdma_usart6_rx;

/* USER CODE BEGIN PV */


// Cancion para DAC
int silent_notes[] = {0};
int silent_durations[] = {1000};  // 1 segundo de silencio
int win_notes[] = {
    523, 659, 784, 1047,   // subida alegre
    988, 1047, 1175,       // remate
    1319, 0,               // nota alta + pausa
    1047, 1175, 1319,      // cierre triunfal
    1568
};

int win_durations[] = {
    150, 150, 150, 300,
    150, 150, 300,
    400, 150,
    150, 150, 300,
    600
};
int mario_dac_notes[] = {
    659, 659, 0, 659,
    0, 523, 659, 784,
    0, 392, 0,

    523, 392, 0, 330,
    440, 494, 466, 440,
    392, 659, 784, 880,
    698, 784, 0, 659,
    523, 587, 494
};

int mario_dac_durations[] = {
    150, 150, 150, 150,
    150, 150, 150, 300,
    300, 300, 300,

    150, 150, 150, 150,
    150, 150, 150, 150,
    200, 200, 200, 200,
    150, 150, 150, 150,
    150, 150, 300
};

int mk64_intro_notes[] = {
    659, 784, 988, 880, 784, 659,
    587, 659, 784, 659, 587, 523,
    659, 784, 988, 880, 784, 659,
    784, 880, 988, 0
};

int mk64_intro_durations[] = {
    180, 180, 360, 180, 180, 360,
    180, 180, 360, 180, 180, 360,
    180, 180, 360, 180, 180, 360,
    180, 180, 500, 200
};

// Cancion para DAC
int mk_notes[] = {
    DAC_FS4, DAC_A4, DAC_B4,
    DAC_D5,  DAC_CS5, DAC_B4,
    DAC_A4,  DAC_FS4,

    DAC_D4,  DAC_FS4, DAC_A4,
    DAC_B4,  DAC_A4,  DAC_FS4,
    DAC_E4,  DAC_D4,

    DAC_FS4, DAC_A4,  DAC_B4,
    DAC_D5,  DAC_CS5, DAC_B4,
    DAC_A4,  DAC_FS4,

    DAC_E4,  DAC_FS4, DAC_G4,
    DAC_A4,  DAC_B4,  DAC_A4,
    DAC_FS4,

    DAC_A4,  DAC_B4,  DAC_C5,
    DAC_B4,  DAC_A4,  DAC_G4,
    DAC_FS4, DAC_E4,

    DAC_D4,  DAC_E4,  DAC_FS4,
    DAC_A4,  DAC_G4,  DAC_FS4,
    DAC_E4,  DAC_D4
};

// Las duraciones no cambian
int mk_durations[] = {
    200, 200, 300,
    200, 200, 300,
    200, 400,

    200, 200, 300,
    200, 200, 300,
    200, 400,

    200, 200, 300,
    200, 200, 300,
    200, 400,

    200, 200, 200,
    200, 300, 200,
    400,

    200, 200, 300,
    200, 200, 200,
    300, 400,

    200, 200, 200,
    300, 200, 200,
    300, 400
};

// ================================================================================

uint8_t rxbuffer;
uint8_t rx3buffer;
uint8_t flag;
uint8_t option_selected = 0;

// DAC
#define size 128
uint16_t Ysine[size];

#define PI 3.14159265
#define TIM_FREQ 84000000
#define TIM_PSC 1


char dummy[60];

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_TIM1_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_DAC_Init(void);
static void MX_TIM6_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_USART6_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void play_note(uint16_t psc)
{
    if (psc == 0)
    {
        __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0);
        return;
    }

    __HAL_TIM_SET_PRESCALER(&htim1, psc);
    htim1.Instance->EGR = TIM_EGR_UG;  // CLAVE

    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 50);
}


void play_melody(uint16_t *notes, uint16_t *durations, uint16_t length)
{
    for (uint16_t i = 0; i < length; i++)
    {
        play_note(notes[i]);
        HAL_Delay(durations[i]);

        // pequeña pausa entre notas (mejora el sonido)
        play_note(DAC_REST);
        HAL_Delay(20);
    }
}

// DMA
void generarSin(void){
	for (int x = 0; x < size; x++){
		Ysine[x] = ((sin(x * 2 * PI / size) + 1) * (4096 / 2));
	}
}

int calcularARR(int freq){
	if (freq == 0){
		return 0;
	}

	int TF = size * freq;
	return ((TIM_FREQ / ((TIM_PSC+1)*TF))-1);

}



void noTone(void)
{
    TIM6->ARR = 0xFFFF;  // frecuencia muy baja (silencio práctico)
}

void playTone(int *tone, int *duration, int *pause, int Nsize)
{
	HAL_DAC_Start_DMA(&hdac, DAC_CHANNEL_2, (uint32_t*)Ysine, size, DAC_ALIGN_12B_R);
    for (int i = 0; i < Nsize; i++)
    {
        int valorARR = calcularARR(tone[i]);   // frecuencia → ARR
        int dur = duration[i];                 // duración de la nota
        int pauseBetweenTones = 0;

        if (pause != NULL)
        {
            pauseBetweenTones = pause[i] - duration[i];
            if (pauseBetweenTones < 0) pauseBetweenTones = 0;
        }

        // Configurar frecuencia (Timer que dispara el DAC)
        TIM6->ARR = valorARR;
        TIM6->CNT = 0;

        HAL_Delay(dur);   // duración de la nota

        // Apagar tono (silencio)
        noTone();

        HAL_Delay(pauseBetweenTones); // pausa entre notas
    }

    HAL_DAC_Stop_DMA(&hdac, DAC_CHANNEL_2);
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
  MX_USART3_UART_Init();
  MX_USART6_UART_Init();
  /* USER CODE BEGIN 2 */

  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
  HAL_UART_Receive_DMA(&huart2, &rxbuffer, 1);
  HAL_UART_Receive_DMA(&huart6, &rx3buffer, 1);
  generarSin();

  // Inicializar DAC
  HAL_DAC_Init(&hdac);
  HAL_TIM_Base_Start(&htim6);

  // Inicializar DAC elgiendo el canal
  HAL_DAC_Start_DMA(&hdac, DAC_CHANNEL_2, (uint32_t*)Ysine, size, DAC_ALIGN_12B_R);



  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  playTone(silent_notes, silent_durations, NULL, (sizeof(silent_notes)/sizeof(silent_notes[0])));
	  // verifico que me envia la nucleo del juego para saber cual musica tengo que poner.
	  // contamos con musica para menu, juego y ganador.
	  switch(rx3buffer)
	  		  {
	  			  case '1':
	  				  //Menu
	  				  playTone(mk64_intro_notes, mk64_intro_durations, NULL, (sizeof(mk64_intro_notes)/sizeof(mk64_intro_notes[0])));

	  				  break;

	  			  case '2':
	  				  //Carrera
	  				  playTone(mk_notes, mk_durations, NULL, (sizeof(mk_notes)/sizeof(mk_notes[0])));

	  				  break;

	  			  case '3':
	  				  //ganador!
	  				  playTone(win_notes, win_durations, NULL, (sizeof(win_notes)/sizeof(win_notes[0])));
	  				  break;
	  			  default:
	  				  //strcpy(dummy, "Opcion invalida\r\n");
	  				  //HAL_UART_Transmit(&huart2, dummy, strlen(dummy), 100);
	  				  noTone();
	  				  break;
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

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
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
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
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
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
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
  htim6.Init.Prescaler = 1-1;
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
  huart2.Init.OverSampling = UART_OVERSAMPLING_8;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_8;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * @brief USART6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART6_UART_Init(void)
{

  /* USER CODE BEGIN USART6_Init 0 */

  /* USER CODE END USART6_Init 0 */

  /* USER CODE BEGIN USART6_Init 1 */

  /* USER CODE END USART6_Init 1 */
  huart6.Instance = USART6;
  huart6.Init.BaudRate = 115200;
  huart6.Init.WordLength = UART_WORDLENGTH_8B;
  huart6.Init.StopBits = UART_STOPBITS_1;
  huart6.Init.Parity = UART_PARITY_NONE;
  huart6.Init.Mode = UART_MODE_TX_RX;
  huart6.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart6.Init.OverSampling = UART_OVERSAMPLING_8;
  if (HAL_UART_Init(&huart6) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART6_Init 2 */

  /* USER CODE END USART6_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();
  __HAL_RCC_DMA2_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream1_IRQn);
  /* DMA1_Stream5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);
  /* DMA1_Stream6_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream6_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream6_IRQn);
  /* DMA2_Stream1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream1_IRQn);

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

// Interrupcion de RX con DMA
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart == &huart6)
    {
        flag = 1;
        HAL_UART_Receive_DMA(&huart6, &rx3buffer, 1);
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
