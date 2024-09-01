#ifndef __WAV_PLAY__
#define __WAV_PLAY__

#include "main.h"
#include "fatfs.h"


#define FILE_NAME_LEN_MAX 128

typedef struct
{
	uint32_t ChunkID;//chunk id固定为"RIFF",即0X46464952
	uint32_t ChunkSize;//集合大小,文件总大小-8
	uint32_t Format;//格式,"WAVE",即0X45564157
}ChunkRIFF;

typedef struct 
{
	uint32_t ChunkID;//chunk id,"fmt "(有个空格),0X20746D66
	uint32_t ChunkSize;//chunk size(不包含ID和Size)//qhc.wav 0x10
	uint16_t AudioFormat;//音频格式,0X01表示线性PCM,0X11表示IMA ADPCM//0x01
	uint16_t NumofChannels;//通道数量,0x01表示单声道,0x02表示双声道//0x02
	uint32_t SampleRate;//采样率;常用:0X1F40--8KHZ,0X3E80--16KHZ,0X7D00--32KHZ,0XAC44--44.1KHZ,0XBB80--48KHZ//0XAC44
	uint32_t ByteRate;//字节速率=采样率*每次采样大小//0X2B110 = 0XAC44*0X4
	uint16_t BlockAlign;//每次采样大小//0X4
	uint16_t BitsPerSample;//采样精度//0x10,16位
}ChunkFMT;

typedef struct
{
	uint32_t ChunkID;//chunk id,"data",0X61746164
	uint32_t ChunkSize;//chunk size(不包含ID和Size,纯音频数据)//0X27E75A0
}Chunkdata;

typedef struct
{
	ChunkRIFF RIFF;
	ChunkFMT FMT;
	Chunkdata Chunk_data;
}WavHead;


typedef struct
{
	uint32_t total_time_byte;
	uint32_t now_time_byte;
	
	uint16_t total_time_s;
	uint16_t now_time_s;
}ProgressBar;

typedef union
{
	uint8_t buf_u8[I2S_BUF];
	uint16_t buf_u16[I2S_BUF/2];
	short buf_i16[I2S_BUF/2];
}WavBufUnion;

typedef struct
{
	WavHead wavhead;
	WavBufUnion buf0;
	WavBufUnion buf1;
	uint8_t buf_sta;//0--应该填充buf0 1--应该填充buf1 
	uint8_t play_sta;//0--停止播放 1--播放中 2--读取完毕正在传送最后一帧 3--歌曲播放完毕
	
	uint8_t volume;//音量 取值范围:0--100	
	uint8_t play_mode;//0--顺序播放 1--随机播放
	uint32_t music_total;//歌曲总数
	uint32_t music_play_now;//当前播放音乐的索引
	
	ProgressBar pbar;
	
//Fatfs控制
	FIL WAV_FIL;
	UINT br_wav;
	uint8_t file_name[FILE_NAME_LEN_MAX];
	uint32_t file_ptr;//指向文件读到的位置
	
}WavPlay;




uint8_t get_wav_head(WavPlay *wav_play);
void Init_wav_play(WavPlay *wav_play,const char *songname);
void Init_wav_SAI_DMA(void);
void ChangeMusic(void *pvParameters);
uint8_t scan_files(char* path,char buff[256][FILE_NAME_LEN_MAX],uint32_t *music_total_save);
void get_play_list(char ori_list[256][FILE_NAME_LEN_MAX],char play_list[256][FILE_NAME_LEN_MAX]);
void tune_volume(WavPlay *wav_play);

#endif




