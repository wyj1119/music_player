#ifdef __has_include
    #if __has_include("lvgl.h")
        #ifndef LV_LVGL_H_INCLUDE_SIMPLE
            #define LV_LVGL_H_INCLUDE_SIMPLE
        #endif
    #endif
#endif

#if defined(LV_LVGL_H_INCLUDE_SIMPLE)
    #include "lvgl.h"
#else
    #include "lvgl/lvgl.h"
#endif


#ifndef LV_ATTRIBUTE_MEM_ALIGN
#define LV_ATTRIBUTE_MEM_ALIGN
#endif

#ifndef LV_ATTRIBUTE_IMG_REPEAT
#define LV_ATTRIBUTE_IMG_REPEAT
#endif
#include "norflash.h"
const LV_ATTRIBUTE_MEM_ALIGN LV_ATTRIBUTE_LARGE_CONST LV_ATTRIBUTE_IMG_REPEAT uint8_t repeat_map[10000] __attribute__((at(icon_repeat_ADDR)));

const lv_img_dsc_t repeat = {
  .header.cf = LV_IMG_CF_TRUE_COLOR_ALPHA,
  .header.always_zero = 0,
  .header.reserved = 0,
  .header.w = 50,
  .header.h = 50,
  .data_size = 2500 * LV_IMG_PX_SIZE_ALPHA_BYTE,
  .data = repeat_map,
};
