#include "motor_encoder_test.h"
#include "nrf24l01.h"
#include "motor.h"
#include "led.h"
#include "delay.h"
#include "stdlib.h"

extern L01Buf L01RcvBuf;
extern unsigned long  LeftMotorStep;
extern unsigned long  RightMotorStep; 
extern unsigned long DelayTime;
extern unsigned long banlanceL;//左  680
extern unsigned long banlanceR;
extern unsigned char Key1;
extern unsigned char Key0;


//码盘每转一圈有32脉冲，轮子外径为3.5cm，因此每接收到一个脉冲，轮子走过3.14cm*3.5/32=3.4344mm,
//轮子每走过一米，理论上有1000/3.4344=291个脉冲，如果轮子的速度为1m/s(很快了)，则每3ms会接收到一个码盘脉冲
void motor_encoder_test()
{
  //unsigned char count_h=0,count_l=0;
  unsigned char rcvData[32]={0};
  
  unsigned int i=0,j=0;
  LeftMotorStep=RightMotorStep=0;
  forward();
  while(1)
  {
    if(L01_Buf_Read(&L01RcvBuf,rcvData)==1 && rcvData[0]=='t' && rcvData[1]=='s' && rcvData[2]=='4'&& rcvData[3]=='1' )
    {	  
       ledTurnon(LED0);
       delay_ms(300);
       ledTurnoff(LED0);//indicate the end of this test
       stop();
       break;//停止
    }
    if(Key1==1)
    {
      Key1=0;
      if(banlanceL<1000-50)
        banlanceL+=50;
      if(banlanceR<1000-50)
        banlanceR+=50;
      PWMPulseWidthSet(PWM_BASE, PWM_OUT_0,banlanceL);//左
      PWMPulseWidthSet(PWM_BASE, PWM_OUT_1,banlanceR);//右
    }
    if(Key0==1)
    {
       Key0=0;
      if(banlanceL>50)
        banlanceL-=50;
      if(banlanceR>50)
        banlanceR-=50;
      PWMPulseWidthSet(PWM_BASE, PWM_OUT_0,banlanceL);//左
      PWMPulseWidthSet(PWM_BASE, PWM_OUT_1,banlanceR);//右
    }
    
    DelayTime=0;
    while(DelayTime<3);//3mm
    
    if(LeftMotorStep>0)
    {
      LeftMotorStep=0;
      rcvData[i]=rcvData[i] | (0x80>>j);
    }
    else
    {
      rcvData[i]=rcvData[i] & ~(0x80>>j);
    }
    if(RightMotorStep>0)
    {
      RightMotorStep=0;
      rcvData[i+16]=rcvData[i+16] | (0x80>>j);
    }
    else
    {
      rcvData[i+16]=rcvData[i+16] & ~(0x80>>j);
    }
    j++;
    if(j>7)
    {
      j=0;
      i++;
      if(i>15)
      {
        i=0;
        NRF24L01_TxPacket(SSI1_BASE,rcvData);
      }
    }
  }
}