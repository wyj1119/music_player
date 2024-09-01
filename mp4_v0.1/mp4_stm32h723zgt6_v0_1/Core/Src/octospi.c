/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    octospi.c
  * @brief   This file provides code for the configuration
  *          of the OCTOSPI instances.
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
#include "octospi.h"

/* USER CODE BEGIN 0 */
#include "norflash.h"
/* USER CODE END 0 */

OSPI_HandleTypeDef hospi1;

/* OCTOSPI1 init function */
void MX_OCTOSPI1_Init(void)
{

  /* USER CODE BEGIN OCTOSPI1_Init 0 */

  /* USER CODE END OCTOSPI1_Init 0 */

  OSPIM_CfgTypeDef sOspiManagerCfg = {0};

  /* USER CODE BEGIN OCTOSPI1_Init 1 */

  /* USER CODE END OCTOSPI1_Init 1 */
  hospi1.Instance = OCTOSPI1;
  hospi1.Init.FifoThreshold = 4;
  hospi1.Init.DualQuad = HAL_OSPI_DUALQUAD_DISABLE;
  hospi1.Init.MemoryType = HAL_OSPI_MEMTYPE_MACRONIX;
  hospi1.Init.DeviceSize = 25;
  hospi1.Init.ChipSelectHighTime = 5;
  hospi1.Init.FreeRunningClock = HAL_OSPI_FREERUNCLK_DISABLE;
  hospi1.Init.ClockMode = HAL_OSPI_CLOCK_MODE_0;
  hospi1.Init.WrapSize = HAL_OSPI_WRAP_NOT_SUPPORTED;
  hospi1.Init.ClockPrescaler = 2;
  hospi1.Init.SampleShifting = HAL_OSPI_SAMPLE_SHIFTING_HALFCYCLE;
  hospi1.Init.DelayHoldQuarterCycle = HAL_OSPI_DHQC_ENABLE;
  hospi1.Init.ChipSelectBoundary = 0;
  hospi1.Init.DelayBlockBypass = HAL_OSPI_DELAY_BLOCK_BYPASSED;
  hospi1.Init.MaxTran = 0;
  hospi1.Init.Refresh = 0;
  if (HAL_OSPI_Init(&hospi1) != HAL_OK)
  {
    Error_Handler();
  }
  sOspiManagerCfg.ClkPort = 1;
  sOspiManagerCfg.NCSPort = 1;
  sOspiManagerCfg.IOLowPort = HAL_OSPIM_IOPORT_1_LOW;
  if (HAL_OSPIM_Config(&hospi1, &sOspiManagerCfg, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN OCTOSPI1_Init 2 */
	NORFLASH_Init(1);//0--������flash�ڴ�ӳ��  1--����flash�ڴ�ӳ��(����ַ:exFLASH_BASE_ADDR)
  /* USER CODE END OCTOSPI1_Init 2 */

}

void HAL_OSPI_MspInit(OSPI_HandleTypeDef* ospiHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
  if(ospiHandle->Instance==OCTOSPI1)
  {
  /* USER CODE BEGIN OCTOSPI1_MspInit 0 */

  /* USER CODE END OCTOSPI1_MspInit 0 */

  /** Initializes the peripherals clock
  */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_OSPI;
    PeriphClkInitStruct.OspiClockSelection = RCC_OSPICLKSOURCE_D1HCLK;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      Error_Handler();
    }

    /* OCTOSPI1 clock enable */
    __HAL_RCC_OCTOSPIM_CLK_ENABLE();
    __HAL_RCC_OSPI1_CLK_ENABLE();

    __HAL_RCC_GPIOF_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();
    /**OCTOSPI1 GPIO Configuration
    PF6     ------> OCTOSPIM_P1_IO3
    PF7     ------> OCTOSPIM_P1_IO2
    PF8     ------> OCTOSPIM_P1_IO0
    PF9     ------> OCTOSPIM_P1_IO1
    PF10     ------> OCTOSPIM_P1_CLK
    PG6     ------> OCTOSPIM_P1_NCS
    */
    GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF10_OCTOSPIM_P1;
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_OCTOSPIM_P1;
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF10_OCTOSPIM_P1;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  /* USER CODE BEGIN OCTOSPI1_MspInit 1 */

  /* USER CODE END OCTOSPI1_MspInit 1 */
  }
}

void HAL_OSPI_MspDeInit(OSPI_HandleTypeDef* ospiHandle)
{

  if(ospiHandle->Instance==OCTOSPI1)
  {
  /* USER CODE BEGIN OCTOSPI1_MspDeInit 0 */

  /* USER CODE END OCTOSPI1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_OCTOSPIM_CLK_DISABLE();
    __HAL_RCC_OSPI1_CLK_DISABLE();

    /**OCTOSPI1 GPIO Configuration
    PF6     ------> OCTOSPIM_P1_IO3
    PF7     ------> OCTOSPIM_P1_IO2
    PF8     ------> OCTOSPIM_P1_IO0
    PF9     ------> OCTOSPIM_P1_IO1
    PF10     ------> OCTOSPIM_P1_CLK
    PG6     ------> OCTOSPIM_P1_NCS
    */
    HAL_GPIO_DeInit(GPIOF, GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9
                          |GPIO_PIN_10);

    HAL_GPIO_DeInit(GPIOG, GPIO_PIN_6);

  /* USER CODE BEGIN OCTOSPI1_MspDeInit 1 */

  /* USER CODE END OCTOSPI1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

//�ȴ�״̬��־
//flag:��Ҫ�ȴ��ı�־λ
//sta:��Ҫ�ȴ���״̬
//wtime:�ȴ�ʱ��
//����ֵ:0,�ȴ��ɹ�.
//	     1,�ȴ�ʧ��.
uint8_t QSPI_Wait_Flag(uint8_t flag,uint8_t sta,uint8_t wtime)
{
	uint8_t flagsta=0;
	while(wtime)
	{
		flagsta=(OCTOSPI1->SR&flag)?1:0; 
		if(flagsta==sta)break;
		wtime--;
	}
	if(wtime)return 0;
	else return 1;
}
//QSPI��������
//cmd:Ҫ���͵�ָ��
//addr:���͵���Ŀ�ĵ�ַ
//mode:ģʽ,��ϸλ��������:
//	mode[1:0]:ָ��ģʽ;00,��ָ��;01,���ߴ���ָ��;10,˫�ߴ���ָ��;11,���ߴ���ָ��.
//	mode[3:2]:��ַģʽ;00,�޵�ַ;01,���ߴ����ַ;10,˫�ߴ����ַ;11,���ߴ����ַ.
//	mode[5:4]:��ַ����;00,8λ��ַ;01,16λ��ַ;10,24λ��ַ;11,32λ��ַ.
//	mode[7:6]:����ģʽ;00,������;01,���ߴ�������;10,˫�ߴ�������;11,���ߴ�������.
//dmcycle:��ָ��������
void QSPI_Send_CMD(uint8_t cmd,uint32_t addr,uint8_t mode,uint8_t dmcycle)
{
	OSPI_RegularCmdTypeDef Cmdhandler;
    
	Cmdhandler.Instruction=cmd;									//ָ��
	Cmdhandler.Address=addr;									//��ַ
	Cmdhandler.DummyCycles=dmcycle;								//���ÿ�ָ��������
	
	if(((mode>>0)&0x03) == 0)
	Cmdhandler.InstructionMode=HAL_OSPI_INSTRUCTION_NONE;			//ָ��ģʽ
	else if(((mode>>0)&0x03) == 1)
	Cmdhandler.InstructionMode=HAL_OSPI_INSTRUCTION_1_LINE;			//ָ��ģʽ
	else if(((mode>>0)&0x03) == 2)
	Cmdhandler.InstructionMode=HAL_OSPI_INSTRUCTION_2_LINES;			//ָ��ģʽ
	else if(((mode>>0)&0x03) == 3)
	Cmdhandler.InstructionMode=HAL_OSPI_INSTRUCTION_4_LINES;			//ָ��ģʽ
	
	if(((mode>>2)&0x03) == 0)
	Cmdhandler.AddressMode=HAL_OSPI_ADDRESS_NONE;   					//��ַģʽ
	else if(((mode>>2)&0x03) == 1)
	Cmdhandler.AddressMode=HAL_OSPI_ADDRESS_1_LINE;   					//��ַģʽ
	else if(((mode>>2)&0x03) == 2)
	Cmdhandler.AddressMode=HAL_OSPI_ADDRESS_2_LINES;   					//��ַģʽ
	else if(((mode>>2)&0x03) == 3)
	Cmdhandler.AddressMode=HAL_OSPI_ADDRESS_4_LINES;   					//��ַģʽ
	
	if(((mode>>4)&0x03) == 0)
	Cmdhandler.AddressSize=HAL_OSPI_ADDRESS_8_BITS;   					//��ַ����
	else if(((mode>>4)&0x03) == 1)
	Cmdhandler.AddressSize=HAL_OSPI_ADDRESS_16_BITS;   					//��ַ����
	else if(((mode>>4)&0x03) == 2)
	Cmdhandler.AddressSize=HAL_OSPI_ADDRESS_24_BITS;   					//��ַ����
	else if(((mode>>4)&0x03) == 3)
	Cmdhandler.AddressSize=HAL_OSPI_ADDRESS_32_BITS;   					//��ַ����
	
	if(((mode>>6)&0x03) == 0)
	Cmdhandler.DataMode=HAL_OSPI_DATA_NONE;             			//����ģʽ
	else if(((mode>>6)&0x03) == 1)
	Cmdhandler.DataMode=HAL_OSPI_DATA_1_LINE;             			//����ģʽ
	else if(((mode>>6)&0x03) == 2)
	Cmdhandler.DataMode=HAL_OSPI_DATA_2_LINES;             			//����ģʽ
	else if(((mode>>6)&0x03) == 3)
	Cmdhandler.DataMode=HAL_OSPI_DATA_4_LINES;             			//����ģʽ
	
	Cmdhandler.SIOOMode=HAL_OSPI_SIOO_INST_EVERY_CMD;				//ÿ�ζ�����ָ��
	Cmdhandler.AlternateBytesMode=HAL_OSPI_ALTERNATE_BYTES_NONE;		//�޽����ֽ�
	
	Cmdhandler.OperationType = HAL_OSPI_OPTYPE_COMMON_CFG;
	Cmdhandler.FlashId = HAL_OSPI_FLASH_ID_1;
	Cmdhandler.NbData = 1;
	Cmdhandler.DataDtrMode = HAL_OSPI_DATA_DTR_DISABLE;
	Cmdhandler.InstructionSize = HAL_OSPI_INSTRUCTION_8_BITS;
	Cmdhandler.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;
	Cmdhandler.AddressDtrMode = HAL_OSPI_ADDRESS_DTR_DISABLE;
	Cmdhandler.DQSMode = HAL_OSPI_DQS_DISABLE;

	HAL_OSPI_Command(&hospi1,&Cmdhandler,5000);
}

//QSPI����ָ�����ȵ�����
//buf:�������ݻ������׵�ַ
//datalen:Ҫ��������ݳ���
//����ֵ:0,����
//    ����,�������
uint8_t QSPI_Receive(uint8_t* buf,uint32_t datalen)
{
    hospi1.Instance->DLR=datalen-1;                           //�������ݳ���
    if(HAL_OSPI_Receive(&hospi1,buf,5000)==HAL_OK) return 0;  //��������
    else return 1;
}

//QSPI����ָ�����ȵ�����
//buf:�������ݻ������׵�ַ
//datalen:Ҫ��������ݳ���
//����ֵ:0,����
//    ����,�������
uint8_t QSPI_Transmit(uint8_t* buf,uint32_t datalen)
{
    hospi1.Instance->DLR=datalen-1;                            //�������ݳ���
    if(HAL_OSPI_Transmit(&hospi1,buf,5000)==HAL_OK) return 0;  //��������
    else return 1;
}
/* USER CODE END 1 */
