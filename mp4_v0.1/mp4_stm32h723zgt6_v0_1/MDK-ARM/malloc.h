#ifndef __MALLOC_H
#define __MALLOC_H
#include "main.h" 
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32H7开发板
//内存管理 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2019/5/10
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved  
//********************************************************************************
//修改说明
//无
////////////////////////////////////////////////////////////////////////////////// 	 

 
#ifndef NULL
#define NULL 0
#endif

//定义六个内存池
//#define SRAMIN	 	0		//AXI内存池,AXI共512KB 
#define SRAMEX   	0		//外部内存池(SDRAM),SDRAM共32MB
//#define SRAM12		2		//SRAM1/2内存池,SRAM1+SRAM2,共256KB
//#define SRAM4		3		//SRAM4内存池,SRAM4共64KB
//#define SRAMDTCM 	4		//DTCM内存池,DTCM共128KB,此部分内存仅CPU和MDMA(通过AHBS)可以访问!!!!
//#define SRAMITCM 	5		//ITCM内存池,DTCM共64 KB,此部分内存仅CPU和MDMA(通过AHBS)可以访问!!!!


#define SRAMBANK 	1		//定义支持的SRAM块数.	


//mem1内存参数设定.mem1是H7内部的AXI内存.
//#define MEM1_BLOCK_SIZE			64  	  						//内存块大小为64字节
//#define MEM1_MAX_SIZE			448*1024  						//最大管理内存 448K,H7的AXI内存总共512KB
//#define MEM1_ALLOC_TABLE_SIZE	MEM1_MAX_SIZE/MEM1_BLOCK_SIZE 	//内存表大小

//mem2内存参数设定.mem2是外部的SDRAM内存
#define MEM2_BLOCK_SIZE			64  	  						//内存块大小为64字节
#define MEM2_MAX_SIZE			8192*1024  					//最大管理内存59753K,外扩SDRAM总共64MB,LTDC占了2MB,还剩62MB.
#define MEM2_ALLOC_TABLE_SIZE	MEM2_MAX_SIZE/MEM2_BLOCK_SIZE 	//内存表大小
		 
////mem3内存参数设定.mem3是H7内部的SRAM1+SRAM2内存
//#define MEM3_BLOCK_SIZE			64  	  						//内存块大小为64字节
//#define MEM3_MAX_SIZE			240 *1024  						//最大管理内存240K,H7的SRAM1+SRAM2共256KB
//#define MEM3_ALLOC_TABLE_SIZE	MEM3_MAX_SIZE/MEM3_BLOCK_SIZE 	//内存表大小
//		 
////mem4内存参数设定.mem4是H7内部的SRAM4内存
//#define MEM4_BLOCK_SIZE			64  	  						//内存块大小为64字节
//#define MEM4_MAX_SIZE			60 *1024  						//最大管理内存60K,H7的SRAM4共64KB
//#define MEM4_ALLOC_TABLE_SIZE	MEM4_MAX_SIZE/MEM4_BLOCK_SIZE 	//内存表大小
//		 
////mem5内存参数设定.mem5是H7内部的DTCM内存,此部分内存仅CPU和MDMA可以访问!!!!!!
//#define MEM5_BLOCK_SIZE			64  	  						//内存块大小为64字节
//#define MEM5_MAX_SIZE			120 *1024  						//最大管理内存120K,H7的DTCM共128KB
//#define MEM5_ALLOC_TABLE_SIZE	MEM5_MAX_SIZE/MEM5_BLOCK_SIZE 	//内存表大小
//		 
////mem6内存参数设定.mem6是H7内部的ITCM内存,此部分内存仅CPU和MDMA可以访问!!!!!!
//#define MEM6_BLOCK_SIZE			64  	  						//内存块大小为64字节
//#define MEM6_MAX_SIZE			60 *1024  						//最大管理内存60K,H7的ITCM共64KB
//#define MEM6_ALLOC_TABLE_SIZE	MEM6_MAX_SIZE/MEM6_BLOCK_SIZE 	//内存表大小
//		 


//内存管理控制器
struct _m_mallco_dev
{
	void (*init)(uint8_t);					//初始化
	uint16_t (*perused)(uint8_t);		  	    	//内存使用率
	uint8_t 	*membase[SRAMBANK];				//内存池 管理SRAMBANK个区域的内存
	uint32_t *memmap[SRAMBANK]; 				//内存管理状态表
	uint8_t  memrdy[SRAMBANK]; 				//内存管理是否就绪
};
extern struct _m_mallco_dev mallco_dev;	 //在mallco.c里面定义

void mymemset(void *s,uint8_t c,uint32_t count);	//设置内存
void mymemcpy(void *des,void *src,uint32_t n);//复制内存     
void my_mem_init(uint8_t memx);				//内存管理初始化函数(外/内部调用)
uint32_t my_mem_malloc(uint8_t memx,uint32_t size);	//内存分配(内部调用)
uint8_t my_mem_free(uint8_t memx,uint32_t offset);		//内存释放(内部调用)
uint16_t my_mem_perused(uint8_t memx) ;			//获得内存使用率(外/内部调用) 
////////////////////////////////////////////////////////////////////////////////
//用户调用函数
void myfree(uint8_t memx,void *ptr);  			//内存释放(外部调用)
void *mymalloc(uint8_t memx,uint32_t size);			//内存分配(外部调用)
void *myrealloc(uint8_t memx,void *ptr,uint32_t size);//重新分配内存(外部调用)
void *lv_mymalloc(uint32_t size);
void lv_myfree(void *ptr);
void *lv_myrealloc(void *ptr,uint32_t size);
#endif













