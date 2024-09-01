#include "wav_play.h"
#include "usart.h"
#include "dma.h"
#include "sai.h"
#include "rng.h"


/*
 *return
 *0-ok
 *11-RIFF wrong
 *12-WAVE wrong
 *13-fmt  wrong
 *14-data wrong
 */
 
#define WAV_HEAD_BUF_SIZE 512
uint8_t *my_strstr(uint8_t *src,uint32_t target)
{
	uint32_t i=0;
	for(i=0;i<WAV_HEAD_BUF_SIZE;i++)
	{
//		USART_printf(&hlpuart1,"%x\r\n",*(uint32_t *)(src+i));
		if(*(uint32_t *)(src+i) == target)
			return (src+i);
	}
	return NULL;
}

uint8_t get_wav_head(WavPlay *wav_play)
{
	uint8_t res=0;

	uint8_t sd_rbuf[WAV_HEAD_BUF_SIZE] = {0};
	uint8_t *p = sd_rbuf;
	
	taskENTER_CRITICAL(); //进入临界区
	res = f_open(&wav_play->WAV_FIL,(char *)wav_play->file_name, FA_READ);
	if(res!=FR_OK)return res;
	res = f_lseek(&wav_play->WAV_FIL, 0);//-------------------------------------------一定要加这行
	if(res!=FR_OK)return res;
	res = f_read(&wav_play->WAV_FIL,sd_rbuf,WAV_HEAD_BUF_SIZE,&wav_play->br_wav);
	if(res!=FR_OK)return res;
	res = f_close(&wav_play->WAV_FIL);
	if(res!=FR_OK)return res;
	taskEXIT_CRITICAL(); //退出临界区
	
	p = my_strstr(sd_rbuf,0X46464952);
	if(!p)return 11;
//ChunkRIFF	
	wav_play->wavhead.RIFF.ChunkID = *(uint32_t *)p;
	p+=sizeof(wav_play->wavhead.RIFF.ChunkID);
	wav_play->wavhead.RIFF.ChunkSize = *(uint32_t *)p;
	p+=sizeof(wav_play->wavhead.RIFF.ChunkSize);
	wav_play->wavhead.RIFF.Format = *(uint32_t *)p;
	if(wav_play->wavhead.RIFF.Format!=0X45564157)return 12;

	
	p = my_strstr(sd_rbuf,0X20746D66);
	if(!p)return 13;

//ChunkFMT
	wav_play->wavhead.FMT.ChunkID = *(uint32_t *)p;
	p+=sizeof(wav_play->wavhead.FMT.ChunkID);
	wav_play->wavhead.FMT.ChunkSize = *(uint32_t *)p;
	p+=sizeof(wav_play->wavhead.FMT.ChunkSize);
	wav_play->wavhead.FMT.AudioFormat = *(uint16_t *)p;
	p+=sizeof(wav_play->wavhead.FMT.AudioFormat);
	wav_play->wavhead.FMT.NumofChannels = *(uint16_t *)p;
	p+=sizeof(wav_play->wavhead.FMT.NumofChannels);
	wav_play->wavhead.FMT.SampleRate = *(uint32_t *)p;
	p+=sizeof(wav_play->wavhead.FMT.SampleRate);
	wav_play->wavhead.FMT.ByteRate = *(uint32_t *)p;
	p+=sizeof(wav_play->wavhead.FMT.ByteRate);
	wav_play->wavhead.FMT.BlockAlign = *(uint16_t *)p;
	p+=sizeof(wav_play->wavhead.FMT.BlockAlign);
	wav_play->wavhead.FMT.BitsPerSample = *(uint16_t *)p;
	p+=sizeof(wav_play->wavhead.FMT.BitsPerSample);
	
	p = my_strstr(sd_rbuf,0X61746164);
	if(!p)return 14;
	
//Chunkdata;
	wav_play->wavhead.Chunk_data.ChunkID = *(uint32_t *)p;
	p+=sizeof(wav_play->wavhead.Chunk_data.ChunkID);
	wav_play->wavhead.Chunk_data.ChunkSize = *(uint32_t *)p;
	p+=sizeof(wav_play->wavhead.Chunk_data.ChunkSize);
	
	wav_play->file_ptr = (uint32_t)(p-sd_rbuf);//文件头结束位置
	return 0;
}

/*
 *@param
 *0--44.1khz 
 *1--48khz
 */
void my_SystemClock_Config(uint32_t audio_freq)
{
   RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
	 __HAL_RCC_SAI1_CLK_DISABLE();
	 
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_SAI1;
    PeriphClkInitStruct.PLL2.PLL2M = 25;
    PeriphClkInitStruct.PLL2.PLL2P = 2;
    PeriphClkInitStruct.PLL2.PLL2Q = 2;
    PeriphClkInitStruct.PLL2.PLL2R = 2;
    PeriphClkInitStruct.PLL2.PLL2RGE = RCC_PLL2VCIRANGE_0;
    PeriphClkInitStruct.PLL2.PLL2VCOSEL = RCC_PLL2VCOMEDIUM;
    PeriphClkInitStruct.Sai1ClockSelection = RCC_SAI1CLKSOURCE_PLL2;
	 
		if(audio_freq == 0XAC44)//44.1KHZ
		{
			PeriphClkInitStruct.PLL2.PLL2N = 180;
			PeriphClkInitStruct.PLL2.PLL2FRACN = 5191.0;
		}
		else if(audio_freq == 0XBB80)//48KHZ
		{
			PeriphClkInitStruct.PLL2.PLL2N = 180;
			PeriphClkInitStruct.PLL2.PLL2FRACN = 5191.0;
		}
		else 
			USART_printf(&hlpuart1,"error audio fre\r\n");
		
		USART_printf(&hlpuart1,"audio fre:%u\r\n",audio_freq);
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      Error_Handler();
    }
		__HAL_RCC_SAI1_CLK_ENABLE();
}


void Init_ProgressBar(ProgressBar *pbar,uint32_t AudioSizeByte,uint32_t ByteRate)
{
	pbar->now_time_byte = 0;
	pbar->now_time_s = 0;
	pbar->total_time_byte = AudioSizeByte;//音频总字节大小
	pbar->total_time_s = AudioSizeByte/ByteRate + 0.5;
	
}


void Init_wav_play(WavPlay *wav_play,const char *songname)
{
	uint8_t res = 0;
	
	memset(wav_play->file_name,0,sizeof(wav_play->file_name));
	memcpy(wav_play->file_name,songname,strlen(songname));
	

	sprintf((char *)wav_play->file_name,"music/%s",songname);//给文件加上music文件夹的路径
	USART_printf(&hlpuart1,"%s\r\n",wav_play->file_name);
	res = get_wav_head(wav_play);	
	if(res)
	{
		USART_printf(&hlpuart1,"error get_wav_head : %u\r\n",res);
		return;
	}
	
	Init_ProgressBar(&wav_play->pbar,wav_play->wavhead.Chunk_data.ChunkSize,wav_play->wavhead.FMT.ByteRate);
	
	
	taskENTER_CRITICAL(); //进入临界区
	res = f_open(&wav_play->WAV_FIL,(char *)wav_play->file_name,FA_READ);
	res = f_lseek(&wav_play->WAV_FIL, wav_play->file_ptr);//跳过文件头
	res = f_read(&wav_play->WAV_FIL,wav_play->buf0.buf_u8,wav_play->wavhead.FMT.SampleRate/10,&wav_play->br_wav);
	wav_play->file_ptr+=wav_play->br_wav;
	tune_volume(wav_play);
	wav_play->buf_sta = 1;
	
	res = f_lseek(&wav_play->WAV_FIL, wav_play->file_ptr);
	res = f_read(&wav_play->WAV_FIL,wav_play->buf1.buf_u8,wav_play->wavhead.FMT.SampleRate/10,&wav_play->br_wav);
	wav_play->file_ptr+=wav_play->br_wav;
	tune_volume(wav_play);
	wav_play->buf_sta = 0;
	taskEXIT_CRITICAL(); //退出临界区

	Init_wav_SAI_DMA();
}


extern WavPlay wav_play;
extern QueueHandle_t send_buf_finish_handle;//I2S的DMA传送完一帧
static void I2S_DMATxCplt_m0_myuse(DMA_HandleTypeDef *hdma)
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	BaseType_t res;
	wav_play.buf_sta = 0;
	
	res = xSemaphoreGiveFromISR(send_buf_finish_handle,&xHigherPriorityTaskWoken);
	if(res==pdFALSE)
		USART_printf(&hlpuart1,"I2S_DMATxCplt_m0_myuse / xSemaphoreGiveFromISR error\r\n");
	portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}

static void I2S_DMATxCplt_m1_myuse(DMA_HandleTypeDef *hdma)
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	BaseType_t res;
	wav_play.buf_sta = 1;
	
	res = xSemaphoreGiveFromISR(send_buf_finish_handle,&xHigherPriorityTaskWoken);
	if(res==pdFALSE)
		USART_printf(&hlpuart1,"I2S_DMATxCplt_m1_myuse / xSemaphoreGiveFromISR error\r\n");
	portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}


HAL_StatusTypeDef HAL_SAI_Transmit_DMA_myuse(SAI_HandleTypeDef *hsai)
{
	hsai->hdmatx->XferCpltCallback = I2S_DMATxCplt_m0_myuse;
	hsai->hdmatx->XferM1CpltCallback = I2S_DMATxCplt_m1_myuse;

	/* Enable SAI Tx DMA Request */
	hsai->Instance->CR1 |= SAI_xCR1_DMAEN;

	/* Check if the SAI is already enabled */
	if ((hsai->Instance->CR1 & SAI_xCR1_SAIEN) == 0U)
	{
		/* Enable SAI peripheral */
		__HAL_SAI_ENABLE(hsai);
	}

	return HAL_OK;
}

void Init_wav_SAI_DMA(void)
{
	HAL_SAI_DeInit(&hsai_BlockA1);
	HAL_DMA_DeInit(&hdma_sai1_a);
	MX_DMA_Init();
	my_MX_SAI1_Init(wav_play.wavhead.FMT.SampleRate);
	my_SystemClock_Config(wav_play.wavhead.FMT.SampleRate);
	HAL_SAI_Transmit_DMA_myuse(&hsai_BlockA1);
	__HAL_DMA_DISABLE(&hdma_sai1_a);
	HAL_DMAEx_MultiBufferStart_IT(&hdma_sai1_a,(uint32_t)wav_play.buf0.buf_u8,(uint32_t)&hsai_BlockA1.Instance->DR,(uint32_t)wav_play.buf1.buf_u8,wav_play.wavhead.FMT.SampleRate / 20);//调整传输buf长度来匹配进度条更新速率
	HAL_SAI_DMAPause(&hsai_BlockA1);
}

extern QueueHandle_t change_music_handle;
extern char music_play_list[256][FILE_NAME_LEN_MAX];
void ChangeMusic(void *pvParameters)
{
	while(1)
	{
		xSemaphoreTake(change_music_handle,portMAX_DELAY);
		USART_printf(&hlpuart1,"music_play_now:%u\r\n",wav_play.music_play_now);
		Init_wav_play(&wav_play,music_play_list[wav_play.music_play_now]);
		wav_play.play_sta = 1;
		HAL_SAI_DMAResume(&hsai_BlockA1);
	}
}

uint8_t scan_files(char* path,char buff[256][FILE_NAME_LEN_MAX],uint32_t *music_total_save)
{
	uint8_t fr;
	uint32_t music_total=0;
	static FILINFO fno; //文件信息结构体
	DIR dir;
	fr = f_opendir(&dir, path);  // 打开文件目录	
	if(fr == FR_OK) 
	{//如果打开成功循环读出文件名字到buff中
		for (;;) 
		{  //循环读出文件名字，循环次数等于SD卡根目录下的文件数目
			fr = f_readdir(&dir, &fno); //读取文件名
			if (fr != FR_OK || fno.fname[0] == 0) break;  //读取错误或者读完所有文件结束就跳出循环
			strcpy(buff[music_total], fno.fname);	 //复制文件名字到缓存并打印文件名
//			USART_printf(&hlpuart1,"%s\r\n",buff[music_total]);	
			music_total++;
		}
		*music_total_save = music_total;
		USART_printf(&hlpuart1,"total = %u\r\n",*music_total_save);
		f_closedir(&dir);//关闭文件目录
	}
	return fr;// 返回
}

void tune_volume(WavPlay *wav_play)
{
	uint32_t i=0;
	float ratio;
	
	if(wav_play->volume>=100)
		return;
	
	ratio = wav_play->volume/100.0f;
	
	if(wav_play->buf_sta == 0)
	{
		for(i=0;i<(wav_play->wavhead.FMT.SampleRate/10/2);i++)
		{
			wav_play->buf0.buf_i16[i] *= ratio;
		}
	}
	else if(wav_play->buf_sta == 1)
	{
		for(i=0;i<(wav_play->wavhead.FMT.SampleRate/10/2);i++)
		{
			wav_play->buf1.buf_i16[i] *= ratio;
		}
	}
	
}

void get_play_list(char ori_list[256][FILE_NAME_LEN_MAX],char play_list[256][FILE_NAME_LEN_MAX])
{
	char temp[FILE_NAME_LEN_MAX]={0};
	uint32_t i,j;
	memcpy(*play_list,*ori_list,256*FILE_NAME_LEN_MAX);
	
	
	if(wav_play.play_mode == 1)//生成随机播放列表
	{
		for (i = wav_play.music_total-1 ; i>0 ; i--) 
		{
			HAL_RNG_GenerateRandomNumber(&hrng,&rand_num);
			// 生成一个随机位置
			j = rand_num % (i + 1);

			// 交换元素
			memcpy(temp,play_list[i],FILE_NAME_LEN_MAX);
			memcpy(play_list[i],play_list[j],FILE_NAME_LEN_MAX);
			memcpy(play_list[j],temp,FILE_NAME_LEN_MAX);
    }
		
		USART_printf(&hlpuart1,"random list:\r\n");
		for(i=0;i<wav_play.music_total;i++)
		{
			USART_printf(&hlpuart1,"%u:%s\r\n",i,play_list[i]);
		}
		
	}
}
