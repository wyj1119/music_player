#ifndef __LVGL_TASK_H__
#define __LVGL_TASK_H__

#include "main.h"
#include "wav_play.h"

void lvgl(void *pvParameters);
void my_gui_app(void);
static void btn_pause_cb(lv_event_t * e);
static void btn_next_cb(lv_event_t * e);
static void btn_previous_cb(lv_event_t * e);
static void btn_play_mode_cb(lv_event_t * e);
#endif
