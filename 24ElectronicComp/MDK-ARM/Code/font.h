#ifndef __FONT_H
#define __FONT_H

//-------常用颜色----------
typedef enum
{
    RGB565_WHITE    = (0xFFFF),                                                     // 白色
    RGB565_BLACK    = (0x0000),                                                     // 黑色
    RGB565_BLUE     = (0x001F),                                                     // 蓝色
    RGB565_PURPLE   = (0xF81F),                                                     // 紫色
    RGB565_PINK     = (0xFE19),                                                     // 粉色
    RGB565_RED      = (0xF800),                                                     // 红色
    RGB565_MAGENTA  = (0xF81F),                                                     // 品红
    RGB565_GREEN    = (0x07E0),                                                     // 绿色
    RGB565_CYAN     = (0x07FF),                                                     // 青色
    RGB565_YELLOW   = (0xFFE0),                                                     // 黄色
    RGB565_BROWN    = (0xBC40),                                                     // 棕色
    RGB565_GRAY     = (0x8430),                                                     // 灰色

    RGB565_39C5BB   = (0x3616),
    RGB565_66CCFF   = (0x665F),
}rgb565_color_enum;

extern const uint8_t      ascii_font_8x16[][16];
extern const uint8_t      ascii_font_6x8[][6];
extern const uint8_t      chinese_test[8][16];
extern const uint8_t      oled_16x16_chinese[][16];
extern const uint8_t      gImage_seekfree_logo[38400];

extern const uint8_t gImage_game2[294]; //游戏手柄
extern const uint8_t sbdxq[][16];

#endif
