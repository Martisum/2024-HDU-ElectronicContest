#ifndef __MOTOR_H__
#define __MOTOR_H__

#include "main.h"


#define PI 3.1415926

/*
1号电机使用TIM8_CH1，使用TIM1的encoder
2号电机使用TIM8_CH2，使用TIM2的encoder
......以此类推
*/

typedef struct {
    int16_t set_targetS;
    int16_t pre_targetS;

    int PS;
    float IS;
    float DS;

    int16_t now_error;
    int16_t sum_error;
    int16_t pre_error;
    int diff_tar;

    int16_t pwm_out;
} PID;

extern float spd_kp,spd_ki,spd_kd;
extern float dis_kp,dis_ki,dis_kd;
extern uint16_t HORIZON_PWM;

void spd_pid_init(void);
void set_loc(uint16_t tar_loc,uint16_t now_loc);
void spd_pid(int16_t spd,int now_spd);
void set_servo_angle(int16_t angle);
#endif /* __MOTOR_H__ */
