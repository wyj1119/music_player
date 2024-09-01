#include "rgb_lcd.h"
#include "ltdc.h"
#include "usart.h"
#include "font.h"

_ltdc_dev ltdc_dev;
extern __IO uint8_t frame_buf[480][800][3];
extern __IO uint8_t frame_buf1[480][800][3];

uint8_t color_test[3] = {0xff,0x00,0x00};
uint8_t color_back[3] = {0xff,0xff,0xff};

void lcd_upd(void *pvParameters) //������
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

//����ָ����
//layerx:���,0,��һ��; 1,�ڶ���; 2,ͬʱ������; 3,ͬʱ�ر�����
//sw:1 ��;0�ر�
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


//���㺯��
//x,y:����
//color:��ɫֵ
void LTDC_Draw_Point(uint16_t x,uint16_t y,uint8_t *color)
{
	if(ltdc_dev.dir)	//����
	{
		frame_buf[x][y][0] = color[0];
		frame_buf[x][y][1] = color[1];
		frame_buf[x][y][2] = color[2];
	}else 			//����
	{
		frame_buf[ltdc_dev.height - y - 1][x][0] = color[0];
		frame_buf[ltdc_dev.height - y - 1][x][1] = color[1];
		frame_buf[ltdc_dev.height - y - 1][x][2] = color[2];
	}
}

//���㺯��
//����ֵ:��ɫֵ
uint8_t *LTDC_Read_Point(uint16_t x,uint16_t y)
{
	if(ltdc_dev.dir)	//����
	{
		return (uint8_t *)frame_buf[x][y];		
	}else 			//����
	{
		return (uint8_t *)frame_buf[ltdc_dev.height - y - 1][x];
	}
}

//LTDC������,DMA2D���
//(sx,sy),(ex,ey):�����ζԽ�����,�����СΪ:(ex-sx+1)*(ey-sy+1)   
//ע��:sx,ex,���ܴ���lcddev.width-1;sy,ey,���ܴ���lcddev.height-1!!!
//color:Ҫ������ɫ
void LTDC_Fill(uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey,uint32_t color)
{ 
	uint32_t psx,psy,pex,pey;	//��LCD���Ϊ��׼������ϵ,����������仯���仯
	uint32_t timeout=0; 
	uint16_t offline;
	uint32_t addr; 
	//����ϵת��
	if(ltdc_dev.dir)	//����
	{
		psx=sx;psy=sy;
		pex=ex;pey=ey;
	}else			//����
	{
		USART_printf(&hlpuart1,"NO CONFIG DMA2D FOR ltdc_dev.dir==0\r\n");
		while(1);
//		psx=sy;psy=ltdc_dev.height-ex-1;
//		pex=ey;pey=ltdc_dev.height-sx-1;
	} 
	offline=ltdc_dev.width-(pex-psx+1);
	addr=((uint32_t)frame_buf[psy][psx]);
	RCC->AHB1ENR|=1<<23;			//ʹ��DM2Dʱ��
	DMA2D->CR=3<<16;				//�Ĵ������洢��ģʽ
	DMA2D->OPFCCR=LCD_PIXFORMAT;	//������ɫ��ʽ
	DMA2D->OOR=offline;				//������ƫ�� 
	DMA2D->CR&=~(1<<0);				//��ֹͣDMA2D
	DMA2D->OMAR=addr;				//����洢����ַ
	DMA2D->NLR=(pey-psy+1)|((pex-psx+1)<<16);	//�趨�����Ĵ���
	DMA2D->OCOLR=color;				//�趨�����ɫ�Ĵ��� 
	DMA2D->CR|=1<<0;				//����DMA2D
	while((DMA2D->ISR&(1<<1))==0)	//�ȴ��������
	{
		timeout++;
		if(timeout>0X1FFFFF)break;	//��ʱ�˳�
	}  
	DMA2D->IFCR|=1<<1;				//���������ɱ�־ 	
}

//��ָ�����������ָ����ɫ��,DMA2D���	
//�˺�����֧��uint16_t,RGB565��ʽ����ɫ�������.
//(sx,sy),(ex,ey):�����ζԽ�����,�����СΪ:(ex-sx+1)*(ey-sy+1)  
//ע��:sx,ex,���ܴ���lcddev.width-1;sy,ey,���ܴ���lcddev.height-1!!!
//color:Ҫ������ɫ�����׵�ַ
void LTDC_Color_Fill(uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey,uint8_t *color)
{
	uint32_t psx,psy,pex,pey;	//��LCD���Ϊ��׼������ϵ,����������仯���仯
	uint32_t timeout=0; 
	uint16_t offline;
	uint32_t addr; 
	//����ϵת��
	if(ltdc_dev.dir)	//����
	{
		psx=sx;psy=sy;
		pex=ex;pey=ey;
	}else			//����
	{
		USART_printf(&hlpuart1,"NO CONFIG DMA2D FOR ltdc_dev.dir==0\r\n");
		while(1);
//		psx=sy;psy=ltdc_dev.height-ex-1;
//		pex=ey;pey=ltdc_dev.height-sx-1;
	}
	offline=ltdc_dev.width-(pex-psx+1);
	addr=((uint32_t)frame_buf[psy][psx]);
	RCC->AHB1ENR|=1<<23;			//ʹ��DM2Dʱ��
	DMA2D->CR=0<<16;				//�洢�����洢��ģʽ
	DMA2D->FGPFCCR=LCD_PIXFORMAT;	//������ɫ��ʽ
	DMA2D->FGOR=0;					//ǰ������ƫ��Ϊ0
	DMA2D->OOR=offline;				//������ƫ�� 
	DMA2D->CR&=~(1<<0);				//��ֹͣDMA2D
	DMA2D->FGMAR=(uint32_t)color;		//Դ��ַ
	DMA2D->OMAR=addr;				//����洢����ַ
	DMA2D->NLR=(pey-psy+1)|((pex-psx+1)<<16);	//�趨�����Ĵ��� 
	DMA2D->CR|=1<<0;				//����DMA2D
	while((DMA2D->ISR&(1<<1))==0)	//�ȴ��������
	{
		timeout++;
		if(timeout>0X1FFFFF)break;	//��ʱ�˳�
	} 
	DMA2D->IFCR|=1<<1;				//���������ɱ�־  	
}

//LCD����
//color:��ɫֵ
void LTDC_Clear(uint32_t color)
{
	LTDC_Fill(0,0,ltdc_dev.width-1,ltdc_dev.height-1,color);
}


//����
//x1,y1:�������
//x2,y2:�յ�����
void LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,uint8_t *color)
{
    uint16_t t;
    int xerr = 0, yerr = 0, delta_x, delta_y, distance;
    int incx, incy, uRow, uCol;
    delta_x = x2 - x1; //������������
    delta_y = y2 - y1;
    uRow = x1;
    uCol = y1;

    if (delta_x > 0)incx = 1; //���õ�������
    else if (delta_x == 0)incx = 0; //��ֱ��
    else
    {
        incx = -1;
        delta_x = -delta_x;
    }

    if (delta_y > 0)incy = 1;
    else if (delta_y == 0)incy = 0; //ˮƽ��
    else
    {
        incy = -1;
        delta_y = -delta_y;
    }

    if ( delta_x > delta_y)distance = delta_x; //ѡȡ��������������
    else distance = delta_y;

    for (t = 0; t <= distance + 1; t++ ) //�������
    {
        LTDC_Draw_Point(uCol,uRow,color); //����
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
//������
//(x1,y1),(x2,y2):���εĶԽ�����
void LCD_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,uint8_t *color)
{
    LCD_DrawLine(x1, y1, x2, y1,color);
    LCD_DrawLine(x1, y1, x1, y2,color);
    LCD_DrawLine(x1, y2, x2, y2,color);
    LCD_DrawLine(x2, y1, x2, y2,color);
}
//��ָ��λ�û�һ��ָ����С��Բ
//(x,y):���ĵ�
//r    :�뾶
void LCD_Draw_Circle(uint16_t x0, uint16_t y0, uint8_t r,uint8_t *color)
{
    int a, b;
    int di;
		uint16_t x = y0;
		uint16_t y = x0;
    a = 0;
    b = r;
    di = 3 - (r << 1);       //�ж��¸���λ�õı�־

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

        //ʹ��Bresenham�㷨��Բ
        if (di < 0)di += 4 * a + 6;
        else
        {
            di += 10 + 4 * (a - b);
            b--;
        }
    }
}
//��ָ��λ����ʾһ���ַ�
//x,y:��ʼ����
//num:Ҫ��ʾ���ַ�:" "--->"~"
//size:�����С 12/16/24/32
//mode:���ӷ�ʽ(1)���Ƿǵ��ӷ�ʽ(0)
void LCD_ShowChar(uint16_t x, uint16_t y, uint8_t num, uint8_t size, uint8_t mode,uint8_t *color)
{
    uint8_t temp, t1, t;
    uint16_t y0 = y;
    uint8_t csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size / 2);		//�õ�����һ���ַ���Ӧ������ռ���ֽ���
    num = num - ' '; //�õ�ƫ�ƺ��ֵ��ASCII�ֿ��Ǵӿո�ʼȡģ������-' '���Ƕ�Ӧ�ַ����ֿ⣩

    for (t = 0; t < csize; t++)
    {
        if (size == 12)temp = asc2_1206[num][t]; 	 	//����1206����
        else if (size == 16)temp = asc2_1608[num][t];	//����1608����
        else if (size == 24)temp = asc2_2412[num][t];	//����2412����
        else if (size == 32)temp = asc2_3216[num][t];	//����3216����
        else return;								//û�е��ֿ�

        for (t1 = 0; t1 < 8; t1++)
        {
            if (temp & 0x80)LTDC_Draw_Point(y, x, color);
            else if (mode == 0)LTDC_Draw_Point(y, x, color_back);

            temp <<= 1;
            y++;

            if (y >= ltdc_dev.height)return;		//��������

            if ((y - y0) == size)
            {
                y = y0;
                x++;

                if (x >= ltdc_dev.width)return;	//��������

                break;
            }
        }
    }
}
//m^n����
//����ֵ:m^n�η�.
uint32_t LCD_Pow(uint8_t m, uint8_t n)
{
    uint32_t result = 1;

    while (n--)result *= m;

    return result;
}
//��ʾ����,��λΪ0,����ʾ
//x,y :�������
//len :���ֵ�λ��
//size:�����С
//color:��ɫ
//num:��ֵ(0~4294967295);
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
//��ʾ����,��λΪ0,������ʾ
//x,y:�������
//num:��ֵ(0~999999999);
//len:����(��Ҫ��ʾ��λ��)
//size:�����С
//mode:
//[7]:0,�����;1,���0.
//[6:1]:����
//[0]:0,�ǵ�����ʾ;1,������ʾ.
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
//��ʾ�ַ���
//x,y:�������
//width,height:�����С
//size:�����С
//*p:�ַ�����ʼ��ַ
void LCD_ShowString(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t size, uint8_t *p,uint8_t *color)
{
    uint8_t x0 = x;
    width += x;
    height += y;

    while ((*p <= '~') && (*p >= ' ')) //�ж��ǲ��ǷǷ��ַ�!
    {
        if (x >= width)
        {
            x = x0;
            y += size;
        }

        if (y >= height)break; //�˳�

        LCD_ShowChar(x, y, *p, size, 0,color);
        x += size / 2;
        p++;
    }
}


