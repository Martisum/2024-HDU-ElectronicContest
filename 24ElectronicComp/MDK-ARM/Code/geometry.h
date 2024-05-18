/*
 * geometry.h
 *
 *  Created on: 2022��6��29��
 *      Author: 22575
 */

#ifndef GEOMETRY_H_
#define GEOMETRY_H_

#include "main.h"

#define POINTS 64                             //�������ݵ���
#define MIN_VALUE 100                          //����������Сֵ
#define MAX_VALUE 4000                         //�����������ֵ 
#define SCALE ((MAX_VALUE - MIN_VALUE) / 2.0)  
#define OFFSET MIN_VALUE  
#define M_PI  3.14159265

extern uint16_t sine_wave_u16[POINTS];
extern int sine_wave[POINTS]; 
extern uint8_t global_wave_type;

void sin_basedata(void);
uint16_t my_abs(uint16_t m, uint16_t n);
int8_t my_limit(int8_t num);
//��ʾһ����
void show_point(uint8_t x,uint8_t y);
//ɾ��һ����
void del_point(uint8_t x,uint8_t y);
void show_line(uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2);
void del_rectangle(uint8_t x,uint8_t y,uint8_t height,uint8_t wide);
uint8_t speed_data_convert(int16_t temp);
void wave_point_init(void);
void wave_point_add(uint8_t num);
void refresh_wave(uint8_t startline, uint8_t endline);

#endif /* GEOMETRY_H_ */
