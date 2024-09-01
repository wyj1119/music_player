#ifndef __RGB_LCD_H__
#define __RGB_LCD_H__


#include "main.h"

#define LCD_LAYER0_ADDR 0xC0010000
#define LCD_LAYER1_ADDR 0xC0129400

#define LCD_PIXEL_FORMAT_ARGB8888       0X00    
#define LCD_PIXEL_FORMAT_RGB888         0X01    
#define LCD_PIXEL_FORMAT_RGB565         0X02       
#define LCD_PIXEL_FORMAT_ARGB1555       0X03      
#define LCD_PIXEL_FORMAT_ARGB4444       0X04     
#define LCD_PIXEL_FORMAT_L8             0X05     
#define LCD_PIXEL_FORMAT_AL44           0X06     
#define LCD_PIXEL_FORMAT_AL88           0X07      

#define RED 0XFFFF0000
#define GREEN 0XFF00FF00
#define BLUE 0XFF0000FF
#define WHITE 0XFFFFFFFF
#define BLACK 0XFF000000

///////////////////////////////////////////////////////////////////////
//�û��޸����ò���:

//������ɫ���ظ�ʽ,һ����RGB565
#define LCD_PIXFORMAT				LCD_PIXEL_FORMAT_RGB888	
//����Ĭ�ϱ�������ɫ
#define LTDC_BACKLAYERCOLOR			0X00000000	
//LCD֡�������׵�ַ,���ﶨ����SDRAM����.
#define LCD_FRAME_BUF_ADDR			LCD_LAYER0_ADDR  



//LCD LTDC��Ҫ������
typedef struct  
{
	uint16_t width;		//LCD���
	uint16_t height;	//LCD�߶�
	uint8_t pixsize;	//ÿ��������ռ�ֽ���
//	uint16_t hsw;			//ˮƽͬ�����
//	uint16_t vsw;			//��ֱͬ�����
//	uint16_t hbp;			//ˮƽ����
//	uint16_t vbp;			//��ֱ����
//	uint16_t hfp;			//ˮƽǰ��
//	uint16_t vfp;			//��ֱǰ�� 
	uint8_t activelayer;		//��ǰ����:0,��һ��; 1,�ڶ���; 2,ͬʱ������; 3,ͬʱ�ر�����
	uint8_t dir;				//0,����;1,����;
}_ltdc_dev; 


void lcd_upd(void *pvParameters); //������
void Init_ltdc_param(void);
void LTDC_Layer_Switch(uint8_t layerx,uint8_t sw);
void LTDC_Draw_Point(uint16_t x,uint16_t y,uint8_t *color);
uint8_t *LTDC_Read_Point(uint16_t x,uint16_t y);
void LTDC_Fill(uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey,uint32_t color);
void LTDC_Color_Fill(uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey,uint8_t *color);
void LTDC_Clear(uint32_t color);
void LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,uint8_t *color);
void LCD_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,uint8_t *color);
void LCD_Draw_Circle(uint16_t x0, uint16_t y0, uint8_t r,uint8_t *color);
void LCD_ShowChar(uint16_t x_input, uint16_t y_input, uint8_t num, uint8_t size, uint8_t mode,uint8_t *color);
void LCD_ShowNum(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size,uint8_t *color);
void LCD_ShowxNum(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint8_t mode,uint8_t *color);
void LCD_ShowString(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t size, uint8_t *p,uint8_t *color);
#endif

