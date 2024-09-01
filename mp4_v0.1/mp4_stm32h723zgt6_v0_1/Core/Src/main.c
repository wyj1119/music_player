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
#include "dma.h"
#include "dma2d.h"
#include "fatfs.h"
#include "i2c.h"
#include "usart.h"
#include "ltdc.h"
#include "octospi.h"
#include "rng.h"
#include "sai.h"
#include "sdmmc.h"
#include "tim.h"
#include "gpio.h"
#include "fmc.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "sdram.h"
#include "norflash.h"
#include "wav_play.h"
#include "key.h"
#include "rgb_lcd.h"
#include "gt911.h"
#include "lvgl_task.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define START_TASK_PRIO 1 //任务优先级
#define START_STK_SIZE 32 //任务堆栈大小
TaskHandle_t StartTask_Handler; //任务句柄
void start_task(void *pvParameters); //任务函数

#define FILLBUF_TASK_PRIO 6 //任务优先级
#define FILLBUF_STK_SIZE 128 //任务堆栈大小
TaskHandle_t FillBuf_Handler; //任务句柄
void fill_buf(void *pvParameters); //任务函数

#define KEY_SCAN_TASK_PRIO 5 //任务优先级
#define KEY_SCAN_STK_SIZE 256 //任务堆栈大小
TaskHandle_t KeyScan_Handler; //任务句柄
//void key_scan(void *pvParameters); //任务函数

#define ChangeMusic_PRIO 3 //任务优先级
#define ChangeMusic_STK_SIZE 256 //任务堆栈大小
TaskHandle_t ChangeMusic_Handler; //任务句柄
//void ChangeMusic(void *pvParameters); //任务函数

#define LVGL_PRIO 4 //任务优先级
#define LVGL_STK_SIZE 1024 //任务堆栈大小
TaskHandle_t LVGL_Handler; //任务句柄
//void lvgl(void *pvParameters); //任务函数


/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint32_t rand_num;//存放随机数

WavPlay wav_play = {0};
FATFS SD_FATFS;

char music_list[256][FILE_NAME_LEN_MAX] __attribute__((at(MUSIC_LIST_ADDR)));//歌曲列表
char music_play_list[256][FILE_NAME_LEN_MAX] __attribute__((at(MUSIC_PLAY_LIST_ADDR)));//歌曲播放列表

__IO uint32_t frame_buf[480][800] __attribute__((at(LCD_LAYER0_ADDR)));
__IO uint32_t frame_buf1[480][800] __attribute__((at(LCD_LAYER1_ADDR)));

QueueHandle_t send_buf_finish_handle;//I2S的DMA传送完一帧
QueueHandle_t change_music_handle;//切歌

extern lv_obj_t *label_now_t;
extern lv_obj_t *label_total_t;

extern lv_obj_t *song_pbar;
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
	uint8_t res = FR_OK;
	
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
	MX_LPUART1_UART_Init();
	MX_FMC_Init();
	MX_OCTOSPI1_Init();
  MX_DMA_Init();
  MX_LTDC_Init();
  MX_SAI1_Init();
  MX_SDMMC2_SD_Init();
  MX_TIM3_Init();
  MX_I2C1_Init();
  MX_RNG_Init();
  MX_FATFS_Init();
  MX_DMA2D_Init();
  /* USER CODE BEGIN 2 */
//	HAL_TIM_Base_Start_IT(&htim3);

	lv_init();
	lv_port_disp_init();
//	lv_port_indev_init();
	my_gui_app();
	init_wav_play_param(2,0,&wav_play);
	
	do{
		res = f_mount(&SD_FATFS,"0:",1);
		if(res != FR_OK)
		{
			USART_printf(&hlpuart1,"f_mount error:%u\r\n",res);
		}
	}while(res!=FR_OK);
	
	scan_files("music",music_list,&wav_play.music_total);	
	get_play_list(music_list,music_play_list);
	
	send_buf_finish_handle = xSemaphoreCreateBinary();//更新缓冲区信号量
	if(!send_buf_finish_handle)
	{
		USART_printf(&hlpuart1,"semaphore send_buf_finish_handle create error\r\n");
		while(1);
	}
	change_music_handle = xSemaphoreCreateBinary();
	if(!change_music_handle)
	{
		USART_printf(&hlpuart1,"semaphore change_music_handle create error\r\n");
		while(1);
	}
	
	USART_printf(&hlpuart1,"music_play_now:%u\r\n",wav_play.music_play_now);
	Init_wav_play(&wav_play,music_play_list[wav_play.music_play_now]);
	xTaskCreate((TaskFunction_t )start_task, //任务函数
			  (const char* )"start_task", //任务名称
				(uint16_t )START_STK_SIZE, //任务堆栈大小
				(void* )NULL, //传递给任务函数的参数
				(UBaseType_t )START_TASK_PRIO, //任务优先级
				(TaskHandle_t* )&StartTask_Handler); //任务句柄 	
  vTaskStartScheduler(); //开启任务调度
	
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
//		HAL_GPIO_TogglePin(LED_R_GPIO_Port,LED_R_Pin);
		
		HAL_Delay(500);
		
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

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 2;
  RCC_OscInitStruct.PLL.PLLN = 44;
  RCC_OscInitStruct.PLL.PLLP = 1;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_3;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void start_task(void *pvParameters)
{
	taskENTER_CRITICAL(); //进入临界区
	//创建任务
	xTaskCreate((TaskFunction_t )fill_buf, //任务函数
			  (const char* )"FillBuf", //任务名称
				(uint16_t )FILLBUF_STK_SIZE, //任务堆栈大小
				(void* )NULL, //传递给任务函数的参数
				(UBaseType_t )FILLBUF_TASK_PRIO, //任务优先级
				(TaskHandle_t* )&FillBuf_Handler); //任务句柄 
				
	xTaskCreate((TaskFunction_t )key_scan, //任务函数
			  (const char* )"KeyScan", //任务名称
				(uint16_t )KEY_SCAN_STK_SIZE, //任务堆栈大小
				(void* )NULL, //传递给任务函数的参数
				(UBaseType_t )KEY_SCAN_TASK_PRIO, //任务优先级
				(TaskHandle_t* )&KeyScan_Handler); //任务句柄 
	
	xTaskCreate((TaskFunction_t )ChangeMusic, //任务函数
			(const char* )"ChangeMusic", //任务名称
			(uint16_t )ChangeMusic_STK_SIZE, //任务堆栈大小
			(void* )NULL, //传递给任务函数的参数
			(UBaseType_t )ChangeMusic_PRIO, //任务优先级
			(TaskHandle_t* )&ChangeMusic_Handler); //任务句柄

	xTaskCreate((TaskFunction_t )lvgl, //任务函数
			(const char* )"lvgl", //任务名称
			(uint16_t )LVGL_STK_SIZE, //任务堆栈大小
			(void* )NULL, //传递给任务函数的参数
			(UBaseType_t )LVGL_PRIO, //任务优先级
			(TaskHandle_t* )&lvgl); //任务句柄
			
	vTaskDelete(StartTask_Handler); //删除开始任务
	taskEXIT_CRITICAL(); //退出临界区
}


void fill_buf(void *pvParameters)
{
	BaseType_t res;
	while(1)
	{
		xSemaphoreTake(send_buf_finish_handle,portMAX_DELAY);
		if(wav_play.play_sta == 1)
		{
			if(wav_play.buf_sta == 0)
			{
				taskENTER_CRITICAL(); //进入临界区
				f_lseek(&wav_play.WAV_FIL, wav_play.file_ptr);
				f_read(&wav_play.WAV_FIL,wav_play.buf0.buf_u8,wav_play.wavhead.FMT.SampleRate/10,&wav_play.br_wav);
				tune_volume(&wav_play);
				taskEXIT_CRITICAL(); //退出临界区
				wav_play.file_ptr+=wav_play.br_wav;								
				
			}
			else if(wav_play.buf_sta == 1)
			{
				taskENTER_CRITICAL(); //进入临界区
				f_lseek(&wav_play.WAV_FIL, wav_play.file_ptr);
				f_read(&wav_play.WAV_FIL,wav_play.buf1.buf_u8,wav_play.wavhead.FMT.SampleRate/10,&wav_play.br_wav);
				tune_volume(&wav_play);
				taskEXIT_CRITICAL(); //退出临界区
				wav_play.file_ptr+=wav_play.br_wav;
			}
			
			if(wav_play.br_wav < wav_play.wavhead.FMT.SampleRate/10)
//			if(wav_play.file_ptr > 100000)//测试整个歌单能否播放
			{
				f_close(&wav_play.WAV_FIL);
				wav_play.file_ptr = 0;//文件指针清0
				
				if(wav_play.buf_sta == 0)
					memset(wav_play.buf0.buf_u8+wav_play.br_wav,0,wav_play.wavhead.FMT.SampleRate/10 - wav_play.br_wav);
				else if(wav_play.buf_sta == 1)
					memset(wav_play.buf1.buf_u8+wav_play.br_wav,0,wav_play.wavhead.FMT.SampleRate/10 - wav_play.br_wav);
								
				wav_play.play_sta = 2;//播放结束		
			}
			
			wav_play.pbar.now_time_byte += wav_play.br_wav;			
			wav_play.pbar.now_time_s = ((float)wav_play.pbar.now_time_byte / wav_play.pbar.total_time_byte)*wav_play.pbar.total_time_s + 0.5f;
			lv_label_set_text_fmt(label_now_t,"%02u:%02u",wav_play.pbar.now_time_s/60,wav_play.pbar.now_time_s%60);
			lv_bar_set_value(song_pbar,wav_play.pbar.now_time_s,LV_ANIM_ON);
		}
		else if(wav_play.play_sta == 2)
		{
			wav_play.play_sta = 3;
			wav_play.pbar.now_time_byte = 0;
			
			if(wav_play.play_mode != 2)
			{
				if(++wav_play.music_play_now>=wav_play.music_total)
					wav_play.music_play_now = 0;
			}
			
			HAL_SAI_DMAPause(&hsai_BlockA1);
			wav_play.play_sta = 3;
			f_close(&wav_play.WAV_FIL);
			wav_play.buf_sta = 0;
			res = xSemaphoreGive(change_music_handle);
			if(res==pdFALSE)
				USART_printf(&hlpuart1,"music play end / xSemaphoreGive error\r\n");
		}

	}
}


/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM23 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM23) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */
	if(htim->Instance == TIM3)
	{
//		lv_tick_inc(1);	
	}
  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
	USART_printf(&hlpuart1,"Error_Handler\r\n");
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
