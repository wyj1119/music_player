#ifndef __NORFLASH_H__
#define __NORFLASH_H__

#include "main.h"
//W25Xϵ��/Qϵ��оƬ�б�	   
#define W25Q80 	0XEF13 	
#define W25Q16 	0XEF14
#define W25Q32 	0XEF15
#define W25Q64 	0XEF16
#define W25Q128	0XEF17
#define W25Q256 0XEF18

extern uint16_t NORFLASH_TYPE;					//����W25QXXоƬ�ͺ�		   

#define exFLASH_BASE_ADDR    (0X90000000) //FLASH��ʼ��ַ
#define exFLASH_SIZE (32*1024*1024)

//cc936
#define font_ADDR (0X90000000)
#define uni2oem_ADDR 	font_ADDR
#define oem2uni_ADDR (0X90015484)
#define font_kai_24_ADDR 	 (0X9002A908)

//icon ͼ��
#define icon_ADDR			(0X91000000)
#define icon_pause_ADDR		 icon_ADDR
#define icon_play_ADDR   (0X91002710)
#define icon_next_ADDR		 (0X91004E20)
#define icon_previous_ADDR   (0X91007530)

#define icon_shuffle_ADDR   (0x91009C40)
#define icon_repeatOne_ADDR		 (0x9100C350)
#define icon_repeat_ADDR   (0x9100EA60)


#define icon_pause_SIZE    (50*50*4)
#define icon_play_SIZE    (50*50*4)
#define icon_next_SIZE    (50*50*4)
#define icon_previous_SIZE    (50*50*4)

#define icon_shuffle_SIZE    (50*50*4)
#define icon_repeatOne_SIZE    (50*50*4)
#define icon_repeat_SIZE    (50*50*4)

////////////////////////////////////////////////////////////////////////////////// 
//ָ���
#define W25X_WriteEnable		0x06 
#define W25X_WriteDisable		0x04 
#define W25X_ReadStatusReg1		0x05 
#define W25X_ReadStatusReg2		0x35 
#define W25X_ReadStatusReg3		0x15 
#define W25X_WriteStatusReg1    0x01 
#define W25X_WriteStatusReg2    0x31 
#define W25X_WriteStatusReg3    0x11 
#define W25X_ReadData			0x03 
#define W25X_FastReadData		0x0B 
#define W25X_FastReadDual		0x3B 
#define W25X_PageProgram		0x02 
#define W25X_QuadPageProgram  0X32
#define W25X_BlockErase			0xD8 
#define W25X_SectorErase		0x20 
#define W25X_ChipErase			0xC7 
#define W25X_PowerDown			0xB9 
#define W25X_ReleasePowerDown	0xAB 
#define W25X_DeviceID			0xAB 
#define W25X_ManufactDeviceID	0x90 
#define W25X_JedecDeviceID		0x9F 
#define W25X_Enable4ByteAddr    0xB7
#define W25X_Exit4ByteAddr      0xE9
#define W25X_SetReadParam		0xC0 
#define W25X_EnterQPIMode       0x38
#define W25X_ExitQPIMode        0xFF

void NORFLASH_Init(uint8_t mem_map_opt);				//��ʼ��W25QXX
void NORFLASH_Qspi_Enable(void);		//ʹ��QSPIģʽ
void NORFLASH_Qspi_Disable(void);		//�ر�QSPIģʽ
uint16_t  NORFLASH_ReadID(void);				//��ȡFLASH ID
uint8_t NORFLASH_ReadSR(uint8_t regno);			//��ȡ״̬�Ĵ��� 
void NORFLASH_4ByteAddr_Enable(void);	//ʹ��4�ֽڵ�ַģʽ
void NORFLASH_Write_SR(uint8_t regno,uint8_t sr);	//д״̬�Ĵ���
void NORFLASH_Write_Enable(void);  		//дʹ�� 
void NORFLASH_Write_Disable(void);		//д����
void NORFLASH_Write_NoCheck(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite);	//дflash,��У��
void NORFLASH_Read(uint8_t* pBuffer,uint32_t ReadAddr,uint16_t NumByteToRead);   			//��ȡflash
void NORFLASH_Write(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite);			//д��flash
void NORFLASH_Erase_Chip(void);    	  		//��Ƭ����
void NORFLASH_Erase_Sector(uint32_t Dst_Addr);	//��������
void NORFLASH_Wait_Busy(void);           	//�ȴ�����

void show_flash_state(void);
void memmap_flash(void);

#endif
