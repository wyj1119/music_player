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
//用户修改配置部分:

//定义颜色像素格式,一般用RGB565
#define LCD_PIXFORMAT				LCD_PIXEL_FORMAT_RGB888	
//定义默认背景层颜色
#define LTDC_BACKLAYERCOLOR			0X00000000	
//LCD帧缓冲区首地址,这里定义在SDRAM里面.
#define LCD_FRAME_BUF_ADDR			LCD_LAYER0_ADDR  



//LCD LTDC重要参数集
typedef struct  
{
	uint16_t width;		//LCD宽度
	uint16_t height;	//LCD高度
	uint8_t pixsize;	//每个像素所占字节数
//	uint16_t hsw;			//水平同步宽度
//	uint16_t vsw;			//垂直同步宽度
//	uint16_t hbp;			//水平后廊
//	uint16_t vbp;			//垂直后廊
//	uint16_t hfp;			//水平前廊
//	uint16_t vfp;			//垂直前廊 
	uint8_t activelayer;		//当前层编号:0,第一层; 1,第二层; 2,同时打开两层; 3,同时关闭两层
	uint8_t dir;				//0,竖屏;1,横屏;
}_ltdc_dev; 


void lcd_upd(void *pvParameters); //任务函数
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

