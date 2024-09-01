#include "gt911.h"
#include "i2c.h"
#include "usart.h"
#include "rgb_lcd.h"
#include "sai.h"
#include "rng.h"
_m_tp_dev tp_dev = {0};	 	//触屏控制器在touch.c里面定义

//向GT911写入一次数据
//reg:起始寄存器地址
//buf:数据缓缓存区
//len:写数据长度
//返回值:0,成功;1,失败.
uint8_t GT911_WR_Reg(uint16_t reg,uint8_t *buf,uint8_t len)
{
	return HAL_I2C_Mem_Write(&hi2c1,GT911_ADDR<<1,reg,I2C_MEMADD_SIZE_16BIT,buf,len,0xffff);
}
//从GT911读出一次数据
//reg:起始寄存器地址
//buf:数据缓缓存区
//len:读数据长度			  
uint8_t GT911_RD_Reg(uint16_t reg,uint8_t *buf,uint8_t len)
{
	return HAL_I2C_Mem_Read(&hi2c1,GT911_ADDR<<1,reg,I2C_MEMADD_SIZE_16BIT,buf,len,0xffff);
}

//初始化GT911触摸屏
//返回值:0,初始化成功;1,初始化失败 
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

	
	GT_RST(0);				//复位
	GT_INT(1);
	HAL_Delay(1);
 	GT_RST(1);				//释放复位 
 	HAL_Delay(10);
	
	
  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = T_INT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(T_INT_GPIO_Port, &GPIO_InitStruct);
	
 	HAL_Delay(100);
	res = GT911_RD_Reg(GT_PID_REG,temp,4);//读取产品ID
	temp[4]=0;
	USART_printf(&hlpuart1,"res = %u,CTP ID:%s\r\n",res,temp);	//打印ID
	if(strcmp((char*)temp,"911")==0)//ID==911
	{
		temp[0]=0X02;			
		GT911_WR_Reg(GT_CTRL_REG,temp,1);//软复位GT911
		GT911_RD_Reg(GT_CFGS_REG,temp,1);//读取GT_CFGS_REG寄存器
		if(temp[0]<0X60)//默认版本比较低,需要更新flash配置
		{
			USART_printf(&hlpuart1,"Default Ver:%d\r\n",temp[0]);
		}
		HAL_Delay(10);
		temp[0]=0X00;	 
		GT911_WR_Reg(GT_CTRL_REG,temp,1);//结束复位   
		
//		USART_printf(&hlpuart1,"Touch Init OK\r\n");	//打印ID
		return 0;
	}
	else
		USART_printf(&hlpuart1,"Touch Init Failed\r\n");	//打印ID
	return 1;
}
const uint16_t GT911_TPX_TBL[5]={GT_TP1_REG,GT_TP2_REG,GT_TP3_REG,GT_TP4_REG,GT_TP5_REG};
//扫描触摸屏(采用查询方式)
//mode:0,正常扫描.
//返回值:当前触屏状态.
//0,触屏无触摸;1,触屏有触摸
extern _ltdc_dev ltdc_dev;
uint8_t GT911_Scan(uint8_t mode)
{
	uint8_t buf[4];
	uint8_t i=0;
	uint8_t res=0;
	uint8_t temp;
	static uint8_t t=0;//控制查询间隔,从而降低CPU占用率 
//	USART_printf(&hlpuart1,"TOUCH\r\n");
	t++;
	if((t%10)==0||t<10)//空闲时,每进入10次CTP_Scan函数才检测1次,从而节省CPU使用率
	{
		taskENTER_CRITICAL(); //进入临界区
		GT911_RD_Reg(GT_GSTID_REG,&mode,1);//读取触摸点的状态 
		taskEXIT_CRITICAL(); //退出临界区
		
		//printf("mode:0x%x\r\n",mode);
		if((mode&0XF)&&((mode&0XF)<6))
		{
			temp=0XFF<<(mode&0XF);//将点的个数转换为1的位数,匹配tp_dev.sta定义 
			tp_dev.sta=(~temp)|TP_PRES_DOWN|TP_CATH_PRES; 
			for(i=0;i<5;i++)
			{
				if(tp_dev.sta&(1<<i))	//触摸有效?
				{
					taskENTER_CRITICAL(); //进入临界区
					GT911_RD_Reg(GT911_TPX_TBL[i],buf,4);	//读取XY坐标值
					taskEXIT_CRITICAL(); //退出临界区
					tp_dev.x[i]=((uint16_t)buf[1]<<8)+buf[0];
					tp_dev.y[i]=(((uint16_t)buf[3]<<8)+buf[2]);
					
//					USART_printf(&hlpuart1,"x[%d]:%d,y[%d]:%d\r\n",i,tp_dev.x[i],i,tp_dev.y[i]);//串口打印坐标，用于调试
				}			
			} 
			res=1;
			if(tp_dev.x[0]==0 && tp_dev.y[0]==0)mode=0;	//读到的数据都是0,则忽略此次数据
			t=0;		//触发一次,则会最少连续监测10次,从而提高命中率
		}
 		if(mode&0X80&&((mode&0XF)<6))
		{
			temp=0;
			taskENTER_CRITICAL(); //进入临界区
			GT911_WR_Reg(GT_GSTID_REG,&temp,1);//清标志 	
			taskEXIT_CRITICAL(); //退出临界区
		}
	}
	if((mode&0X8F)==0X80)//无触摸点按下
	{
		if(tp_dev.sta&TP_PRES_DOWN)	//之前是被按下的
		{
			tp_dev.sta&=~(1<<7);	//标记按键松开
		}else						//之前就没有被按下
		{
			tp_dev.x[0]=0xffff;
			tp_dev.y[0]=0xffff;
			tp_dev.sta&=0XE0;	//清除点有效标记	
		}	 
	}
	if(t>240)t=10;//重新从10开始计数
	return res;
}




