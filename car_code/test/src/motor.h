#ifndef MOTOR_H
#define MOTOR_H


//包含头文件
#include "Pwm.h"


//管脚配置情况参见原理图
#define PWM_L     GPIO_PIN_2//PG端口     左轮PWM控制
#define MOTOR_L0  GPIO_PIN_0//PG端口     左轮控制输入1
#define MOTOR_L1  GPIO_PIN_1//PG端口     左轮控制输入2
//#define SEN_L     ADC_CTL_CH6   //SEN_L没有接。。。坑爹啊
#define CODE_L    GPIO_PIN_3//PG端口     左轮码盘

#define PWM_R     GPIO_PIN_1//PD端口     右轮控制使能
#define MOTOR_R0  GPIO_PIN_3//PD端口     右轮控制输入1
#define MOTOR_R1  GPIO_PIN_2//PD端口     右轮控制输入2
#define SEN_R     ADC_CTL_CH3
#define CODE_R    GPIO_PIN_0//PD端口     右轮码盘


//GPIO初始化
void initMotor();

//PWM初始化
void initPWM();

void leftspeedSet(char direction,unsigned long lspeed);//-100~100
void rightspeedSet(char direction,unsigned long rspeed);

//前进
void forward();

//后退
void backward();
//停止
void stop();
//转弯,逆时针
void turn();
void stepAdjust();
#endif

