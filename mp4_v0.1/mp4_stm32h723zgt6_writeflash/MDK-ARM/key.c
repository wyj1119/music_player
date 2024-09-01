#include "key.h"
#include "usart.h"
#include "sai.h"
#include "rng.h"

extern WavPlay wav_play;
extern QueueHandle_t change_music_handle;
void key_scan(void *pvParameters)
{
	static uint8_t key_state = 0;
	BaseType_t res;
	while(1)
	{
		uint8_t key_val = 0;//0--无按键按下 1--USER_KEY 2--Volume- 3--Volume+
		if(HAL_GPIO_ReadPin(USER_KEY_GPIO_Port,USER_KEY_Pin) == RESET)
			key_val = 1;
		else if(HAL_GPIO_ReadPin(volumepop_GPIO_Port,volumepop_Pin) == RESET)
			key_val = 2;
		else if(HAL_GPIO_ReadPin(volumeplus_GPIO_Port,volumeplus_Pin) == RESET)
			key_val = 3;
		
		
		switch(key_state)
		{
			case 0:
			{
				if(key_val!=0)
					key_state = 1;
			}break;
			case 1:
			{
				if(key_val == 0)
					key_state = 0;
				else 
				{
						if(key_val == 1)
						{
							USART_printf(&hlpuart1,"key0,play_sta = %u\r\n",wav_play.play_sta);
								if(wav_play.play_sta==1)
								{
									wav_play.play_sta = 0;
									HAL_SAI_DMAPause(&hsai_BlockA1);							
								}
								else if(wav_play.play_sta==0)
								{
									wav_play.play_sta = 1;
									HAL_SAI_DMAResume(&hsai_BlockA1);
								}
						}
						else if(key_val == 2)
						{
//							USART_printf(&hlpuart1,"key1");
								if(wav_play.music_play_now<=0)
									wav_play.music_play_now = wav_play.music_total-1;
								else
									wav_play.music_play_now--;
		
								HAL_SAI_DMAPause(&hsai_BlockA1);			
								wav_play.play_sta = 3;
								f_close(&wav_play.WAV_FIL);
								wav_play.buf_sta = 0;
								res = xSemaphoreGive(change_music_handle);
								if(res==pdFALSE)
									USART_printf(&hlpuart1,"key 2 / xSemaphoreGive error\r\n");
						}
						else if(key_val == 3)
						{
//							USART_printf(&hlpuart1,"key2");

								if(++wav_play.music_play_now>=wav_play.music_total)
								wav_play.music_play_now = 0;

								HAL_SAI_DMAPause(&hsai_BlockA1);
								wav_play.play_sta = 3;
								f_close(&wav_play.WAV_FIL);
								wav_play.buf_sta = 0;
								res = xSemaphoreGive(change_music_handle);
								if(res==pdFALSE)
									USART_printf(&hlpuart1,"key 3 / xSemaphoreGive error\r\n");
						}
						key_state = 2;
				}
			}break;
			case 2:
			{
				if(key_val==0)
					key_state = 0;
			}break;
		}
		

		vTaskDelay(10);
		
		
	}
	
}
