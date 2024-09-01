#include "gt911.h"
#include "i2c.h"
#include "usart.h"
#include "rgb_lcd.h"
#include "sai.h"
#include "rng.h"
_m_tp_dev tp_dev = {0};	 	//������������touch.c���涨��

//��GT911д��һ������
//reg:��ʼ�Ĵ�����ַ
//buf:���ݻ�������
//len:д���ݳ���
//����ֵ:0,�ɹ�;1,ʧ��.
uint8_t GT911_WR_Reg(uint16_t reg,uint8_t *buf,uint8_t len)
{
	return HAL_I2C_Mem_Write(&hi2c1,GT911_ADDR<<1,reg,I2C_MEMADD_SIZE_16BIT,buf,len,0xffff);
}
//��GT911����һ������
//reg:��ʼ�Ĵ�����ַ
//buf:���ݻ�������
//len:�����ݳ���			  
uint8_t GT911_RD_Reg(uint16_t reg,uint8_t *buf,uint8_t len)
{
	return HAL_I2C_Mem_Read(&hi2c1,GT911_ADDR<<1,reg,I2C_MEMADD_SIZE_16BIT,buf,len,0xffff);
}

//��ʼ��GT911������
//����ֵ:0,��ʼ���ɹ�;1,��ʼ��ʧ�� 
uint8_t GT911_Init(void)
{
	uint8_t res = 0;
	uint8_t temp[5]; 
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = T_RST_Pin|T_INT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(T_RST_GPIO_Port, &GPIO_InitStruct);
	HAL_GPIO_WritePin(T_RST_GPIO_Port,T_RST_Pin|T_INT_Pin,1);

	
	GT_RST(0);				//��λ
	GT_INT(1);
	HAL_Delay(1);
 	GT_RST(1);				//�ͷŸ�λ 
 	HAL_Delay(10);
	
	
  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = T_INT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(T_INT_GPIO_Port, &GPIO_InitStruct);
	
 	HAL_Delay(100);
	res = GT911_RD_Reg(GT_PID_REG,temp,4);//��ȡ��ƷID
	temp[4]=0;
	USART_printf(&hlpuart1,"res = %u,CTP ID:%s\r\n",res,temp);	//��ӡID
	if(strcmp((char*)temp,"911")==0)//ID==911
	{
		temp[0]=0X02;			
		GT911_WR_Reg(GT_CTRL_REG,temp,1);//��λGT911
		GT911_RD_Reg(GT_CFGS_REG,temp,1);//��ȡGT_CFGS_REG�Ĵ���
		if(temp[0]<0X60)//Ĭ�ϰ汾�Ƚϵ�,��Ҫ����flash����
		{
			USART_printf(&hlpuart1,"Default Ver:%d\r\n",temp[0]);
		}
		HAL_Delay(10);
		temp[0]=0X00;	 
		GT911_WR_Reg(GT_CTRL_REG,temp,1);//������λ   
		
//		USART_printf(&hlpuart1,"Touch Init OK\r\n");	//��ӡID
		return 0;
	}
	else
		USART_printf(&hlpuart1,"Touch Init Failed\r\n");	//��ӡID
	return 1;
}
const uint16_t GT911_TPX_TBL[5]={GT_TP1_REG,GT_TP2_REG,GT_TP3_REG,GT_TP4_REG,GT_TP5_REG};
//ɨ�败����(���ò�ѯ��ʽ)
//mode:0,����ɨ��.
//����ֵ:��ǰ����״̬.
//0,�����޴���;1,�����д���
extern _ltdc_dev ltdc_dev;
uint8_t GT911_Scan(uint8_t mode)
{
	uint8_t buf[4];
	uint8_t i=0;
	uint8_t res=0;
	uint8_t temp;
	static uint8_t t=0;//���Ʋ�ѯ���,�Ӷ�����CPUռ���� 
//	USART_printf(&hlpuart1,"TOUCH\r\n");
	t++;
	if((t%10)==0||t<10)//����ʱ,ÿ����10��CTP_Scan�����ż��1��,�Ӷ���ʡCPUʹ����
	{
		taskENTER_CRITICAL(); //�����ٽ���
		GT911_RD_Reg(GT_GSTID_REG,&mode,1);//��ȡ�������״̬ 
		taskEXIT_CRITICAL(); //�˳��ٽ���
		
		//printf("mode:0x%x\r\n",mode);
		if((mode&0XF)&&((mode&0XF)<6))
		{
			temp=0XFF<<(mode&0XF);//����ĸ���ת��Ϊ1��λ��,ƥ��tp_dev.sta���� 
			tp_dev.sta=(~temp)|TP_PRES_DOWN|TP_CATH_PRES; 
			for(i=0;i<5;i++)
			{
				if(tp_dev.sta&(1<<i))	//������Ч?
				{
					taskENTER_CRITICAL(); //�����ٽ���
					GT911_RD_Reg(GT911_TPX_TBL[i],buf,4);	//��ȡXY����ֵ
					taskEXIT_CRITICAL(); //�˳��ٽ���
					tp_dev.x[i]=((uint16_t)buf[1]<<8)+buf[0];
					tp_dev.y[i]=(((uint16_t)buf[3]<<8)+buf[2]);
					
//					USART_printf(&hlpuart1,"x[%d]:%d,y[%d]:%d\r\n",i,tp_dev.x[i],i,tp_dev.y[i]);//���ڴ�ӡ���꣬���ڵ���
				}			
			} 
			res=1;
			if(tp_dev.x[0]==0 && tp_dev.y[0]==0)mode=0;	//���������ݶ���0,����Դ˴�����
			t=0;		//����һ��,��������������10��,�Ӷ����������
		}
 		if(mode&0X80&&((mode&0XF)<6))
		{
			temp=0;
			taskENTER_CRITICAL(); //�����ٽ���
			GT911_WR_Reg(GT_GSTID_REG,&temp,1);//���־ 	
			taskEXIT_CRITICAL(); //�˳��ٽ���
		}
	}
	if((mode&0X8F)==0X80)//�޴����㰴��
	{
		if(tp_dev.sta&TP_PRES_DOWN)	//֮ǰ�Ǳ����µ�
		{
			tp_dev.sta&=~(1<<7);	//��ǰ����ɿ�
		}else						//֮ǰ��û�б�����
		{
			tp_dev.x[0]=0xffff;
			tp_dev.y[0]=0xffff;
			tp_dev.sta&=0XE0;	//�������Ч���	
		}	 
	}
	if(t>240)t=10;//���´�10��ʼ����
	return res;
}




