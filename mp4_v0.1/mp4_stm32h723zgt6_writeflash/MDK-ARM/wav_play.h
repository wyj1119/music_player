#ifndef __WAV_PLAY__
#define __WAV_PLAY__

#include "main.h"
#include "fatfs.h"


#define FILE_NAME_LEN_MAX 128

typedef struct
{
	uint32_t ChunkID;//chunk id�̶�Ϊ"RIFF",��0X46464952
	uint32_t ChunkSize;//���ϴ�С,�ļ��ܴ�С-8
	uint32_t Format;//��ʽ,"WAVE",��0X45564157
}ChunkRIFF;

typedef struct 
{
	uint32_t ChunkID;//chunk id,"fmt "(�и��ո�),0X20746D66
	uint32_t ChunkSize;//chunk size(������ID��Size)//qhc.wav 0x10
	uint16_t AudioFormat;//��Ƶ��ʽ,0X01��ʾ����PCM,0X11��ʾIMA ADPCM//0x01
	uint16_t NumofChannels;//ͨ������,0x01��ʾ������,0x02��ʾ˫����//0x02
	uint32_t SampleRate;//������;����:0X1F40--8KHZ,0X3E80--16KHZ,0X7D00--32KHZ,0XAC44--44.1KHZ,0XBB80--48KHZ//0XAC44
	uint32_t ByteRate;//�ֽ�����=������*ÿ�β�����С//0X2B110 = 0XAC44*0X4
	uint16_t BlockAlign;//ÿ�β�����С//0X4
	uint16_t BitsPerSample;//��������//0x10,16λ
}ChunkFMT;

typedef struct
{
	uint32_t ChunkID;//chunk id,"data",0X61746164
	uint32_t ChunkSize;//chunk size(������ID��Size,����Ƶ����)//0X27E75A0
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
	uint8_t buf_sta;//0--Ӧ�����buf0 1--Ӧ�����buf1 
	uint8_t play_sta;//0--ֹͣ���� 1--������ 2--��ȡ������ڴ������һ֡ 3--�����������
	
	uint8_t volume;//���� ȡֵ��Χ:0--100	
	uint8_t play_mode;//0--˳�򲥷� 1--�������
	uint32_t music_total;//��������
	uint32_t music_play_now;//��ǰ�������ֵ�����
	
	ProgressBar pbar;
	
//Fatfs����
	FIL WAV_FIL;
	UINT br_wav;
	uint8_t file_name[FILE_NAME_LEN_MAX];
	uint32_t file_ptr;//ָ���ļ�������λ��
	
}WavPlay;




uint8_t get_wav_head(WavPlay *wav_play);
void Init_wav_play(WavPlay *wav_play,const char *songname);
void Init_wav_SAI_DMA(void);
void ChangeMusic(void *pvParameters);
uint8_t scan_files(char* path,char buff[256][FILE_NAME_LEN_MAX],uint32_t *music_total_save);
void get_play_list(char ori_list[256][FILE_NAME_LEN_MAX],char play_list[256][FILE_NAME_LEN_MAX]);
void tune_volume(WavPlay *wav_play);

#endif




