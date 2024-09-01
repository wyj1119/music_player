#ifndef __SDRAM_H__
#define __SDRAM_H__

#include "main.h"

#define exSDRAM_BASE_ADDR    (0XC0000000) //SDRAM开始地址//由于外扩SDRAM数据位是16位的,所以这里必须将其搞成uin16_t
#define exSDRAM_SIZE (32*1024*1024)

#define MUSIC_LIST_ADDR 				exSDRAM_BASE_ADDR
#define MUSIC_PLAY_LIST_ADDR 		0xC0008000
#define SONGNAME_UNICODE16_ADDR		0xC05EC000
#define SONGNAME_UTF8_ADDR			0xC05EC100
//0xC05EC300

#define LCD_LAYER0_ADDR 0xC0010000
#define LCD_LAYER1_ADDR 0xC0187000
#define LVGL_BUF0_ADDR 0xC02FE000
#define LVGL_BUF1_ADDR 0xC0475000


//SDRAM配置参数
#define SDRAM_MODEREG_BURST_LENGTH_1             ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_LENGTH_2             ((uint16_t)0x0001)
#define SDRAM_MODEREG_BURST_LENGTH_4             ((uint16_t)0x0002)
#define SDRAM_MODEREG_BURST_LENGTH_8             ((uint16_t)0x0004)
#define SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL      ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_TYPE_INTERLEAVED     ((uint16_t)0x0008)
#define SDRAM_MODEREG_CAS_LATENCY_2              ((uint16_t)0x0020)
#define SDRAM_MODEREG_CAS_LATENCY_3              ((uint16_t)0x0030)
#define SDRAM_MODEREG_OPERATING_MODE_STANDARD    ((uint16_t)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_PROGRAMMED ((uint16_t)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_SINGLE     ((uint16_t)0x0200)



void SDRAM_Initialization_Sequence(SDRAM_HandleTypeDef *hsdram);
uint8_t SDRAM_Send_Cmd(uint8_t bankx,uint8_t cmd,uint8_t refresh,uint16_t regval);
void FMC_SDRAM_WriteBuffer(uint8_t *pBuffer,uint32_t WriteAddr,uint32_t n);
void FMC_SDRAM_ReadBuffer(uint8_t *pBuffer,uint32_t ReadAddr,uint32_t n);
void fmc_sdram_test(void);

#endif
