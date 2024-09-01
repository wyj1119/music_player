#include "rgb_lcd.h"
#include "ltdc.h"
#include "usart.h"
#include "font.h"

_ltdc_dev ltdc_dev;
extern __IO uint8_t frame_buf[480][800][3];
extern __IO uint8_t frame_buf1[480][800][3];

uint8_t color_test[3] = {0xff,0x00,0x00};
uint8_t color_back[3] = {0xff,0xff,0xff};

void lcd_upd(void *pvParameters) //任务函数
{
	uint8_t str_temp[20] = "HelloWorld!!!";
	while(1)
	{
		
		LCD_DrawLine(0,0,799,479,color_test);
		LCD_DrawLine(799,0,0,479,color_test);
		LCD_DrawRectangle(10,10,100,100,color_test);
		LCD_Draw_Circle(399,239,50,color_test);
		LCD_ShowChar(50,50,'a',32,1,color_test);
		LCD_ShowString(100,100,16*sizeof(str_temp),32,32,str_temp,color_test);
		HAL_GPIO_TogglePin(LED_R_GPIO_Port,LED_R_Pin);
//		USART_printf(&hlpuart1,"lcd upd\r\n");
//		if(++sta>=5)sta = 0;
		vTaskDelay(500);
	}
}

void Init_ltdc_param(void)
{
	ltdc_dev.activelayer = 0;
	ltdc_dev.dir = 1;
	ltdc_dev.width = 800;
	ltdc_dev.height = 480;
	ltdc_dev.pixsize = 3;
}

//开关指定层
//layerx:层号,0,第一层; 1,第二层; 2,同时打开两层; 3,同时关闭两层
//sw:1 打开;0关闭
void LTDC_Layer_Switch(uint8_t layerx,uint8_t sw)
{
	if(sw==1) __HAL_LTDC_LAYER_ENABLE(&hltdc,layerx);
	else if(sw==0) __HAL_LTDC_LAYER_DISABLE(&hltdc,layerx);
	else if(sw==2)
	{
		__HAL_LTDC_LAYER_ENABLE(&hltdc,0);
		__HAL_LTDC_LAYER_ENABLE(&hltdc,1);
	}
	else if(sw==3)
	{
		__HAL_LTDC_LAYER_DISABLE(&hltdc,0);
		__HAL_LTDC_LAYER_DISABLE(&hltdc,1);
	}
	__HAL_LTDC_RELOAD_CONFIG(&hltdc);
	ltdc_dev.activelayer = layerx;
}


//画点函数
//x,y:坐标
//color:颜色值
void LTDC_Draw_Point(uint16_t x,uint16_t y,uint8_t *color)
{
	if(ltdc_dev.dir)	//横屏
	{
		frame_buf[x][y][0] = color[0];
		frame_buf[x][y][1] = color[1];
		frame_buf[x][y][2] = color[2];
	}else 			//竖屏
	{
		frame_buf[ltdc_dev.height - y - 1][x][0] = color[0];
		frame_buf[ltdc_dev.height - y - 1][x][1] = color[1];
		frame_buf[ltdc_dev.height - y - 1][x][2] = color[2];
	}
}

//读点函数
//返回值:颜色值
uint8_t *LTDC_Read_Point(uint16_t x,uint16_t y)
{
	if(ltdc_dev.dir)	//横屏
	{
		return (uint8_t *)frame_buf[x][y];		
	}else 			//竖屏
	{
		return (uint8_t *)frame_buf[ltdc_dev.height - y - 1][x];
	}
}

//LTDC填充矩形,DMA2D填充
//(sx,sy),(ex,ey):填充矩形对角坐标,区域大小为:(ex-sx+1)*(ey-sy+1)   
//注意:sx,ex,不能大于lcddev.width-1;sy,ey,不能大于lcddev.height-1!!!
//color:要填充的颜色
void LTDC_Fill(uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey,uint32_t color)
{ 
	uint32_t psx,psy,pex,pey;	//以LCD面板为基准的坐标系,不随横竖屏变化而变化
	uint32_t timeout=0; 
	uint16_t offline;
	uint32_t addr; 
	//坐标系转换
	if(ltdc_dev.dir)	//横屏
	{
		psx=sx;psy=sy;
		pex=ex;pey=ey;
	}else			//竖屏
	{
		USART_printf(&hlpuart1,"NO CONFIG DMA2D FOR ltdc_dev.dir==0\r\n");
		while(1);
//		psx=sy;psy=ltdc_dev.height-ex-1;
//		pex=ey;pey=ltdc_dev.height-sx-1;
	} 
	offline=ltdc_dev.width-(pex-psx+1);
	addr=((uint32_t)frame_buf[psy][psx]);
	RCC->AHB1ENR|=1<<23;			//使能DM2D时钟
	DMA2D->CR=3<<16;				//寄存器到存储器模式
	DMA2D->OPFCCR=LCD_PIXFORMAT;	//设置颜色格式
	DMA2D->OOR=offline;				//设置行偏移 
	DMA2D->CR&=~(1<<0);				//先停止DMA2D
	DMA2D->OMAR=addr;				//输出存储器地址
	DMA2D->NLR=(pey-psy+1)|((pex-psx+1)<<16);	//设定行数寄存器
	DMA2D->OCOLR=color;				//设定输出颜色寄存器 
	DMA2D->CR|=1<<0;				//启动DMA2D
	while((DMA2D->ISR&(1<<1))==0)	//等待传输完成
	{
		timeout++;
		if(timeout>0X1FFFFF)break;	//超时退出
	}  
	DMA2D->IFCR|=1<<1;				//清除传输完成标志 	
}

//在指定区域内填充指定颜色块,DMA2D填充	
//此函数仅支持uint16_t,RGB565格式的颜色数组填充.
//(sx,sy),(ex,ey):填充矩形对角坐标,区域大小为:(ex-sx+1)*(ey-sy+1)  
//注意:sx,ex,不能大于lcddev.width-1;sy,ey,不能大于lcddev.height-1!!!
//color:要填充的颜色数组首地址
void LTDC_Color_Fill(uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey,uint8_t *color)
{
	uint32_t psx,psy,pex,pey;	//以LCD面板为基准的坐标系,不随横竖屏变化而变化
	uint32_t timeout=0; 
	uint16_t offline;
	uint32_t addr; 
	//坐标系转换
	if(ltdc_dev.dir)	//横屏
	{
		psx=sx;psy=sy;
		pex=ex;pey=ey;
	}else			//竖屏
	{
		USART_printf(&hlpuart1,"NO CONFIG DMA2D FOR ltdc_dev.dir==0\r\n");
		while(1);
//		psx=sy;psy=ltdc_dev.height-ex-1;
//		pex=ey;pey=ltdc_dev.height-sx-1;
	}
	offline=ltdc_dev.width-(pex-psx+1);
	addr=((uint32_t)frame_buf[psy][psx]);
	RCC->AHB1ENR|=1<<23;			//使能DM2D时钟
	DMA2D->CR=0<<16;				//存储器到存储器模式
	DMA2D->FGPFCCR=LCD_PIXFORMAT;	//设置颜色格式
	DMA2D->FGOR=0;					//前景层行偏移为0
	DMA2D->OOR=offline;				//设置行偏移 
	DMA2D->CR&=~(1<<0);				//先停止DMA2D
	DMA2D->FGMAR=(uint32_t)color;		//源地址
	DMA2D->OMAR=addr;				//输出存储器地址
	DMA2D->NLR=(pey-psy+1)|((pex-psx+1)<<16);	//设定行数寄存器 
	DMA2D->CR|=1<<0;				//启动DMA2D
	while((DMA2D->ISR&(1<<1))==0)	//等待传输完成
	{
		timeout++;
		if(timeout>0X1FFFFF)break;	//超时退出
	} 
	DMA2D->IFCR|=1<<1;				//清除传输完成标志  	
}

//LCD清屏
//color:颜色值
void LTDC_Clear(uint32_t color)
{
	LTDC_Fill(0,0,ltdc_dev.width-1,ltdc_dev.height-1,color);
}


//画线
//x1,y1:起点坐标
//x2,y2:终点坐标
void LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,uint8_t *color)
{
    uint16_t t;
    int xerr = 0, yerr = 0, delta_x, delta_y, distance;
    int incx, incy, uRow, uCol;
    delta_x = x2 - x1; //计算坐标增量
    delta_y = y2 - y1;
    uRow = x1;
    uCol = y1;

    if (delta_x > 0)incx = 1; //设置单步方向
    else if (delta_x == 0)incx = 0; //垂直线
    else
    {
        incx = -1;
        delta_x = -delta_x;
    }

    if (delta_y > 0)incy = 1;
    else if (delta_y == 0)incy = 0; //水平线
    else
    {
        incy = -1;
        delta_y = -delta_y;
    }

    if ( delta_x > delta_y)distance = delta_x; //选取基本增量坐标轴
    else distance = delta_y;

    for (t = 0; t <= distance + 1; t++ ) //画线输出
    {
        LTDC_Draw_Point(uCol,uRow,color); //画点
        xerr += delta_x ;
        yerr += delta_y ;

        if (xerr > distance)
        {
            xerr -= distance;
            uRow += incx;
        }

        if (yerr > distance)
        {
            yerr -= distance;
            uCol += incy;
        }
    }
}
//画矩形
//(x1,y1),(x2,y2):矩形的对角坐标
void LCD_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,uint8_t *color)
{
    LCD_DrawLine(x1, y1, x2, y1,color);
    LCD_DrawLine(x1, y1, x1, y2,color);
    LCD_DrawLine(x1, y2, x2, y2,color);
    LCD_DrawLine(x2, y1, x2, y2,color);
}
//在指定位置画一个指定大小的圆
//(x,y):中心点
//r    :半径
void LCD_Draw_Circle(uint16_t x0, uint16_t y0, uint8_t r,uint8_t *color)
{
    int a, b;
    int di;
		uint16_t x = y0;
		uint16_t y = x0;
    a = 0;
    b = r;
    di = 3 - (r << 1);       //判断下个点位置的标志

    while (a <= b)
    {
        LTDC_Draw_Point(x + a, y - b,color);        //5
        LTDC_Draw_Point(x + b, y - a,color);        //0
        LTDC_Draw_Point(x + b, y + a,color);        //4
        LTDC_Draw_Point(x + a, y + b,color);        //6
        LTDC_Draw_Point(x - a, y + b,color);        //1
        LTDC_Draw_Point(x - b, y + a,color);
        LTDC_Draw_Point(x - a, y - b,color);        //2
        LTDC_Draw_Point(x - b, y - a,color);        //7
        a++;

        //使用Bresenham算法画圆
        if (di < 0)di += 4 * a + 6;
        else
        {
            di += 10 + 4 * (a - b);
            b--;
        }
    }
}
//在指定位置显示一个字符
//x,y:起始坐标
//num:要显示的字符:" "--->"~"
//size:字体大小 12/16/24/32
//mode:叠加方式(1)还是非叠加方式(0)
void LCD_ShowChar(uint16_t x, uint16_t y, uint8_t num, uint8_t size, uint8_t mode,uint8_t *color)
{
    uint8_t temp, t1, t;
    uint16_t y0 = y;
    uint8_t csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size / 2);		//得到字体一个字符对应点阵集所占的字节数
    num = num - ' '; //得到偏移后的值（ASCII字库是从空格开始取模，所以-' '就是对应字符的字库）

    for (t = 0; t < csize; t++)
    {
        if (size == 12)temp = asc2_1206[num][t]; 	 	//调用1206字体
        else if (size == 16)temp = asc2_1608[num][t];	//调用1608字体
        else if (size == 24)temp = asc2_2412[num][t];	//调用2412字体
        else if (size == 32)temp = asc2_3216[num][t];	//调用3216字体
        else return;								//没有的字库

        for (t1 = 0; t1 < 8; t1++)
        {
            if (temp & 0x80)LTDC_Draw_Point(y, x, color);
            else if (mode == 0)LTDC_Draw_Point(y, x, color_back);

            temp <<= 1;
            y++;

            if (y >= ltdc_dev.height)return;		//超区域了

            if ((y - y0) == size)
            {
                y = y0;
                x++;

                if (x >= ltdc_dev.width)return;	//超区域了

                break;
            }
        }
    }
}
//m^n函数
//返回值:m^n次方.
uint32_t LCD_Pow(uint8_t m, uint8_t n)
{
    uint32_t result = 1;

    while (n--)result *= m;

    return result;
}
//显示数字,高位为0,则不显示
//x,y :起点坐标
//len :数字的位数
//size:字体大小
//color:颜色
//num:数值(0~4294967295);
void LCD_ShowNum(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size,uint8_t *color)
{
    uint8_t t, temp;
    uint8_t enshow = 0;

    for (t = 0; t < len; t++)
    {
        temp = (num / LCD_Pow(10, len - t - 1)) % 10;

        if (enshow == 0 && t < (len - 1))
        {
            if (temp == 0)
            {
                LCD_ShowChar(x + (size / 2)*t, y, ' ', size, 0,color);
                continue;
            }
            else enshow = 1;

        }

        LCD_ShowChar(x + (size / 2)*t, y, temp + '0', size, 0,color);
    }
}
//显示数字,高位为0,还是显示
//x,y:起点坐标
//num:数值(0~999999999);
//len:长度(即要显示的位数)
//size:字体大小
//mode:
//[7]:0,不填充;1,填充0.
//[6:1]:保留
//[0]:0,非叠加显示;1,叠加显示.
void LCD_ShowxNum(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint8_t mode,uint8_t *color)
{
    uint8_t t, temp;
    uint8_t enshow = 0;

    for (t = 0; t < len; t++)
    {
        temp = (num / LCD_Pow(10, len - t - 1)) % 10;

        if (enshow == 0 && t < (len - 1))
        {
            if (temp == 0)
            {
                if (mode & 0X80)LCD_ShowChar(x + (size / 2)*t, y, '0', size, mode & 0X01,color);
                else LCD_ShowChar(x + (size / 2)*t, y, ' ', size, mode & 0X01,color);

                continue;
            }
            else enshow = 1;

        }

        LCD_ShowChar(x + (size / 2)*t, y, temp + '0', size, mode & 0X01,color);
    }
}
//显示字符串
//x,y:起点坐标
//width,height:区域大小
//size:字体大小
//*p:字符串起始地址
void LCD_ShowString(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t size, uint8_t *p,uint8_t *color)
{
    uint8_t x0 = x;
    width += x;
    height += y;

    while ((*p <= '~') && (*p >= ' ')) //判断是不是非法字符!
    {
        if (x >= width)
        {
            x = x0;
            y += size;
        }

        if (y >= height)break; //退出

        LCD_ShowChar(x, y, *p, size, 0,color);
        x += size / 2;
        p++;
    }
}


