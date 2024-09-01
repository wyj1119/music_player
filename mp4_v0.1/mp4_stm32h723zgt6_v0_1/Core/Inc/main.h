/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "lv_port_disp_template.h"
#include "lv_port_indev_template.h"
#include "lvgl.h"
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
#define volumeplus_Pin GPIO_PIN_3
#define volumeplus_GPIO_Port GPIOE
#define USER_KEY_Pin GPIO_PIN_13
#define USER_KEY_GPIO_Port GPIOC
#define volumepop_Pin GPIO_PIN_0
#define volumepop_GPIO_Port GPIOC
#define SD_CD_Pin GPIO_PIN_1
#define SD_CD_GPIO_Port GPIOC
#define T_RST_Pin GPIO_PIN_12
#define T_RST_GPIO_Port GPIOB
#define T_INT_Pin GPIO_PIN_13
#define T_INT_GPIO_Port GPIOB
#define LED_R_Pin GPIO_PIN_3
#define LED_R_GPIO_Port GPIOG
#define LED_G_Pin GPIO_PIN_8
#define LED_G_GPIO_Port GPIOC
#define LED_B_Pin GPIO_PIN_11
#define LED_B_GPIO_Port GPIOC
#define A_XSMT_Pin GPIO_PIN_4
#define A_XSMT_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */
#define PRINT_ERROR(res,num) if(res){USART_printf(&hlpuart1,"error:%u\r\n",num);return;}
#define I2S_BUF 4800
extern uint32_t rand_num;//´æ·ÅËæ»úÊý
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
