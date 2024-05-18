#include "main.h"
#include "motor.h"
#include "usart.h"
#include "string.h"
#include "tim.h"
#include "oled.h"
#include "math.h"

float spd_kp=1,spd_ki=0,spd_kd=2;
float dis_kp=1,dis_ki=0,dis_kd=2;

PID speed,location;

//actual pwm=47 is highest 
//actual pwm=82 is horizon
//actual pwm=107 is lowest    
//105 -3 degree 
//47  +2.6 deg      99 -2.6 deg

uint16_t HORIZON_PWM=70;

void spd_pid_init(){
    speed.PS=spd_kp;
    speed.IS=spd_ki;
    speed.DS=spd_kd;

    location.PS=dis_kp;
    location.IS=dis_ki;
    location.DS=dis_kd;
}

void set_loc(uint16_t tar_loc,uint16_t now_loc){
    location.set_targetS=tar_loc;
    location.now_error=location.set_targetS-now_loc;

    location.pwm_out=location.PS*location.now_error+location.DS*(location.now_error-location.pre_error);
    if(location.pwm_out>1000) location.pwm_out=1000;
    else if(location.pwm_out<0) location.pwm_out=0;
    
    location.pre_error=location.now_error;
}

//caculate the pwm_out only! Need to set pwm actually by set_servo_angle()
void spd_pid(int16_t spd,int now_spd){
    speed.set_targetS=spd;
    speed.now_error=speed.set_targetS-now_spd;

    speed.sum_error+=speed.now_error;
    if(speed.sum_error>100) speed.sum_error=100;
    else if(speed.sum_error<-100) speed.sum_error=-100;

    //说明目标值是大的，pwm要正着给
    speed.pwm_out=speed.PS*speed.now_error+speed.DS*(speed.now_error-speed.pre_error);
    if(speed.pwm_out>1000) speed.pwm_out=1000;
    else if(speed.pwm_out<0) speed.pwm_out=0;

    speed.pre_error=speed.now_error;
}

void set_servo_angle(int16_t angle){
    
}
