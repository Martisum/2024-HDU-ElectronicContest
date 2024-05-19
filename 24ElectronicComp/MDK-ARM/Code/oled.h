#ifndef __OLED_H
#define __OLED_H

#include "main.h"
//====================================================Ӳ�� SPI ����====================================================
//Ӳ��SPI�����Ѿ���CUBE��ʼ�����
//�������еĺ궨�����main.h��ʵ��
/*
#define OLED_SPI_SPEED                  (42 * 1000 * 1000)                      // Ӳ�� SPI ����
#define OLED_SPI                        (SPI_1)                                 // Ӳ�� SPI ��
#define OLED_D0_PIN                     (SPI1_SCK_PA5 )                    // Ӳ�� SPI SCK ����
#define OLED_D1_PIN                     (SPI1_MOSI_PA7)                    // Ӳ�� SPI MOSI ����
#define OLED_RES_PIN                    (A3 )                                   // Һ����λ���Ŷ���
#define OLED_DC_PIN                     (A4 )                                   // Һ������λ���Ŷ���
#define OLED_CS_PIN                     (GND )                                   // CS Ƭѡ����
*/
//====================================================Ӳ�� SPI ����====================================================

#define OLED_BRIGHTNESS                 (0x7f)                                  // ����OLED���� Խ��Խ�� ��Χ0-0XFF
#define OLED_DEFAULT_DISPLAY_DIR        (OLED_CROSSWISE)                        // Ĭ�ϵ���ʾ����
#define OLED_DEFAULT_DISPLAY_FONT       (OLED_6X8_FONT )                        // Ĭ�ϵ�����ģʽ

//#define gpio_low(x)             ((GPIO_TypeDef*)gpio_group[(x>>5)])->BCR   = (uint16)(1 << (x & 0x0F))
extern unsigned char GRAM[1024];   			//缓存显存数组

typedef enum
{
    OLED_CROSSWISE                      = 0,                                    // ����ģʽ
    OLED_CROSSWISE_180                  = 1,                                    // ����ģʽ  ��ת180
}oled_dir_enum;

typedef enum
{
    OLED_6X8_FONT                       = 0,                                    // 6x8      ����
    OLED_8X16_FONT                      = 1,                                    // 8x16     ����
    OLED_16X16_FONT                     = 2,                                    // 16x16    ���� Ŀǰ��֧��
}oled_font_size_enum;

#define OLED_X_MAX                      (128)
#define OLED_Y_MAX                      (64 )

void    oled_clear                      (void);
void    oled_full                       (const uint8_t color);
void    oled_set_dir                    (oled_dir_enum dir);
void    oled_set_font                   (oled_font_size_enum font);
void    oled_draw_point                 (uint16_t x, uint16_t y, const uint8_t color);

void    oled_show_string                (uint16_t x, uint16_t y, const char ch[]);
void    oled_show_int                   (uint16_t x, uint16_t y, const int32_t dat, uint8_t num);
void    oled_show_uint                  (uint16_t x, uint16_t y, const uint32_t dat, uint8_t num);
void    oled_show_float                 (uint16_t x, uint16_t y, const double dat, uint8_t num, uint8_t pointnum);

void    oled_show_binary_image          (uint16_t x, uint16_t y, const uint8_t *image, uint16_t width, uint16_t height, uint16_t dis_width, uint16_t dis_height);
// void    oled_show_gray_image            (uint16_t x, uint16_t y, const uint8_t *image, uint16_t width, uint16_t height, uint16_t dis_width, uint16_t dis_height, uint8_t threshold);

// void    oled_show_wave                  (uint16_t x, uint16_t y, const uint16_t *image, uint16_t width, uint16_t value_max, uint16_t dis_width, uint16_t dis_value_max);
void    oled_show_chinese               (uint16_t x, uint16_t y, uint8_t size, const uint8_t *chinese_buffer, uint8_t number);
void    oled_set_coordinate             (uint8_t x, uint8_t y);
void    oled_write_data                 (const uint8_t data);
void    oled_init                       (void);

void 		OLED_Refresh 										(void);
void 		OLED_GFill 											(void);
void 		OLED_GClear 										(void);
void 		OLED_DrawPoint									(unsigned char x,unsigned char y);
void 		OLED_ClearPoint									(unsigned char x,unsigned char y);

#endif
