/*
*---------------------------------------------------------------
*                        Lvgl Font Tool                         
*                                                               
* 注:使用unicode编码                                              
* 注:本字体文件由Lvgl Font Tool V0.4 生成                          
* 作者:阿里(qq:617622104)                                         
*---------------------------------------------------------------
*/


#include "lvgl.h"
#include "norflash.h"

typedef struct{
    uint16_t min;
    uint16_t max;
    uint8_t  bpp;
    uint8_t  reserved[3];
}x_header_t;
typedef struct{
    uint32_t pos;
}x_table_t;
typedef struct{
    uint8_t adv_w;
    uint8_t box_w;
    uint8_t box_h;
    int8_t  ofs_x;
    int8_t  ofs_y;
    uint8_t r;
}glyph_dsc_t;


static const uint8_t glyph_dsc_xbf[3368391] __attribute__((at(font_kai_24_ADDR)));

static const uint8_t * __user_font_get_bitmap(const lv_font_t * font, uint32_t unicode_letter) {
    x_header_t *p_hd = (x_header_t*)&glyph_dsc_xbf[0];
    if( unicode_letter>p_hd->max || unicode_letter<p_hd->min ) {
        return NULL;
    }
    uint32_t unicode_offset = unicode_letter-p_hd->min;
    uint32_t *p_pos = (uint32_t *)&glyph_dsc_xbf[ sizeof(x_header_t) ];
    if( p_pos[unicode_offset] != 0 ) {
        return &glyph_dsc_xbf[ p_pos[unicode_offset]+sizeof(glyph_dsc_t) ];
    }
    return NULL;
}


static bool __user_font_get_glyph_dsc(const lv_font_t * font, lv_font_glyph_dsc_t * dsc_out, uint32_t unicode_letter, uint32_t unicode_letter_next) {
    x_header_t *p_hd = (x_header_t*)&glyph_dsc_xbf[0];
    if( unicode_letter>p_hd->max || unicode_letter<p_hd->min ) {
        return false;
    }
    uint32_t unicode_offset = unicode_letter-p_hd->min;
    uint32_t *p_pos = (uint32_t *)&glyph_dsc_xbf[ sizeof(x_header_t) ];
    if( p_pos[unicode_offset] != 0 ) {
        glyph_dsc_t * gdsc = (glyph_dsc_t*)&glyph_dsc_xbf[ p_pos[unicode_offset] ];
        dsc_out->adv_w = gdsc->adv_w;
        dsc_out->box_h = gdsc->box_h;
        dsc_out->box_w = gdsc->box_w;
        dsc_out->ofs_x = gdsc->ofs_x;
        dsc_out->ofs_y = gdsc->ofs_y;
        dsc_out->bpp   = p_hd->bpp;
        return true;
    }
    return false;
}


//KaiTi,,-1
//字模高度：27
//XBF字体,内部大数组
lv_font_t kai_24 = {
    .get_glyph_bitmap = __user_font_get_bitmap,
    .get_glyph_dsc = __user_font_get_glyph_dsc,
    .line_height = 27,
    .base_line = 0,
};

