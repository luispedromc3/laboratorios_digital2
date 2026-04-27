/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
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
#include "ili9341.h"
#include "bitmaps.h"
#include "NOTES_PSC.h"
#include <string.h>
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
SPI_HandleTypeDef hspi1;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;

UART_HandleTypeDef huart5;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;
UART_HandleTypeDef huart6;
DMA_HandleTypeDef hdma_usart3_rx;
DMA_HandleTypeDef hdma_usart6_rx;

/* USER CODE BEGIN PV */
extern const uint16_t fondo[];
extern const uint16_t pista5[];

uint8_t rx_control1[1];
uint8_t rx_control2[1];

uint16_t dir_x_car1 = 155;
uint16_t dir_y_car1 = 208;
volatile uint16_t new_x_car1, new_y_car1;
volatile uint8_t move_ready1 = 0;
volatile char cmd1 = 0;
uint8_t indx1 = 1;

uint16_t dir_x_car2 = 155;
uint16_t dir_y_car2 = 192;
volatile uint16_t new_x_car2, new_y_car2;
volatile uint8_t move_ready2 = 0;
volatile char cmd2 = 0;
uint8_t indx2 = 1;



uint8_t STEP_SIZE = 8;

//uint16_t note_index = 0;
//uint32_t last_note_time = 0;

#define TOTAL_NOTES (sizeof(mk_notes)/sizeof(uint16_t))

// Para usar con timer en lugar de delay
volatile uint16_t note_index = 0;
volatile uint32_t note_time = 0;
volatile uint8_t note_on = 1;


// Variables para meta
#define FINISH_X 150
#define FINISH_Y1 180
#define FINISH_Y2 220
uint16_t prev_x_car1, prev_x_car2;

uint8_t laps_car1 = 0;
uint8_t laps_car2 = 0;

uint8_t crossed_car1 = 0;
uint8_t crossed_car2 = 0;

// Check point
#define CP1_X 131
#define CP1_Y1 10
#define CP1_Y2 70

uint8_t cp_car2 = 0;
uint8_t cp_car1 = 0;

// Canciones PWM
// ------------------------ Cancion para Pwm ------------------------------
uint16_t mk_notes[] = {
    PSC_FS4, PSC_A4, PSC_B4,
    PSC_D5, PSC_CS5, PSC_B4,
    PSC_A4, PSC_FS4,

    PSC_D4, PSC_FS4, PSC_A4,
    PSC_B4, PSC_A4, PSC_FS4,
    PSC_E4, PSC_D4,

    PSC_FS4, PSC_A4, PSC_B4,
    PSC_D5, PSC_CS5, PSC_B4,
    PSC_A4, PSC_FS4,

    PSC_E4, PSC_FS4, PSC_G4,
    PSC_A4, PSC_B4, PSC_A4,
    PSC_FS4,

	PSC_A4, PSC_B4, PSC_C5,
	PSC_B4, PSC_A4, PSC_G4,
	PSC_FS4, PSC_E4,

	PSC_D4, PSC_E4, PSC_FS4,
	PSC_A4, PSC_G4, PSC_FS4,
	PSC_E4, PSC_D4
};

uint16_t mk_durations[] = {
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


// Tile set
#define TILE_SIZE 16
#define MAP_W 20
#define MAP_H 15

uint8_t collision_map[MAP_H][MAP_W];

// ================================================================================


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_SPI1_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_USART6_UART_Init(void);
static void MX_TIM1_Init(void);
static void MX_TIM2_Init(void);
static void MX_UART5_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

int check_collision(uint16_t x1, uint16_t y1,
                    uint16_t x2, uint16_t y2)
{
    return (x1 < x2 + 16 &&
            x1 + 16 > x2 &&
            y1 < y2 + 16 &&
            y1 + 16 > y2);
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
  MX_USART2_UART_Init();
  MX_SPI1_Init();
  MX_USART3_UART_Init();
  MX_USART6_UART_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_UART5_Init();
  /* USER CODE BEGIN 2 */
	LCD_Init();
	HAL_TIM_Base_Start_IT(&htim2);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);

	LCD_Clear(0x00);

	LCD_Bitmap(0, 0, 320, 240, pista5);
//	FillRect(0, 0, 319, 206, 0x421b);
//
	//LCD_Print("Hola Mundo", 20, 100, 2, 0x001F, 0xCAB9);

	//LCD_Sprite(int x, int y, int width, int height, unsigned char bitmap[],int columns, int index, char flip, char offset);
	//LCD_Sprite(60,100,32,32,pesaSprite,4,3,0,1);

	//LCD_Bitmap(unsigned int x, unsigned int y, unsigned int width, unsigned int height, unsigned char bitmap[]);
	//LCD_Bitmap(0, 0, 320, 240, fondo);

	//for (int x = 0; x < 319; x++) {
	//	LCD_Bitmap(x, 116, 15, 15, tile);
	//	LCD_Bitmap(x, 68, 15, 15, tile);
	//	LCD_Bitmap(x, 207, 15, 15, tile);
	//	LCD_Bitmap(x, 223, 15, 15, tile);
	//	x += 15;
	//}

	//LCD_Bitmap(50,10, 22, 22, car1);

	//LCD_BitmapTransparent(50, 12+22, 22, 22, car1, 0xbdf7);
	LCD_Sprite(dir_x_car2, dir_y_car2, 16, 16, cars1, 4, 1, 0, 0);
	LCD_Sprite(dir_x_car1, dir_y_car1, 16, 16, cars2, 4, 1, 0, 0);

	HAL_UART_Receive_DMA(&huart3, rx_control1, sizeof(rx_control1));
	HAL_UART_Receive_DMA(&huart6, rx_control2, sizeof(rx_control2));



	// Crear mapa de colisiones
	for(int ty = 0; ty < MAP_H; ty++){
		for(int tx = 0; tx < MAP_W; tx++){

			int px = tx * TILE_SIZE;
			int py = ty * TILE_SIZE;

			int gray_pixels = 0;

			for(int i=0; i<TILE_SIZE; i++){
				for(int j=0; j<TILE_SIZE; j++){

					uint16_t color = pista5[(py+i)*320 + (px+j)];

					if(color == 0x9cf3)// SOLO gris es válido
						gray_pixels++;
				}
			}

			// umbral
			if(gray_pixels > (TILE_SIZE * TILE_SIZE * 8 / 10))
				collision_map[ty][tx] = 0; // libre
			else
				collision_map[ty][tx] = 1; // bloqueado
		}
	}

	char data = '1';
	HAL_UART_Transmit(&huart5, &data, 1, 100);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1) {
		// Cancion de menu
		 //play_melody(mk_notes, mk_durations, sizeof(mk_notes)/sizeof(uint16_t));
		//play_melody_non_blocking();
		// =========================
		// CONTROL CARRO 2
		// =========================
		if(move_ready2){

		    // guardar posición anterior
		    uint16_t prev_x = dir_x_car2;
		    uint16_t prev_y = dir_y_car2;
		    prev_x_car2 = dir_x_car2;
		    // =========================
		    // MOVIMIENTO EN X
		    // =========================
		    int left   = new_x_car2 / 16;
		    int right  = (new_x_car2 + 15) / 16;
		    int top    = dir_y_car2 / 16;
		    int bottom = (dir_y_car2 + 15) / 16;

		    if(collision_map[top][left] == 0 &&
		       collision_map[top][right] == 0 &&
		       collision_map[bottom][left] == 0 &&
		       collision_map[bottom][right] == 0 &&
		       !check_collision(new_x_car2, dir_y_car2, dir_x_car1, dir_y_car1))
		    {
		        dir_x_car2 = new_x_car2;
		    }

		    // =========================
		    // MOVIMIENTO EN Y
		    // =========================
		    left   = dir_x_car2 / 16;
		    right  = (dir_x_car2 + 15) / 16;
		    top    = new_y_car2 / 16;
		    bottom = (new_y_car2 + 15) / 16;

		    if(collision_map[top][left] == 0 &&
		       collision_map[top][right] == 0 &&
		       collision_map[bottom][left] == 0 &&
		       collision_map[bottom][right] == 0 &&
		       !check_collision(dir_x_car2, new_y_car2, dir_x_car1, dir_y_car1))
		    {

		        dir_y_car2 = new_y_car2;
		    }

		    // =========================
		    // BORRADO
		    // =========================
		    int dx = dir_x_car2 - prev_x;
		    int dy = dir_y_car2 - prev_y;

		    if(dx > 0){ // derecha
		        for(int i = 0; i < STEP_SIZE; i++){
		            V_line(prev_x + i, prev_y, 16, 0x9cf3);
		        }
		        indx2 = 1;
		    }
		    else if(dx < 0){ // izquierda
		        for(int i = 0; i < STEP_SIZE; i++){
		            V_line(prev_x + 15 - i, prev_y, 16, 0x9cf3);
		        }
		        indx2 = 2;
		    }

		    if(dy > 0){ // abajo
		        for(int i = 0; i < STEP_SIZE; i++){
		            H_line(prev_x, prev_y + i, 16, 0x9cf3);
		        }
		        indx2 = 3;
		    }
		    else if(dy < 0){ // arriba
		        for(int i = 0; i < STEP_SIZE; i++){
		            H_line(prev_x, prev_y + 15 - i, 16, 0x9cf3);
		        }
		        indx2 = 0;
		    }

		    // =========================
		    // DIBUJO
		    // =========================
		    LCD_Sprite(dir_x_car2, dir_y_car2, 16, 16, cars1, 4, indx2, 0, 0);

		    move_ready2 = 0;
		}



		// Control 1
		if(move_ready1){

		    // 🔹 guardar posición anterior
		    uint16_t prev_x = dir_x_car1;
		    uint16_t prev_y = dir_y_car1;
		    prev_x_car1 = dir_x_car1;
		    // =========================
		    // MOVIMIENTO EN X
		    // =========================
		    int left   = new_x_car1 / 16;
		    int right  = (new_x_car1 + 15) / 16;
		    int top    = dir_y_car1 / 16;
		    int bottom = (dir_y_car1 + 15) / 16;

		    if(collision_map[top][left] == 0 &&
		       collision_map[top][right] == 0 &&
		       collision_map[bottom][left] == 0 &&
		       collision_map[bottom][right] == 0 &&
		       !check_collision(new_x_car1, dir_y_car1, dir_x_car2, dir_y_car2))
		    {
		        dir_x_car1 = new_x_car1;
		    }

		    // =========================
		    // MOVIMIENTO EN Y
		    // =========================
		    left   = dir_x_car1 / 16;
		    right  = (dir_x_car1 + 15) / 16;
		    top    = new_y_car1 / 16;
		    bottom = (new_y_car1 + 15) / 16;

		    if(collision_map[top][left] == 0 &&
		       collision_map[top][right] == 0 &&
		       collision_map[bottom][left] == 0 &&
		       collision_map[bottom][right] == 0 &&
		       !check_collision(dir_x_car1, new_y_car1, dir_x_car2, dir_y_car2))
		    {
		        dir_y_car1 = new_y_car1;
		    }

		    // =========================
		    // BORRADO del tamaño de STEP_SIZE
		    // =========================
		    int dx = dir_x_car1 - prev_x;
		    int dy = dir_y_car1 - prev_y;

		    // borrar en X
		    if(dx > 0){ // derecha
		        for(int i = 0; i < STEP_SIZE; i++){
		            V_line(prev_x + i, prev_y, 16, 0x9cf3);
		        }
		        indx1 = 1;
		    }
		    else if(dx < 0){ // izquierda
		        for(int i = 0; i < STEP_SIZE; i++){
		            V_line(prev_x + 15 - i, prev_y, 16, 0x9cf3);
		        }
		        indx1 = 2;
		    }

		    // borrar en Y
		    if(dy > 0){ // abajo
		        for(int i = 0; i < STEP_SIZE; i++){
		            H_line(prev_x, prev_y + i, 16, 0x9cf3);
		        }
		        indx1 = 3;
		    }
		    else if(dy < 0){ // arriba
		        for(int i = 0; i < STEP_SIZE; i++){
		            H_line(prev_x, prev_y + 15 - i, 16, 0x9cf3);
		        }
		        indx1 = 0;
		    }

		    // =========================
		    // DIBUJO
		    // =========================
		    LCD_Sprite(dir_x_car1, dir_y_car1, 16, 16, cars2, 4, indx1, 0, 0);

		    move_ready1 = 0;
		}


		// Logica de meta para carro 2
		if(cp_car2 == 0 &&
		   prev_x_car2 > CP1_X && dir_x_car2 <= CP1_X &&
		   dir_y_car2 > CP1_Y1 && dir_y_car2 < CP1_Y2)
		{
		    cp_car2 = 1;
		}

		if(cp_car2 == 1 &&
		   prev_x_car2 < FINISH_X && dir_x_car2 >= FINISH_X &&
		   dir_y_car2 > FINISH_Y1 && dir_y_car2 < FINISH_Y2)
		{
		    if(!crossed_car2){
		        laps_car2++;
		        crossed_car2 = 1;
		        cp_car2 = 0;
		    }
		}
		else{
		    crossed_car2 = 0;
		}

		// Lógica meta para carro
		if(cp_car1 == 0 &&
		   prev_x_car1 > CP1_X && dir_x_car1 <= CP1_X &&
		   dir_y_car1 > CP1_Y1 && dir_y_car1 < CP1_Y2)
		{
		    cp_car1 = 1;
		}

		if(cp_car1 == 1 &&
		   prev_x_car1 < FINISH_X && dir_x_car1 >= FINISH_X &&
		   dir_y_car1 > FINISH_Y1 && dir_y_car1 < FINISH_Y2)
		{
		    if(!crossed_car1){
		        laps_car1++;
		        crossed_car1 = 1;
		        cp_car1 = 0;
		    }
		}
		else{
		    crossed_car1 = 0;
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
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

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
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 83;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 999;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief UART5 Initialization Function
  * @param None
  * @retval None
  */
static void MX_UART5_Init(void)
{

  /* USER CODE BEGIN UART5_Init 0 */

  /* USER CODE END UART5_Init 0 */

  /* USER CODE BEGIN UART5_Init 1 */

  /* USER CODE END UART5_Init 1 */
  huart5.Instance = UART5;
  huart5.Init.BaudRate = 115200;
  huart5.Init.WordLength = UART_WORDLENGTH_8B;
  huart5.Init.StopBits = UART_STOPBITS_1;
  huart5.Init.Parity = UART_PARITY_NONE;
  huart5.Init.Mode = UART_MODE_TX_RX;
  huart5.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart5.Init.OverSampling = UART_OVERSAMPLING_8;
  if (HAL_UART_Init(&huart5) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN UART5_Init 2 */

  /* USER CODE END UART5_Init 2 */

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
  huart3.Init.Mode = UART_MODE_RX;
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
  huart6.Init.Mode = UART_MODE_RX;
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
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LCD_RESET_GPIO_Port, LCD_RESET_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LCD_CS_Pin|SD_CS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LCD_RESET_Pin */
  GPIO_InitStruct.Pin = LCD_RESET_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(LCD_RESET_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LCD_DC_Pin */
  GPIO_InitStruct.Pin = LCD_DC_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(LCD_DC_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LCD_CS_Pin SD_CS_Pin */
  GPIO_InitStruct.Pin = LCD_CS_Pin|SD_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */
  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
// Interrupciones

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart == &huart3)
	{
	    cmd1 = rx_control1[0];

	    new_x_car1 = dir_x_car1;
	    new_y_car1 = dir_y_car1;

	    switch(cmd1){
	        case 'r': new_x_car1 += STEP_SIZE; break;
	        case 'l': new_x_car1 -= STEP_SIZE; break;
	        case 'u': new_y_car1 -= STEP_SIZE; break;
	        case 'd': new_y_car1 += STEP_SIZE; break;
	    }

	    move_ready1 = 1;

	    HAL_UART_Receive_DMA(&huart3, rx_control1, 1);
	}
    if (huart == &huart6)
        {
    	if (huart == &huart6)
    	{
    	    cmd2 = rx_control2[0];

    	    new_x_car2 = dir_x_car2;
    	    new_y_car2 = dir_y_car2;

    	    switch(cmd2){
    	        case 'r': new_x_car2 += STEP_SIZE; break;
    	        case 'l': new_x_car2 -= STEP_SIZE; break;
    	        case 'u': new_y_car2 -= STEP_SIZE; break;
    	        case 'd': new_y_car2 += STEP_SIZE; break;
    	    }

    	    move_ready2 = 1;

    	    HAL_UART_Receive_DMA(&huart6, rx_control2, 1);
    	}
            HAL_UART_Receive_DMA(&huart6, rx_control2, 1);
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
	while (1) {
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
