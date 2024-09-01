#ifndef __MALLOC_H
#define __MALLOC_H
#include "main.h" 
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32H7������
//�ڴ���� ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2019/5/10
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved  
//********************************************************************************
//�޸�˵��
//��
////////////////////////////////////////////////////////////////////////////////// 	 

 
#ifndef NULL
#define NULL 0
#endif

//���������ڴ��

#define SRAMEX   	0		//�ⲿ�ڴ��(SDRAM),SDRAM��32MB

#define SRAMBANK 	1		//����֧�ֵ�SRAM����.	




//mem2�ڴ�����趨.mem2���ⲿ��SDRAM�ڴ�
#define MEM2_BLOCK_SIZE			32  	  						//�ڴ���СΪ64�ֽ�
#define MEM2_MAX_SIZE			2UL*1024UL *1024UL  					//�������ڴ�59753K,����SDRAM�ܹ�64MB,LTDCռ��2MB,��ʣ62MB.
#define MEM2_ALLOC_TABLE_SIZE	MEM2_MAX_SIZE/MEM2_BLOCK_SIZE 	//�ڴ���С
		 


//�ڴ���������
struct _m_mallco_dev
{
	void (*init)(uint8_t);					//��ʼ��
	uint16_t (*perused)(uint8_t);		  	    	//�ڴ�ʹ����
	uint8_t 	*membase[SRAMBANK];				//�ڴ�� ����SRAMBANK��������ڴ�
	uint32_t *memmap[SRAMBANK]; 				//�ڴ����״̬��
	uint8_t  memrdy[SRAMBANK]; 				//�ڴ�����Ƿ����
};
extern struct _m_mallco_dev mallco_dev;	 //��mallco.c���涨��

void mymemset(void *s,uint8_t c,uint32_t count);	//�����ڴ�
void mymemcpy(void *des,void *src,uint32_t n);//�����ڴ�     
void my_mem_init(uint8_t memx);				//�ڴ�����ʼ������(��/�ڲ�����)
uint32_t my_mem_malloc(uint8_t memx,uint32_t size);	//�ڴ����(�ڲ�����)
uint8_t my_mem_free(uint8_t memx,uint32_t offset);		//�ڴ��ͷ�(�ڲ�����)
uint16_t my_mem_perused(uint8_t memx) ;			//����ڴ�ʹ����(��/�ڲ�����) 
////////////////////////////////////////////////////////////////////////////////
//�û����ú���
void myfree(uint8_t memx,void *ptr);  			//�ڴ��ͷ�(�ⲿ����)
void *mymalloc(uint8_t memx,uint32_t size);			//�ڴ����(�ⲿ����)
void *myrealloc(uint8_t memx,void *ptr,uint32_t size);//���·����ڴ�(�ⲿ����)

void *my_lv_malloc(uint32_t size);
void my_lv_free(void *data);
void *my_lv_realloc(void * data_p, size_t new_size);
#endif













