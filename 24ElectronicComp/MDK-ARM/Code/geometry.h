/*
 * geometry.h
 *
 *  Created on: 2022年6月29日
 *      Author: 22575
 */

#ifndef GEOMETRY_H_
#define GEOMETRY_H_

#include "main.h" 

#define POINTS 64                             //正弦数据点数
#define MIN_VALUE 100                          //正弦数据最小值
#define MAX_VALUE 4000                         //正弦数据最大值 
#define SCALE ((MAX_VALUE - MIN_VALUE) / 2.0)  
#define OFFSET MIN_VALUE  
#define M_PI  3.14159265

#define OLED_MIN_VALUE 0
#define OLED_MAX_VALUE 63
#define OLED_SCALE ((OLED_MAX_VALUE - OLED_MIN_VALUE) / 4.0)
#define OLED_OFFSET OLED_MIN_VALUE 

extern uint16_t sine_wave_u16[POINTS];
extern int sine_wave[POINTS]; 
extern uint8_t sine_oled_wave[POINTS];
extern uint16_t square_wave_u16[POINTS];
extern int square_wave[POINTS]; 
extern uint8_t square_oled_wave[POINTS];
extern uint8_t global_wave_type;
extern uint8_t global_freq;
void sin_basedata(void);
void square_basedata(void);
uint16_t my_abs(uint16_t m, uint16_t n);
int8_t my_limit(int8_t num);
void show_point(uint8_t x,uint8_t y); //显示一个点
void del_point(uint8_t x,uint8_t y); //删除一个点
void show_line(uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2);
void del_rectangle(uint8_t x,uint8_t y,uint8_t height,uint8_t wide);
uint8_t speed_data_convert(int16_t temp);
void wave_point_init(void);
void wave_point_add(uint8_t x, uint8_t num);
void refresh_wave(uint8_t startline, uint8_t endline);
void draw_sin(void);

#endif /* GEOMETRY_H_ */
