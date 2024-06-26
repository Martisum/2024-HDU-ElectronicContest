#ifndef __MOTOR_H__
#define __MOTOR_H__

#include "main.h"

#define PI 3.1415926

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

//actual pwm=47 is highest 
//actual pwm=82 is horizon
//actual pwm=107 is lowest    
//105 -3 degree 
//47  +2.6 deg      99 -2.6 deg
#define UP_DEG_PWM 99
#define DOWN_DEG_PWM 47
#define HORIZON_DEG_PWM 80

extern PID speed,location;
extern float spd_kp,spd_ki,spd_kd;
extern float dis_kp,dis_ki,dis_kd;
extern uint16_t HORIZON_PWM;
extern uint8_t control_state;
extern int16_t X_now;
extern int16_t X_last;
extern int16_t Y_now;
extern int16_t Y_last;
extern int16_t x_speed;
extern uint16_t oscillation_tar;
extern float current;
extern float voltage;
extern int16_t location_target;

uint8_t abs_distance(int16_t a,int16_t b);
void spd_pid_init(void);
void set_loc(uint16_t tar_loc,uint16_t now_loc);
void spd_pid(int16_t spd,int now_spd);
void set_servo_angle(int16_t angle);
#endif /* __MOTOR_H__ */
