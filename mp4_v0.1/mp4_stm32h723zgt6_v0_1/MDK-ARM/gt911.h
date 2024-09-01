#ifndef __GT911_H__
#define __GT911_H__
 
#include "main.h"
#include "wav_play.h"
//IO操作函数	 
#define GT911_ADDR 0X14

//GT9147 部分寄存器定义 
#define GT_CTRL_REG 	0X8040   	//GT911控制寄存器
#define GT_CFGS_REG 	0X8047   	//GT911配置起始地址寄存器
#define GT_CHECK_REG 	0X80FF   	//GT911校验和寄存器
#define GT_PID_REG 		0X8140   	//GT911产品ID寄存器

#define GT_GSTID_REG 	0X814E   	//GT911当前检测到的触摸情况
#define GT_TP1_REG 		0X8150  	//第一个触摸点数据地址
#define GT_TP2_REG 		0X8158		//第二个触摸点数据地址
#define GT_TP3_REG 		0X8160		//第三个触摸点数据地址
#define GT_TP4_REG 		0X8168		//第四个触摸点数据地址
#define GT_TP5_REG 		0X8170		//第五个触摸点数据地址  
 
#define GT_RST(n)    		HAL_GPIO_WritePin(T_RST_GPIO_Port,T_RST_Pin,n);  //GT911复位引脚
#define GT_INT(n)    		HAL_GPIO_WritePin(T_INT_GPIO_Port,T_INT_Pin,n);  //GT911中断引脚

//#define GT_INT    		HAL_GPIO_ReadPin(T_INT_GPIO_Port,T_INT_Pin);		//GT911中断引脚	

#define TP_PRES_DOWN 0x80  //触屏被按下	  
#define TP_CATH_PRES 0x40  //有按键按下了 
#define CT_MAX_TOUCH  5    //电容屏支持的点数,固定为5点
//触摸屏控制器
typedef struct
{
	uint8_t (*init)(void);			  //初始化触摸屏控制器
	uint8_t (*scan)(uint8_t);				  //扫描触摸屏.0,屏幕扫描;1,物理坐标;	 
	void (*adjust)(void);		//触摸屏校准 
	uint16_t x[CT_MAX_TOUCH]; 		//当前坐标
	uint16_t y[CT_MAX_TOUCH];		//电容屏有最多5组坐标,电阻屏则用x[0],y[0]代表:此次扫描时,触屏的坐标,用
								          //x[4],y[4]存储第一次按下时的坐标. 
	uint8_t  sta;					      //笔的状态 
								          //b7:按下1/松开0; 
	                        //b6:0,没有按键按下;1,有按键按下. 
								          //b5:保留
								          //b4~b0:电容触摸屏按下的点数(0,表示未按下,1表示按下)
/////////////////////触摸屏校准参数(电容屏不需要校准)//////////////////////								
	float xfac;					
	float yfac;
	short xoff;
	short yoff;	   
//新增的参数,当触摸屏的左右上下完全颠倒时需要用到.
//b0:0,竖屏(适合左右为X坐标,上下为Y坐标的TP)
//   1,横屏(适合左右为Y坐标,上下为X坐标的TP) 
//b1~6:保留.
//b7:0,电阻屏
//   1,电容屏 
	uint8_t touchtype;
}_m_tp_dev;


uint8_t GT911_Init(void);
uint8_t GT911_Scan(uint8_t mode);
#endif
