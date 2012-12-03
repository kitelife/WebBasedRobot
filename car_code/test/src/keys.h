#ifndef KEYS_H
#define KEYS_H

#include "delay.h"
#include "adc.h"

//按键
#define KEY0  GPIO_PIN_0  //port B
#define KEY1  GPIO_PIN_1  //port B

//碰撞开关
#define SW1  GPIO_PIN_7  //port C
#define SW2  GPIO_PIN_6  //port C

//红外
#define LED_FRONT_LEFT  GPIO_PIN_6  //PORT G
#define ADC_FRONT_LEFT  ADC_CTL_CH1

#define LED_FRONT_MIDDLE  GPIO_PIN_7  //PORT G
#define ADC_FRONT_MIDDLE  ADC_CTL_CH2

#define LED_FRONT_RIGHT GPIO_PIN_7  //PORT A
#define ADC_FRONT_RIGHT  ADC_CTL_CH0

#define LED_BACK_LEFT   GPIO_PIN_5  //PORT G
#define ADC_BACK_LEFT  ADC_CTL_CH7

#define LED_BACK_MIDDLE GPIO_PIN_4  //PORT G
#define ADC_BACK_MIDDLE  ADC_CTL_CH5

#define LED_BACK_RIGHT  GPIO_PIN_7  //PORT F
#define ADC_BACK_RIGHT  ADC_CTL_CH4


//管脚配置情况参见原理图
//#define RF1 

void initKey();
void initSW();
void initInfrared();

//获得对应传感器所测得的距离。
unsigned long getDistence(int num);

#endif
