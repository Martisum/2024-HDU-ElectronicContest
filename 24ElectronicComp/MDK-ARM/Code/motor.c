#include "main.h"
#include "motor.h"
#include "usart.h"
#include "string.h"
#include "tim.h"
#include "oled.h"
#include "math.h"

float spd_kp=1.5,spd_ki=0,spd_kd=0;
float dis_kp=-0.3,dis_ki=-0.03,dis_kd=0;

int16_t X_now = 0;
int16_t X_last = 0;
int16_t Y_now=0;
int16_t Y_last=0;
int16_t x_speed=0;

//control_state is 0:stop, 1:speed PID only, 2:speed and location PID
uint8_t control_state=0;
uint8_t oscillation_tar=0;

PID speed,location;

//actual pwm=47 is highest 
//actual pwm=82 is horizon
//actual pwm=107 is lowest    
//105 -3 degree 
//47  +2.6 deg      99 -2.6 deg
#define UP_DEG_PWM 99
#define DOWN_DEG_PWM 47
#define HORIZON_DEG_PWM 80

uint16_t HORIZON_PWM=80;

void spd_pid_init(){
    speed.PS=spd_kp;
    speed.IS=spd_ki;
    speed.DS=spd_kd;

    location.PS=dis_kp;
    location.IS=dis_ki;
    location.DS=dis_kd;
}

uint8_t abs_distance(int16_t a,int16_t b){
    if(a-b>=0) return a-b;
    else return b-a;
}

void set_loc(uint16_t tar_loc,uint16_t now_loc){
    location.set_targetS=tar_loc;
    location.now_error=location.set_targetS-now_loc;

    location.sum_error+=location.now_error;
    if(location.sum_error>200) location.sum_error=200;
    else if(location.sum_error<-200) location.sum_error=-200;

    location.pwm_out=location.PS*location.now_error+location.IS*location.sum_error+location.DS*(location.now_error-location.pre_error);
    if(location.pwm_out>200) location.pwm_out=200;
    else if(location.pwm_out<-200) location.pwm_out=-200;
    
    location.pre_error=location.now_error;
}

//caculate the pwm_out only! Need to set pwm actually by set_servo_angle()
void spd_pid(int16_t spd,int now_spd){
    // if(now_spd >=5 || now_spd <=-5)  speed.PS=spd_kp;
    // else speed.PS=spd_kp*0.5;
    speed.set_targetS=spd;
    speed.now_error= now_spd - speed.set_targetS;

    speed.sum_error+=speed.now_error;
    if(speed.sum_error>100) speed.sum_error=100;
    else if(speed.sum_error<-100) speed.sum_error=-100;

    //˵��Ŀ��ֵ�Ǵ�ģ�pwmҪ���Ÿ�
    speed.pwm_out=-1*(speed.PS*speed.now_error+speed.DS*(speed.now_error-speed.pre_error));
    if(speed.pwm_out>1000) speed.pwm_out=1000;
    else if(speed.pwm_out< -1000) speed.pwm_out=-1000;

    speed.pre_error=speed.now_error;
}

void set_servo_angle(int16_t angle){
    if(angle>=0){
        if(angle+HORIZON_PWM>UP_DEG_PWM){
            __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3,UP_DEG_PWM);
        }else{
            __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, angle+HORIZON_PWM);
        }
        
    }else{
        if(angle*2+HORIZON_DEG_PWM<DOWN_DEG_PWM){
            __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, DOWN_DEG_PWM);
        }else{
            // __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, angle*(float)((float)((float)HORIZON_DEG_PWM-(float)DOWN_DEG_PWM)/(float)((float)UP_DEG_PWM-(float)HORIZON_DEG_PWM))+HORIZON_PWM);
            __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, angle*2+HORIZON_PWM);
        }
    }
}
