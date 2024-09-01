#ifndef __GT911_H__
#define __GT911_H__
 
#include "main.h"
#include "wav_play.h"
//IO��������	 
#define GT911_ADDR 0X14

//GT9147 ���ּĴ������� 
#define GT_CTRL_REG 	0X8040   	//GT911���ƼĴ���
#define GT_CFGS_REG 	0X8047   	//GT911������ʼ��ַ�Ĵ���
#define GT_CHECK_REG 	0X80FF   	//GT911У��ͼĴ���
#define GT_PID_REG 		0X8140   	//GT911��ƷID�Ĵ���

#define GT_GSTID_REG 	0X814E   	//GT911��ǰ��⵽�Ĵ������
#define GT_TP1_REG 		0X8150  	//��һ�����������ݵ�ַ
#define GT_TP2_REG 		0X8158		//�ڶ������������ݵ�ַ
#define GT_TP3_REG 		0X8160		//���������������ݵ�ַ
#define GT_TP4_REG 		0X8168		//���ĸ����������ݵ�ַ
#define GT_TP5_REG 		0X8170		//��������������ݵ�ַ  
 
#define GT_RST(n)    		HAL_GPIO_WritePin(T_RST_GPIO_Port,T_RST_Pin,n);  //GT911��λ����
#define GT_INT(n)    		HAL_GPIO_WritePin(T_INT_GPIO_Port,T_INT_Pin,n);  //GT911�ж�����

//#define GT_INT    		HAL_GPIO_ReadPin(T_INT_GPIO_Port,T_INT_Pin);		//GT911�ж�����	

#define TP_PRES_DOWN 0x80  //����������	  
#define TP_CATH_PRES 0x40  //�а��������� 
#define CT_MAX_TOUCH  5    //������֧�ֵĵ���,�̶�Ϊ5��
//������������
typedef struct
{
	uint8_t (*init)(void);			  //��ʼ��������������
	uint8_t (*scan)(uint8_t);				  //ɨ�败����.0,��Ļɨ��;1,��������;	 
	void (*adjust)(void);		//������У׼ 
	uint16_t x[CT_MAX_TOUCH]; 		//��ǰ����
	uint16_t y[CT_MAX_TOUCH];		//�����������5������,����������x[0],y[0]����:�˴�ɨ��ʱ,����������,��
								          //x[4],y[4]�洢��һ�ΰ���ʱ������. 
	uint8_t  sta;					      //�ʵ�״̬ 
								          //b7:����1/�ɿ�0; 
	                        //b6:0,û�а�������;1,�а�������. 
								          //b5:����
								          //b4~b0:���ݴ��������µĵ���(0,��ʾδ����,1��ʾ����)
/////////////////////������У׼����(����������ҪУ׼)//////////////////////								
	float xfac;					
	float yfac;
	short xoff;
	short yoff;	   
//�����Ĳ���,��������������������ȫ�ߵ�ʱ��Ҫ�õ�.
//b0:0,����(�ʺ�����ΪX����,����ΪY�����TP)
//   1,����(�ʺ�����ΪY����,����ΪX�����TP) 
//b1~6:����.
//b7:0,������
//   1,������ 
	uint8_t touchtype;
}_m_tp_dev;


uint8_t GT911_Init(void);
uint8_t GT911_Scan(uint8_t mode);
#endif
