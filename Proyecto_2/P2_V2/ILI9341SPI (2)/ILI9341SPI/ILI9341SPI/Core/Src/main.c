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
#include "fatfs.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "fatfs_sd.h"
#include "string.h"
#include "stdio.h"

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

FATFS fs;
FATFS *pfs;
FIL fil;
FRESULT fres;
DWORD fre_clust;
uint32_t totalSpace, freeSpace;
char buffer[100];
char data;
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi1;
SPI_HandleTypeDef hspi2;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim6;

UART_HandleTypeDef huart5;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;
UART_HandleTypeDef huart6;
DMA_HandleTypeDef hdma_usart3_rx;
DMA_HandleTypeDef hdma_usart6_rx;

/* USER CODE BEGIN PV */
extern const uint16_t fondo[];
extern const uint16_t pista5[];
extern const uint16_t menu[];
uint8_t estado_juego = 0;
uint8_t flag_velocidad = 0;
uint8_t flag_mapa_carrera = 0;
uint8_t flag_menu = 0;
uint8_t rx_control1[1];
uint8_t rx_control2[1];
uint8_t flag_jugando = 0;
uint8_t flag_ganador = 0;
uint8_t contador = 0;
uint16_t contador_seg = 0;
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
uint8_t VELOCIDAD = 0;

// declaracion variables para imprimir pantalla guardada en SD
#define BLOCK_ROWS 16
uint16_t img_block[320 * BLOCK_ROWS];



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

// Variables para el boost, misma posicion que los checkpoints
uint8_t boost_active = 0;
uint8_t boost_visible = 0;
uint16_t boost_timer = 0;      // duración visible
uint16_t boost_duration = 0;   // duración del efecto

uint8_t boost_car1 = 0;
uint8_t boost_car2 = 0;

uint16_t boost_time_car1 = 0;
uint16_t boost_time_car2 = 0;

// Posición del boost
uint16_t boost_x = 150;
uint16_t boost_y = 30;

// STEP independientes
uint8_t STEP_SIZE_CAR1 = 8;
uint8_t STEP_SIZE_CAR2 = 8;

uint8_t cp1_event_car1 = 0;
uint8_t cp1_event_car2 = 0;

uint8_t boost_spawned = 0;

//variables para guardar highscore
// Highscore
uint16_t highscore0_seg = 9999;  // valor inicial
uint8_t  highscore0_dec = 0;
uint16_t highscore1_seg = 9999;  // valor inicial
uint8_t  highscore1_dec = 0;
uint16_t highscore2_seg = 9999;  // valor inicial
uint8_t  highscore2_dec = 0;
uint8_t  flag_highscore = 0;    // para dibujar la pantalla solo una vez

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
static void MX_SPI2_Init(void);
static void MX_TIM6_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void transmit_uart(char *string){
	uint8_t len = strlen(string);
	HAL_UART_Transmit(&huart2, (uint8_t*) string, len,200);
	//funcion que usamos para imprimir estado de SD por serial
}
void guardar_highscore(uint16_t seg, uint8_t dec)
{
	//funcion para guardar el highscore dependiendo del modo de velocidad en el que estemos.
	// velocidad alta 1 velocdad media 0 velocidad baja 2
    FATFS fs_hs;
    FIL   fil_hs;
    char  linea[30];

    if(f_mount(&fs_hs, "/", 0) != FR_OK){
        transmit_uart("HS: error montando SD\n");
        return;
    }
    transmit_uart("HS: SD montada\n");
    if (VELOCIDAD == 0){
    	if(f_open(&fil_hs, "highscore0.txt", FA_WRITE | FA_CREATE_ALWAYS) != FR_OK){
			transmit_uart("HS: error abriendo archivo\n");
			f_mount(NULL, "", 1);
			return;
		}
		transmit_uart("HS: archivo abierto\n");

		sprintf(linea, "%u,%u\n", seg, dec);  // coma como separador
		f_puts(linea, &fil_hs);
		transmit_uart("HS: dato escrito\n");

		f_close(&fil_hs);
		f_mount(NULL, "", 1);
		transmit_uart("HS: guardado vel menor exitoso\n");
    }else if (VELOCIDAD == 1){
    	if(f_open(&fil_hs, "highscore1.txt", FA_WRITE | FA_CREATE_ALWAYS) != FR_OK){
			transmit_uart("HS: error abriendo archivo\n");
			f_mount(NULL, "", 1);
			return;
		}
		transmit_uart("HS: archivo abierto\n");

		sprintf(linea, "%u,%u\n", seg, dec);  // coma como separador
		f_puts(linea, &fil_hs);
		transmit_uart("HS: dato escrito\n");

		f_close(&fil_hs);
		f_mount(NULL, "", 1);
		transmit_uart("HS: guardado vel alta exitoso\n");
    }else if (VELOCIDAD == 2){
    	if(f_open(&fil_hs, "highscore2.txt", FA_WRITE | FA_CREATE_ALWAYS) != FR_OK){
			transmit_uart("HS: error abriendo archivo\n");
			f_mount(NULL, "", 1);
			return;
		}
		transmit_uart("HS: archivo abierto\n");

		sprintf(linea, "%u,%u\n", seg, dec);  // coma como separador
		f_puts(linea, &fil_hs);
		transmit_uart("HS: dato escrito\n");

		f_close(&fil_hs);
		f_mount(NULL, "", 1);
		transmit_uart("HS: guardado vel lenta exitoso\n");
    }

}

void cargar_highscore(void)
{
    FATFS fs_hs;
    FIL   fil_hs;
    char  linea[30];
    const char* nombre;
    uint16_t *seg_ptr;
    uint8_t  *dec_ptr;

    // Seleccionar archivo y variables según velocidad. Dependiendo de ello mostramos el
    // valor del highscore adecuado a la velocidad que eligio
    if(VELOCIDAD == 0){
        nombre  = "highscore0.txt";
        seg_ptr = &highscore0_seg;
        dec_ptr = &highscore0_dec;
    } else if(VELOCIDAD == 1){
        nombre  = "highscore1.txt";
        seg_ptr = &highscore1_seg;
        dec_ptr = &highscore1_dec;
    } else {
        nombre  = "highscore2.txt";
        seg_ptr = &highscore2_seg;
        dec_ptr = &highscore2_dec;
    }

    if(f_mount(&fs_hs, "/", 0) != FR_OK){
        transmit_uart("HS: error montando SD\n");
        return;
    }
    transmit_uart("HS: SD montada\n");

    if(f_open(&fil_hs, nombre, FA_READ) != FR_OK){
        transmit_uart("HS: archivo no existe, usando default\n");
        f_mount(NULL, "", 1);
        return;
    }
    transmit_uart("HS: archivo abierto\n");

    if(f_gets(linea, sizeof(linea), &fil_hs)){
        uint16_t s = 0, d = 0;
        sscanf(linea, "%hu,%hu", &s, &d);
        *seg_ptr = s;
        *dec_ptr = (uint8_t)d;

        char msg[50];
        sprintf(msg, "HS: record cargado = %u.%u s\n", *seg_ptr, *dec_ptr);
        transmit_uart(msg);
    } else {
        transmit_uart("HS: error leyendo archivo\n");
    }

    f_close(&fil_hs);
    f_mount(NULL, "", 1);
    transmit_uart("HS: SD desmontada\n");
}

uint8_t load_and_draw_image_from_sd(const char* filename) {
	// funcion para cargar imagen desde SD
	//
    FATFS fs_local;
    FIL fil_local;
    FRESULT res;
    UINT br;
    char msg[60];
    char token[10];
    uint8_t tok_idx = 0;

    #define READ_CHUNK 128
    char chunk[READ_CHUNK + 1];

    uint32_t idx = 0;
    uint32_t total = 0;
    uint32_t total_pixels = 320 * 240;
    int current_row_block = 0;

    res = f_mount(&fs_local, "/", 0);
    if(res != FR_OK){
        transmit_uart("IMG: error montando SD\n");
        return 0;
    }
    transmit_uart("IMG: SD montada\n");

    res = f_open(&fil_local, filename, FA_READ);
    if(res != FR_OK){
        sprintf(msg, "IMG: error abriendo archivo: %d\n", res);
        transmit_uart(msg);
        f_mount(NULL, "", 1);
        return 0;
    }
    transmit_uart("IMG: dibujando...\n");

    while(total < total_pixels){
        res = f_read(&fil_local, chunk, READ_CHUNK, &br);
        if(br == 0) break;
        chunk[br] = '\0';

        for(UINT i = 0; i < br && total < total_pixels; i++){
            char c = chunk[i];

            if(c == ' ' || c == '\n' || c == '\r') continue;

            if(c == ','){
            	// el formato que guardamos la imagen es de numero seguido de comas como los usados
            	// cuando se guarda en RAM.
                if(tok_idx > 0){
                    token[tok_idx] = '\0';
                    img_block[idx++] = (uint16_t)strtol(token, NULL, 16);
                    tok_idx = 0;
                    total++;

                    if(idx >= 320 * BLOCK_ROWS){
                        LCD_Bitmap(0, current_row_block * BLOCK_ROWS, 320, BLOCK_ROWS, img_block);
                        current_row_block++;
                        idx = 0;
                    }
                }
            } else {
                if(tok_idx < sizeof(token) - 1)
                    token[tok_idx++] = c;
            }
        }
    }

    // Procesar último token si no termina en coma
    if(tok_idx > 0 && total < total_pixels){
        token[tok_idx] = '\0';
        img_block[idx++] = (uint16_t)strtol(token, NULL, 16);
        total++;
    }

    // Dibujar último bloque incompleto
    if(idx > 0){
        LCD_Bitmap(0, current_row_block * BLOCK_ROWS, 320, BLOCK_ROWS, img_block);
    }

    f_close(&fil_local);
    f_mount(NULL, "", 1);

    sprintf(msg, "IMG: total pixeles = %lu\n", total);
    transmit_uart(msg);
    return 1;
}
void restartRace(){
	// funcion para empezar de cero la carrera una vez hay ganador.
	move_ready1 = 0;
	move_ready2 = 0;
	cmd1 = 0;
	cmd2 = 0;

	new_x_car1 = dir_x_car1 = 155;
	new_y_car1 = dir_y_car1 = 208;

	new_x_car2 = dir_x_car2 = 155;
	new_y_car2 = dir_y_car2 = 192;

	boost_spawned = 0;

	cp_car1 = 0;
	cp_car2 = 0;
	laps_car2 = 0;
	laps_car1 = 0;
	flag_ganador = 0;
	contador = 0;
	contador_seg = 0;
	//LCD_Bitmap(0, 0, 320, 240, pista5);
	//LCD_Sprite(dir_x_car2, dir_y_car2, 16, 16, cars1, 4, 1, 0, 0);
	//LCD_Sprite(dir_x_car1, dir_y_car1, 16, 16, cars2, 4, 1, 0, 0);

	//musica de juego
	//enviamos el 2 al otro nucleo para indicarle que deseamos la musica de carrera.
	data = '2';
	HAL_UART_Transmit(&huart5, &data, 1, 100);
}



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
  MX_SPI2_Init();
  MX_FATFS_Init();
  MX_TIM6_Init();
  /* USER CODE BEGIN 2 */
	LCD_Init();
	HAL_TIM_Base_Start_IT(&htim2);
	HAL_TIM_Base_Start_IT(&htim6);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	HAL_UART_Receive_DMA(&huart3, rx_control1, sizeof(rx_control1));
	HAL_UART_Receive_DMA(&huart6, rx_control2, sizeof(rx_control2));
	//tiempo para que se inicialice la sd
	HAL_Delay(1000);
	//montamos el sistema
	// comenzamos haciendo unas pruebas a la SD para verificar que este funcionando correctamente
	  	fres = f_mount(&fs, "/", 0);
	  	sprintf(buffer, "f_mount result: %d\r\n", fres);
	  	transmit_uart(buffer);

	  	if(fres == FR_OK){
	  		transmit_uart("Micro SD card is mounted successfully\n");
	  	}
	  	else if(fres != FR_OK){

	  		transmit_uart("Micro SD card's mount error\n");
	  	}

	  	fres = f_open(&fil, "prueba.txt", FA_OPEN_ALWAYS | FA_READ | FA_WRITE);

	  	if(fres == FR_OK){
	  	    transmit_uart("File opened successfully\n");
	  	}
	  	else{
	  	    sprintf(buffer, "f_open error: %d\n", fres);
	  	    transmit_uart(buffer);
	  	}

	  	//escribir en el archivo abierto
	  	for(uint8_t i = 0; i < 5; i++){
	  		f_puts("Hola mundo digital 2\n", &fil);
	  	}

	  	//cerramo archivo abierto
	  	fres = f_close(&fil);
	  	sprintf(buffer, "f_mount result: %d\r\n", fres);
	  	transmit_uart(buffer);
	  	if (fres == FR_OK){
	  		transmit_uart("The file is closed\n");
	  	}
	  	else if(fres != FR_OK){
	  		transmit_uart("The file  was not file closed\n");
	  	}

	  	//leemos archivo
	  	fres = f_open(&fil, "prueba.txt", FA_READ);
	  	sprintf(buffer, "f_mount result: %d\r\n", fres);
	  	transmit_uart(buffer);
	  	if(fres == FR_OK){
	  	  		transmit_uart("File opened for reading\n");
	  	  	}
	  	  	else if(fres != FR_OK){
	  	  		transmit_uart("File was not opened for reading\n");
	  	  	}
	  	//leemos el archivo
	  	while (f_gets(buffer, sizeof(buffer),&fil)){
	  		char mRd[100];
	  		sprintf(mRd, "%s", buffer);
	  		transmit_uart(mRd);
	  	}
	  	fres = f_close(&fil);
	  	sprintf(buffer, "f_mount result: %d\r\n", fres);
	  	transmit_uart(buffer);
	  	  	if (fres == FR_OK){
	  	  		transmit_uart("The file is closed\n");
	  	  	}
	  	  	else if(fres != FR_OK){
	  	  		transmit_uart("The was not file closed\n");
	  	  	}
	  	  	fres = f_mount(NULL, "", 1);
	  	  sprintf(buffer, "f_mount result: %d\r\n", fres);
	  	  transmit_uart(buffer);
	  	  	if(fres == FR_OK){
	  	  		transmit_uart("The microSD is unmounted\n");
	  	  	}
	  	  	else if(fres != FR_OK){
	  	  		transmit_uart("The microSD was not unmounted\n");
	  	  	}


	//LCD_Clear(0x00);
	//load_and_draw_image_from_sd("imagenmenu.txt");


	HAL_UART_Receive_DMA(&huart3, rx_control1, sizeof(rx_control1));
	HAL_UART_Receive_DMA(&huart6, rx_control2, sizeof(rx_control2));
	//enviamos 1 para musica de menu

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

	cargar_highscore(); // aca cargo el valor del highscore desde la SD
	// aca imprimo todos los archivos dentro de la sd para estar seguro de los nombres
	DIR dir;
	FILINFO fno;
	FATFS fs_temp;

	f_mount(&fs_temp, "/", 0);
	f_opendir(&dir, "/");
	while(f_readdir(&dir, &fno) == FR_OK && fno.fname[0] != 0){
	    transmit_uart(fno.fname);
	    transmit_uart("\n");
	}
	f_closedir(&dir);
	f_mount(NULL, "", 1);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1)
	{
	    switch(estado_juego)
	    {
	        case 0: // MENU
	            if(!flag_menu){
	                LCD_Clear(0x00);
	                //LCD_Bitmap(0, 0, 320, 240, menu);
	                load_and_draw_image_from_sd("imagenmenu.txt.txt");
	                flag_menu = 1;
	            }
	            data = '1';
	            HAL_UART_Transmit(&huart5, &data, 1, 100);
	            flag_highscore = 0;
	            if(rx_control1[0] == 'x' || rx_control2[0] == 'x'){
	                rx_control1[0] = 0;
	                rx_control2[0] = 0;
	                flag_menu = 0;
	                flag_mapa_carrera = 0; // forzar redibujo de pista
	                restartRace();
	                estado_juego = 1;
	            }
	            if(rx_control1[0] == 'o' || rx_control2[0] == 'o'){
	                rx_control1[0] = 0;
	                rx_control2[0] = 0;
	                flag_menu = 0;
	                estado_juego = 2;
	            }
	            if(rx_control1[0] == 't' || rx_control2[0] == 't'){
	                rx_control1[0] = 0;
	                rx_control2[0] = 0;
	                flag_menu = 0;
	                flag_velocidad = 0;
	                estado_juego = 4;
	            }
	            break;

	        case 1: // JUEGO
	            data = '2';
	            HAL_UART_Transmit(&huart5, &data, 1, 100);

	            // Dibujar pista solo la primera vez
	            if(!flag_mapa_carrera){
	                LCD_Bitmap(0, 0, 320, 240, pista5);
	                LCD_Sprite(dir_x_car2, dir_y_car2, 16, 16, cars1, 4, 1, 0, 0);
	                LCD_Sprite(dir_x_car1, dir_y_car1, 16, 16, cars2, 4, 1, 0, 0);

	                LCD_Print("3", 140, 110, 2, 0xF800, 0x9cf3);
	                LCD_Print("3", 141, 110, 2, 0xF800, 0x9cf3);
	                LCD_Print("3", 142, 110, 2, 0xF800, 0x9cf3);
					HAL_Delay(1000);
					LCD_Print("3", 140, 110, 2, 0x9cf3, 0x9cf3); // borrar "3"
					LCD_Print("3", 141, 110, 2, 0x9cf3, 0x9cf3);
					LCD_Print("3", 142, 110, 2, 0x9cf3, 0x9cf3);

					LCD_Print("2", 140, 110, 2, 0xFFE0, 0x9cf3);
					LCD_Print("2", 141, 110, 2, 0xFFE0, 0x9cf3);
					LCD_Print("2", 142, 110, 2, 0xFFE0, 0x9cf3);
					HAL_Delay(1000);
					LCD_Print("2", 140, 110, 2, 0x9cf3, 0x9cf3); // borrar "2"
					LCD_Print("2", 141, 110, 2, 0x9cf3, 0x9cf3); // borrar "2"
					LCD_Print("2", 142, 110, 2, 0x9cf3, 0x9cf3); // borrar "2"

					LCD_Print("1", 140, 110, 2, 0x07E0, 0x9cf3);
					LCD_Print("1", 141, 110, 2, 0x07E0, 0x9cf3);
					LCD_Print("1", 142, 110, 2, 0x07E0, 0x9cf3);
					HAL_Delay(1000);
					LCD_Print("1", 140, 110, 2, 0x9cf3, 0x9cf3); // borrar "1"
					LCD_Print("1", 141, 110, 2, 0x9cf3, 0x9cf3); // borrar "1"
					LCD_Print("1", 142, 110, 2, 0x9cf3, 0x9cf3); // borrar "1"
	                flag_mapa_carrera = 1;
	                flag_jugando = 1;
	            }
	            // ── Carro 2 ──
	            if(move_ready2){
	                uint16_t prev_x = dir_x_car2;
	                uint16_t prev_y = dir_y_car2;
	                prev_x_car2 = dir_x_car2;

	                int left   = new_x_car2 / 16;
	                int right  = (new_x_car2 + 15) / 16;
	                int top    = dir_y_car2 / 16;
	                int bottom = (dir_y_car2 + 15) / 16;

	                if(collision_map[top][left] == 0 && collision_map[top][right] == 0 &&
	                   collision_map[bottom][left] == 0 && collision_map[bottom][right] == 0 &&
	                   !check_collision(new_x_car2, dir_y_car2, dir_x_car1, dir_y_car1))
	                    dir_x_car2 = new_x_car2;

	                left = dir_x_car2/16; right = (dir_x_car2+15)/16;
	                top  = new_y_car2/16; bottom = (new_y_car2+15)/16;

	                if(collision_map[top][left] == 0 && collision_map[top][right] == 0 &&
	                   collision_map[bottom][left] == 0 && collision_map[bottom][right] == 0 &&
	                   !check_collision(dir_x_car2, new_y_car2, dir_x_car1, dir_y_car1))
	                    dir_y_car2 = new_y_car2;

	                int dx = dir_x_car2 - prev_x;
	                int dy = dir_y_car2 - prev_y;

	                if(dx > 0){      for(int i=0;i<STEP_SIZE_CAR2;i++) V_line(prev_x+i,    prev_y,16,0x9cf3); indx2=1;}
	                else if(dx < 0){ for(int i=0;i<STEP_SIZE_CAR2;i++) V_line(prev_x+15-i, prev_y,16,0x9cf3); indx2=2;}
	                if(dy > 0){      for(int i=0;i<STEP_SIZE_CAR2;i++) H_line(prev_x,prev_y+i,   16,0x9cf3); indx2=3;}
	                else if(dy < 0){ for(int i=0;i<STEP_SIZE_CAR2;i++) H_line(prev_x,prev_y+15-i,16,0x9cf3); indx2=0;}

	                LCD_Sprite(dir_x_car2, dir_y_car2, 16, 16, cars1, 4, indx2, 0, 0);
	                move_ready2 = 0;
	            }

	            // ── Carro 1 ──
	            if(move_ready1){
	                uint16_t prev_x = dir_x_car1;
	                uint16_t prev_y = dir_y_car1;
	                prev_x_car1 = dir_x_car1;

	                int left   = new_x_car1 / 16;
	                int right  = (new_x_car1 + 15) / 16;
	                int top    = dir_y_car1 / 16;
	                int bottom = (dir_y_car1 + 15) / 16;

	                if(collision_map[top][left] == 0 && collision_map[top][right] == 0 &&
	                   collision_map[bottom][left] == 0 && collision_map[bottom][right] == 0 &&
	                   !check_collision(new_x_car1, dir_y_car1, dir_x_car2, dir_y_car2))
	                    dir_x_car1 = new_x_car1;

	                left = dir_x_car1/16; right = (dir_x_car1+15)/16;
	                top  = new_y_car1/16; bottom = (new_y_car1+15)/16;

	                if(collision_map[top][left] == 0 && collision_map[top][right] == 0 &&
	                   collision_map[bottom][left] == 0 && collision_map[bottom][right] == 0 &&
	                   !check_collision(dir_x_car1, new_y_car1, dir_x_car2, dir_y_car2))
	                    dir_y_car1 = new_y_car1;

	                int dx = dir_x_car1 - prev_x;
	                int dy = dir_y_car1 - prev_y;

	                if(dx > 0){      for(int i=0;i<STEP_SIZE_CAR1;i++) V_line(prev_x+i,    prev_y,16,0x9cf3); indx1=1;}
	                else if(dx < 0){ for(int i=0;i<STEP_SIZE_CAR1;i++) V_line(prev_x+15-i, prev_y,16,0x9cf3); indx1=2;}
	                if(dy > 0){      for(int i=0;i<STEP_SIZE_CAR1;i++) H_line(prev_x,prev_y+i,   16,0x9cf3); indx1=3;}
	                else if(dy < 0){ for(int i=0;i<STEP_SIZE_CAR1;i++) H_line(prev_x,prev_y+15-i,16,0x9cf3); indx1=0;}

	                LCD_Sprite(dir_x_car1, dir_y_car1, 16, 16, cars2, 4, indx1, 0, 0);
	                move_ready1 = 0;
	            }

	            // ── Checkpoints ──
	            if(cp_car2 == 0 &&
	               prev_x_car2 > CP1_X && dir_x_car2 <= CP1_X &&
	               dir_y_car2 > CP1_Y1 && dir_y_car2 < CP1_Y2)
	            { cp_car2 = 1; cp1_event_car2 = 1; }

	            if(cp_car2 == 1 &&
	               prev_x_car2 < FINISH_X && dir_x_car2 >= FINISH_X &&
	               dir_y_car2 > FINISH_Y1 && dir_y_car2 < FINISH_Y2)
	            { if(!crossed_car2){ laps_car2++; crossed_car2=1; cp_car2=0; } }
	            else{ crossed_car2 = 0; }

	            if(cp_car1 == 0 &&
	               prev_x_car1 > CP1_X && dir_x_car1 <= CP1_X &&
	               dir_y_car1 > CP1_Y1 && dir_y_car1 < CP1_Y2)
	            { cp_car1 = 1; cp1_event_car1 = 1; }

	            if(cp_car1 == 1 &&
	               prev_x_car1 < FINISH_X && dir_x_car1 >= FINISH_X &&
	               dir_y_car1 > FINISH_Y1 && dir_y_car1 < FINISH_Y2)
	            { if(!crossed_car1){ laps_car1++; crossed_car1=1; cp_car1=0; } }
	            else{ crossed_car1 = 0; }

	            // ── Boost spawn ──
	            if(!boost_spawned){
	                if((cp1_event_car1 && cp_car2==0) || (cp1_event_car2 && cp_car1==0)){
	                    boost_visible = 1;
	                    boost_spawned = 1;
	                }
	            }
	            cp1_event_car1 = 0;
	            cp1_event_car2 = 0;

	            if(boost_visible){
	                if(cp_car1==0 && check_collision(dir_x_car1,dir_y_car1,boost_x,boost_y)){
	                    boost_visible=0; boost_car1=1; boost_time_car1=0; STEP_SIZE_CAR1=16;
	                }
	                if(cp_car2==0 && check_collision(dir_x_car2,dir_y_car2,boost_x,boost_y)){
	                    boost_visible=0; boost_car2=1; boost_time_car2=0; STEP_SIZE_CAR2=16;
	                }
	            }

	            // ── Dibujo boost ──
	            {
	                static uint8_t boost_drawn = 0;
	                if(boost_visible && !boost_drawn){
	                    LCD_Sprite(boost_x, boost_y, 16, 16, boost, 1, 0, 0, 0);
	                    boost_drawn = 1;
	                }
	                if(!boost_visible && boost_drawn){
	                    for(int i=0;i<16;i++) H_line(boost_x, boost_y+i, 16, 0x9cf3);
	                    boost_drawn = 0;
	                }
	            }

	            // ── Ganador → estado 3 ──
	            if(laps_car1 > 0 || laps_car2 > 0){
	                const char* msg = (laps_car1 > 0) ? "ganador carro 1" : "ganador carro 2";
	                int x = (320 - strlen(msg) * fontXSizeBig) / 2 - 8;
	                LCD_Print((char*)msg, x, 112, 2, 0xFFE0, 0x0000);
	                LCD_Print("\"o\" reiniciar  \"x\" menu", 0, 0, 1, 0xF800, 0xFFFF);
	                char buffer2[40];
	                sprintf(buffer2, "tiempo = %u.%u s", contador_seg, contador);
	                LCD_Print(buffer2, 5, 240-fontYSizeSmal-5, 1, 0x0000, 0xFFFF);
	                data = '3';
	                HAL_UART_Transmit(&huart5, &data, 1, 100);
	                flag_jugando = 0;
	                estado_juego = 3;
	                if (VELOCIDAD == 0){
	                	//guardar memoria
						uint32_t tiempo_actual = (uint32_t)contador_seg * 20 + contador;
						uint32_t tiempo_record = (uint32_t)highscore0_seg * 20 + highscore0_dec;

						if(tiempo_actual < tiempo_record){
							highscore0_seg = contador_seg;
							highscore0_dec = contador;
							guardar_highscore(highscore0_seg, highscore0_dec);

							// Avisar en pantalla
							LCD_Print("  nuevo record!  ", 0, 20, 1, 0x07E0, 0x0000);
						}
	                } else if (VELOCIDAD == 1){
	                	//guardar memoria
						uint32_t tiempo_actual = (uint32_t)contador_seg * 20 + contador;
						uint32_t tiempo_record = (uint32_t)highscore1_seg * 20 + highscore1_dec;

						if(tiempo_actual < tiempo_record){
							highscore1_seg = contador_seg;
							highscore1_dec = contador;
							guardar_highscore(highscore1_seg, highscore1_dec);

							// Avisar en pantalla
							LCD_Print("  nuevo record!  ", 0, 20, 1, 0x07E0, 0x0000);
						}
	                } else if (VELOCIDAD == 2){
	                	//guardar memoria
						uint32_t tiempo_actual = (uint32_t)contador_seg * 20 + contador;
						uint32_t tiempo_record = (uint32_t)highscore2_seg * 20 + highscore2_dec;

						if(tiempo_actual < tiempo_record){
							highscore2_seg = contador_seg;
							highscore2_dec = contador;
							guardar_highscore(highscore2_seg, highscore2_dec);

							// Avisar en pantalla
							LCD_Print("  nuevo record!  ", 0, 20, 1, 0x07E0, 0x0000);
						}
	                }
	            }
	            break;

	        case 2: // HIGHSCORE
	            // mostrar highscore
	        	if(!flag_highscore){
	        	        LCD_Clear(0x00);
	        	        if (VELOCIDAD == 0){
	        	        	// Título
							LCD_Print("HIGH SCORE",(320 - strlen("HIGH SCORE") * fontXSizeBig) / 2,80, 2, 0xFFE0, 0x0000);
							LCD_Print("Velocidad ",(320 - strlen("Velocidad") * fontXSizeBig) / 2,10, 2, 0xFFE0, 0x0000);
							LCD_Print("Media ",(320 - strlen("Media") * fontXSizeBig) / 2,40, 2, 0xFFE0, 0x0000);
							// Valor
							char hs_buf[40];
							if(highscore0_seg == 9999){
								sprintf(hs_buf, "sin record aun");
							} else {
								sprintf(hs_buf, "tiempo: %u.%u s", highscore0_seg, highscore0_dec);
							}
							LCD_Print(hs_buf,(320 - strlen(hs_buf) * fontXSizeSmal) / 2, 130, 1, 0xFFFF, 0x0000);

							LCD_Print("Presione \"x\" para volver",(320 - strlen("Presione \"x\" para volver") * fontXSizeSmal) / 2,200, 1, 0xF800, 0x0000);

							flag_highscore = 1;
	        	        } else if (VELOCIDAD == 1){
	        	        	// Título
							LCD_Print("HIGH SCORE",(320 - strlen("HIGH SCORE") * fontXSizeBig) / 2,80, 2, 0xFFE0, 0x0000);
							LCD_Print("Velocidad ",(320 - strlen("Velocidad") * fontXSizeBig) / 2,10, 2, 0xFFE0, 0x0000);
							LCD_Print("Alta ",(320 - strlen("Alta") * fontXSizeBig) / 2,40, 2, 0xFFE0, 0x0000);
							// Valor
							char hs_buf[40];
							if(highscore1_seg == 9999){
								sprintf(hs_buf, "sin record aun");
							} else {
								sprintf(hs_buf, "tiempo: %u.%u s", highscore1_seg, highscore1_dec);
							}
							LCD_Print(hs_buf,(320 - strlen(hs_buf) * fontXSizeSmal) / 2, 130, 1, 0xFFFF, 0x0000);

							LCD_Print("Presione \"x\" para volver",(320 - strlen("Presione \"x\" para volver") * fontXSizeSmal) / 2,200, 1, 0xF800, 0x0000);

							flag_highscore = 1;
	        	        }else if(VELOCIDAD == 2){
	        	        	// Título
							LCD_Print("HIGH SCORE ",(320 - strlen("HIGH SCORE ") * fontXSizeBig) / 2,80, 2, 0xFFE0, 0x0000);
							LCD_Print("Velocidad ",(320 - strlen("Velocidad") * fontXSizeBig) / 2,10, 2, 0xFFE0, 0x0000);
							LCD_Print("Baja ",(320 - strlen("Baja") * fontXSizeBig) / 2,40, 2, 0xFFE0, 0x0000);
							// Valor
							char hs_buf[40];
							if(highscore2_seg == 9999){
								sprintf(hs_buf, "sin record aun");
							} else {
								sprintf(hs_buf, "tiempo: %u.%u s", highscore2_seg, highscore2_dec);
							}
							LCD_Print(hs_buf,(320 - strlen(hs_buf) * fontXSizeSmal) / 2, 130, 1, 0xFFFF, 0x0000);

							LCD_Print("Presione \"x\" para volver",(320 - strlen("Presione \"x\" para volver") * fontXSizeSmal) / 2,200, 1, 0xF800, 0x0000);

							flag_highscore = 1;
	        	        }

	        	    }
	            if(rx_control1[0] == 'x' || rx_control2[0] == 'x'){
	                rx_control1[0] = 0;
	                rx_control2[0] = 0;
	                flag_menu = 0;
	                estado_juego = 0;
	            }
	            break;

	        case 3: // GANADOR / PANTALLA FINAL
	            data = '3';
	            HAL_UART_Transmit(&huart5, &data, 1, 100);

	            // 'o' → reiniciar carrera
	            if(rx_control1[0] == 'o' || rx_control2[0] == 'o'){
	                rx_control1[0] = 0;
	                rx_control2[0] = 0;
	                flag_mapa_carrera = 0;
	                restartRace();
	                estado_juego = 1;
	            }
	            // 'x' → volver al menú
	            if(rx_control1[0] == 'x' || rx_control2[0] == 'x'){
	                rx_control1[0] = 0;
	                rx_control2[0] = 0;
	                flag_menu = 0;
	                estado_juego = 0;
	            }
	            break;
	        case 4:
	        	if(!flag_velocidad){
					LCD_Clear(0x0000);

					LCD_Print("Seleccionar velocidad",
							  (320 - strlen("Seleccionar velocidad") * fontXSizeSmal) / 2,
							  60, 1, 0xFFFF, 0x0000);

					LCD_Print("Alta:   presione []",
							  (320 - strlen("Alta:   presione X") * fontXSizeSmal) / 2,
							  90, 1, 0xF800, 0x0000);

					LCD_Print("Media:  presione O",
							  (320 - strlen("Media:  presione O") * fontXSizeSmal) / 2,
							  120, 1, 0xFFE0, 0x0000);

					LCD_Print("Baja:   presione T",
							  (320 - strlen("Baja:   presione T") * fontXSizeSmal) / 2,
							  150, 1, 0x07E0, 0x0000);
					LCD_Print("Volver: presione X",
					          (320 - strlen("Volver: presione S") * fontXSizeSmal) / 2,
					          170, 1, 0xFFFF, 0x0000);
					flag_velocidad = 1;
				}

				if(rx_control1[0] == 's' || rx_control2[0] == 's'){
					// velocidad alta
					rx_control1[0] = 0;
					rx_control2[0] = 0;
					STEP_SIZE_CAR1 = 16;
					STEP_SIZE_CAR2 = 16;
					VELOCIDAD = 1;
					flag_velocidad = 0;
					flag_menu = 0;
					estado_juego = 0;
					cargar_highscore();
				}
				if(rx_control1[0] == 'o' || rx_control2[0] == 'o'){
					// velocidad media
					VELOCIDAD = 0;
					rx_control1[0] = 0;
					rx_control2[0] = 0;
					STEP_SIZE_CAR1 = 8;
					STEP_SIZE_CAR2 = 8;
					flag_velocidad = 0;
					flag_menu = 0;
					estado_juego = 0;
					cargar_highscore();
				}
				if(rx_control1[0] == 't' || rx_control2[0] == 't'){
					// velociadad baja
					VELOCIDAD = 2;
					rx_control1[0] = 0;
					rx_control2[0] = 0;
					STEP_SIZE_CAR1 = 4;
					STEP_SIZE_CAR2 = 4;
					flag_velocidad = 0;
					flag_menu = 0;
					estado_juego = 0;
					cargar_highscore();
				}
				if(rx_control1[0] == 'x' || rx_control2[0] == 'x'){
				    rx_control1[0] = 0;
				    rx_control2[0] = 0;
				    flag_velocidad = 0;
				    flag_menu = 0;
				    estado_juego = 0;
				}
				break;
	        default:
	            estado_juego = 0;
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
  * @brief SPI2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI2_Init(void)
{

  /* USER CODE BEGIN SPI2_Init 0 */

  /* USER CODE END SPI2_Init 0 */

  /* USER CODE BEGIN SPI2_Init 1 */

  /* USER CODE END SPI2_Init 1 */
  /* SPI2 parameter configuration*/
  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi2.Init.NSS = SPI_NSS_SOFT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI2_Init 2 */

  /* USER CODE END SPI2_Init 2 */

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
  htim6.Init.Prescaler = 8400-1;
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 500-1;
  htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM6_Init 2 */

  /* USER CODE END TIM6_Init 2 */

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
  HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(SD_SS_GPIO_Port, SD_SS_Pin, GPIO_PIN_SET);

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

  /*Configure GPIO pins : LCD_CS_Pin SD_SS_Pin */
  GPIO_InitStruct.Pin = LCD_CS_Pin|SD_SS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */
  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
// Interrupciones
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	if(htim == &htim6){
		if(flag_jugando){
			contador++;
			if (contador >= 20){
				// ya paso 1 seg.
				contador_seg++;
				contador = 0;
			}
		}

		if(boost_visible){
			boost_timer++;

			if(boost_timer >= 100){ // 100 * 50ms = 5s
				boost_visible = 0;
				boost_timer = 0;
			}
		}

		if(boost_car1){
		    boost_time_car1++;
		    if(boost_time_car1 >= 100){
		        boost_car1 = 0;
		        if(VELOCIDAD == 0){
		        	STEP_SIZE_CAR1 = 8;
		        }else if (VELOCIDAD == 2){
		        	STEP_SIZE_CAR1 = 4;
		        }

		    }
		}

		if(boost_car2){
		    boost_time_car2++;
		    if(boost_time_car2 >= 100){
		        boost_car2 = 0;
		        if(VELOCIDAD == 0){
					STEP_SIZE_CAR1 = 8;
				}else if (VELOCIDAD == 2){
					STEP_SIZE_CAR1 = 4;
				}
		    }
		}

	}
}
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart == &huart3)
	{
	    cmd1 = rx_control1[0];

	    new_x_car1 = dir_x_car1;
	    new_y_car1 = dir_y_car1;

	    switch(cmd1){
	        case 'r': new_x_car1 += STEP_SIZE_CAR1; break;
	        case 'l': new_x_car1 -= STEP_SIZE_CAR1; break;
	        case 'u': new_y_car1 -= STEP_SIZE_CAR1; break;
	        case 'd': new_y_car1 += STEP_SIZE_CAR1; break;
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
					case 'r': new_x_car2 += STEP_SIZE_CAR2; break;
					case 'l': new_x_car2 -= STEP_SIZE_CAR2; break;
					case 'u': new_y_car2 -= STEP_SIZE_CAR2; break;
					case 'd': new_y_car2 += STEP_SIZE_CAR2; break;
				}

				move_ready2 = 1;

				HAL_UART_Receive_DMA(&huart6, rx_control2, 1);
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
