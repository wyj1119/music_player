#include "lvgl_task.h"
#include "usart.h"
#include "sai.h"

extern WavPlay wav_play;
extern QueueHandle_t change_music_handle;
void lvgl(void *pvParameters)
{
	
	while(1)
	{
		
		lv_timer_handler();
		vTaskDelay(5);
	}
}

#define BTN_WIDTH 50
#define BTN_HEIGHT 50
#define INTERVAL 80

LV_IMG_DECLARE(pause);
LV_IMG_DECLARE(next);
LV_IMG_DECLARE(play);
LV_IMG_DECLARE(previous);
LV_IMG_DECLARE(shuffle);
LV_IMG_DECLARE(repeatOne);
LV_IMG_DECLARE(repeat);

LV_FONT_DECLARE(kai_24);

static lv_style_t style_imgbtn,style_imgbtn_pressed;
static lv_style_t style_bgd;

static lv_style_t style_label_songname;
static lv_style_t style_label_song_time;

static lv_style_t style_bar;
static lv_style_t style_bar_indicator;

lv_obj_t *imgbtn_pause;
lv_obj_t *imgbtn_next;
lv_obj_t *imgbtn_previous;
lv_obj_t *imgbtn_play_mode;


lv_obj_t *label_songname;
lv_obj_t *label_now_t;
lv_obj_t *label_total_t;

lv_obj_t *song_pbar;

void my_gui_app(void)
{
    label_songname = lv_label_create(lv_scr_act());
    label_now_t = lv_label_create(lv_scr_act());
    label_total_t = lv_label_create(lv_scr_act());

    song_pbar = lv_bar_create(lv_scr_act());

    imgbtn_pause = lv_imgbtn_create(lv_scr_act());
    imgbtn_next = lv_imgbtn_create(lv_scr_act());
    imgbtn_previous = lv_imgbtn_create(lv_scr_act());
		imgbtn_play_mode = lv_imgbtn_create(lv_scr_act());
//background
    lv_style_init(&style_bgd);
    lv_style_set_bg_grad_dir(&style_bgd,LV_GRAD_DIR_VER);

    lv_style_set_bg_color(&style_bgd,lv_color_hex(0x001b30));//���
    lv_style_set_bg_grad_color(&style_bgd,lv_color_hex(0xffffff));//�յ�

    lv_style_set_bg_main_stop(&style_bgd,0);//���
    lv_style_set_bg_grad_stop(&style_bgd,255);//�յ�
    lv_style_set_bg_img_opa(&style_bgd,LV_OPA_60);

    lv_obj_add_style(lv_scr_act(),&style_bgd,LV_STATE_DEFAULT);

//imgbtn

    lv_imgbtn_set_src(imgbtn_pause,LV_IMGBTN_STATE_RELEASED,NULL,&play,NULL);
    lv_imgbtn_set_src(imgbtn_next,LV_IMGBTN_STATE_RELEASED,NULL,&next,NULL);
    lv_imgbtn_set_src(imgbtn_previous,LV_IMGBTN_STATE_RELEASED,NULL,&previous,NULL);
		lv_imgbtn_set_src(imgbtn_play_mode,LV_IMGBTN_STATE_RELEASED,NULL,&repeat,NULL);
		
    lv_style_init(&style_imgbtn);
    lv_style_set_width(&style_imgbtn,BTN_WIDTH);
    lv_style_set_height(&style_imgbtn,BTN_HEIGHT);
    lv_style_set_img_recolor_opa(&style_imgbtn,255);
    lv_style_set_img_recolor(&style_imgbtn,lv_color_hex(0x7d7d7d));
    lv_obj_add_style(imgbtn_pause,&style_imgbtn,LV_STATE_DEFAULT);
    lv_obj_add_style(imgbtn_next,&style_imgbtn,LV_STATE_DEFAULT);
    lv_obj_add_style(imgbtn_previous,&style_imgbtn,LV_STATE_DEFAULT);
		lv_obj_add_style(imgbtn_play_mode,&style_imgbtn,LV_STATE_DEFAULT);

    lv_style_init(&style_imgbtn_pressed);
    lv_style_set_width(&style_imgbtn_pressed,BTN_WIDTH);
    lv_style_set_height(&style_imgbtn_pressed,BTN_HEIGHT);
    lv_style_set_img_recolor_opa(&style_imgbtn_pressed,255);
    lv_style_set_img_recolor(&style_imgbtn_pressed,lv_color_hex(0x393939));
    lv_obj_add_style(imgbtn_pause,&style_imgbtn_pressed,LV_STATE_PRESSED);
    lv_obj_add_style(imgbtn_next,&style_imgbtn_pressed,LV_STATE_PRESSED);
    lv_obj_add_style(imgbtn_previous,&style_imgbtn_pressed,LV_STATE_PRESSED);
		lv_obj_add_style(imgbtn_play_mode,&style_imgbtn_pressed,LV_STATE_PRESSED);
		
    lv_obj_set_pos(imgbtn_pause,375,390);
    lv_obj_align_to(imgbtn_next,imgbtn_pause,LV_ALIGN_OUT_RIGHT_MID,INTERVAL,0);
    lv_obj_align_to(imgbtn_previous,imgbtn_pause,LV_ALIGN_OUT_LEFT_MID,-INTERVAL,0);
		lv_obj_align_to(imgbtn_play_mode,imgbtn_next,LV_ALIGN_OUT_RIGHT_MID,INTERVAL+20,0);
		
    lv_obj_add_event_cb(imgbtn_pause,btn_pause_cb,LV_EVENT_CLICKED,NULL);
    lv_obj_add_event_cb(imgbtn_next,btn_next_cb,LV_EVENT_CLICKED,NULL);
    lv_obj_add_event_cb(imgbtn_previous,btn_previous_cb,LV_EVENT_CLICKED,NULL);
		lv_obj_add_event_cb(imgbtn_play_mode,btn_play_mode_cb,LV_EVENT_CLICKED,NULL);
		
//label songname
    lv_style_init(&style_label_songname);
    lv_style_set_text_color(&style_label_songname,lv_color_hex(0xffffff));
    lv_style_set_width(&style_label_songname,800);
    lv_style_set_text_align(&style_label_songname,LV_TEXT_ALIGN_CENTER);

    lv_style_set_x(&style_label_songname,0);
    lv_style_set_y(&style_label_songname,30);
    lv_style_set_text_font(&style_label_songname,&kai_24);
    lv_obj_add_style(label_songname,&style_label_songname,LV_STATE_DEFAULT);

    
//bar
    lv_style_init(&style_bar);
    lv_style_set_bg_color(&style_bar,lv_color_hex(0x65a2b5));
    lv_style_set_height(&style_bar,5);
    lv_style_set_width(&style_bar,600);
    lv_style_set_x(&style_bar,100);
    lv_style_set_y(&style_bar,360);
    lv_obj_add_style(song_pbar,&style_bar,LV_STATE_DEFAULT);

    lv_style_init(&style_bar_indicator);
    lv_style_set_bg_color(&style_bar_indicator,lv_color_hex(0xffffff));
    lv_obj_add_style(song_pbar,&style_bar_indicator,LV_PART_INDICATOR);

    lv_bar_set_value(song_pbar,0,LV_ANIM_ON);
    lv_bar_set_range(song_pbar,0,100);

//label song time
    lv_style_init(&style_label_song_time);
    lv_style_set_text_color(&style_label_song_time,lv_color_hex(0xffffff));
    lv_style_set_width(&style_label_song_time,100);
    lv_style_set_text_align(&style_label_song_time,LV_TEXT_ALIGN_CENTER);
    lv_style_set_text_font(&style_label_song_time,&lv_font_montserrat_14);

    lv_obj_add_style(label_now_t,&style_label_song_time,LV_STATE_DEFAULT);
    lv_obj_add_style(label_total_t,&style_label_song_time,LV_STATE_DEFAULT);

    lv_obj_align_to(label_now_t,song_pbar,LV_ALIGN_OUT_LEFT_MID,0,0);
    lv_obj_align_to(label_total_t,song_pbar,LV_ALIGN_OUT_RIGHT_MID,0,0);
    lv_label_set_text(label_now_t, "00:00");
    lv_label_set_text(label_total_t, "59:59");

}


static void btn_pause_cb(lv_event_t * e)
{
	lv_event_code_t code = lv_event_get_code(e);
	if(code == LV_EVENT_CLICKED)
	{
			//pause code
//			USART_printf(&hlpuart1,"btn_pause_cb,play_sta = %u\r\n",wav_play.play_sta);
			if(wav_play.play_sta==1)
			{
				wav_play.play_sta = 0;
				lv_imgbtn_set_src(imgbtn_pause,LV_IMGBTN_STATE_RELEASED,NULL,&play,NULL);
				HAL_SAI_DMAPause(&hsai_BlockA1);
			}
			else if(wav_play.play_sta==0)
			{
				wav_play.play_sta = 1;
				lv_imgbtn_set_src(imgbtn_pause,LV_IMGBTN_STATE_RELEASED,NULL,&pause,NULL);
				HAL_SAI_DMAResume(&hsai_BlockA1);
			}
	}

}

static void btn_next_cb(lv_event_t * e)
{
	BaseType_t res;
	lv_event_code_t code = lv_event_get_code(e);
	if(code == LV_EVENT_CLICKED)
	{
			//next code
//			USART_printf(&hlpuart1,"btn_next_cb");
			if(wav_play.play_mode != 2)
			{
				if(++wav_play.music_play_now>=wav_play.music_total)
					wav_play.music_play_now = 0;
			}
			HAL_SAI_DMAPause(&hsai_BlockA1);
			wav_play.play_sta = 3;
			f_close(&wav_play.WAV_FIL);
			wav_play.buf_sta = 0;
			res = xSemaphoreGive(change_music_handle);
			if(res==pdFALSE)
				USART_printf(&hlpuart1,"btn_next_cb / xSemaphoreGive error\r\n");
	}

}

static void btn_previous_cb(lv_event_t * e)
{
	BaseType_t res;
	lv_event_code_t code = lv_event_get_code(e);
	if(code == LV_EVENT_CLICKED)
	{
			//previous code
//				USART_printf(&hlpuart1,"btn_previous_cb");
			if(wav_play.play_mode != 2)
			{
				if(wav_play.music_play_now<=0)
					wav_play.music_play_now = wav_play.music_total-1;
				else
					wav_play.music_play_now--;
			}

			HAL_SAI_DMAPause(&hsai_BlockA1);			
			wav_play.play_sta = 3;
			f_close(&wav_play.WAV_FIL);
			wav_play.buf_sta = 0;
			res = xSemaphoreGive(change_music_handle);
			if(res==pdFALSE)
				USART_printf(&hlpuart1,"btn_previous_cb / xSemaphoreGive error\r\n");  
	}

}

extern char music_list[256][FILE_NAME_LEN_MAX];//歌曲列表
extern char music_play_list[256][FILE_NAME_LEN_MAX];//歌曲播放列表
static void btn_play_mode_cb(lv_event_t * e)
{
	BaseType_t res;
	lv_event_code_t code = lv_event_get_code(e);
	if(code == LV_EVENT_CLICKED)
	{
		if(++wav_play.play_mode>=3)
			wav_play.play_mode = 0;
		
		switch(wav_play.play_mode)
		{
			case 0:
			{
				lv_imgbtn_set_src(imgbtn_play_mode,LV_IMGBTN_STATE_RELEASED,NULL,&repeat,NULL);break;
				get_play_list(music_list,music_play_list);
			}
			case 1:
			{
				lv_imgbtn_set_src(imgbtn_play_mode,LV_IMGBTN_STATE_RELEASED,NULL,&shuffle,NULL);break;
				get_play_list(music_list,music_play_list);
			}
			case 2:
			{
				lv_imgbtn_set_src(imgbtn_play_mode,LV_IMGBTN_STATE_RELEASED,NULL,&repeatOne,NULL);break;	
			}
		}
	}
}


