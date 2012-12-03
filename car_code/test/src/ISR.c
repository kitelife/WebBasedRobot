#include "ISR.h"

#include "baselib.h"

#include "delay.h"
#include "nrf24l01.h"
#include "led.h"
#include "motor.h"
#include "keys.h"

unsigned long DelayTime;
unsigned char Key1=0;
unsigned char Key0=0;

extern unsigned char TestNum;
extern unsigned long  LeftMotorStep;
extern unsigned long  RightMotorStep; 


//general function
void initTimer()
{
      //使能定时器0
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0); 
      //配置定时器0为32位周期定时器
	TimerConfigure(TIMER0_BASE, TIMER_CFG_32_BIT_PER);
	//设置定时器0的装载值
	TimerLoadSet(TIMER0_BASE, TIMER_A, SysCtlClockGet()/1000);  
	//设置定时器中断优先级为1
	IntPrioritySet(INT_TIMER0A, 1<<5);
	//设置定时器中断为超时中断
	TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);  
	//使能定时器中断
	IntEnable(INT_TIMER0A);     
	TimerEnable(TIMER0_BASE, TIMER_A); 
    /*    
        //使能定时器1
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1); 
        //配置定时器1为32位周期定时器
	TimerConfigure(TIMER1_BASE, TIMER_CFG_32_BIT_PER);
	//设置定时器1的装载值
	TimerLoadSet(TIMER1_BASE, TIMER_A, SysCtlClockGet()>>8);  
	//设置定时器中断优先级为1
	IntPrioritySet(INT_TIMER1A, 1<<5);
	//设置定时器中断为超时中断
	TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);  
	//使能定时器中断
	IntEnable(INT_TIMER1A);     
	TimerEnable(TIMER1_BASE, TIMER_A); */
}

/*RF receiving(nrf24l01.c) and key&led test(key_led_test.c)*/
void GPIO_Port_A_ISR(void)
{
  unsigned char sta;
  long intstatus = GPIOPinIntStatus(GPIO_PORTA_BASE, 0);
  if(intstatus & IRQ1)
  {
    sta=NRF24L01_Read_Reg(SSI0_BASE,READ_REG|STATUS);
    if(sta & RX_OK)
    {
       NRF24L01_RxPacket(SSI0_BASE);
    }
    NRF24L01_Write_Reg(SSI0_BASE,WRITE_REG|STATUS,intstatus);
  }
  GPIOPinIntClear(GPIO_PORTA_BASE,IRQ1);
}
/*RF receiving(nrf24l01.c) and key&led test(key_led_test.c)*/
void GPIO_Port_B_ISR(void)
{
  static unsigned char ledState=0;
  unsigned char sta;
  long intstatus = GPIOPinIntStatus(GPIO_PORTB_BASE, 0);
  if(intstatus & IRQ2)
  {
    sta=NRF24L01_Read_Reg(SSI1_BASE,READ_REG|STATUS);
    if(sta & RX_OK)
    {
       NRF24L01_RxPacket(SSI1_BASE);
    }
    NRF24L01_Write_Reg(SSI1_BASE,WRITE_REG|STATUS,intstatus);
  }
  else if(intstatus & KEY0)
  {
    Key0=1;
    if(TestNum==1)
    {
      if(ledState/3)//b0xx
      {
        switch(ledState%3)
        {
        case 0:
          ledTurnoff(LED0);
          break;
        case 1:
          ledTurnoff(LED1);
          break;
        case 2:
          ledTurnoff(LED2);
          break;
        }
        ledState-=3;
      }
      else        //b1xx
      {
        switch(ledState%3)
        {
        case 0:
          ledTurnon(LED0);
          break;
        case 1:
          ledTurnon(LED1);
          break;
        case 2:
          ledTurnon(LED2);
          break;
        default:
          break;
        }
        ledState+=3;
      }
    }
  }
  else if(intstatus & KEY1)
  {
    Key1=1;
    if(TestNum==1)
    {
      ledState=(ledState+1)%3;
    }
  }
  GPIOPinIntClear(GPIO_PORTB_BASE,KEY0 | KEY1 | IRQ2);
}


/*charging*/
void GPIO_Port_H_ISR(void)//PH2是充电开关
{
    return;
}

/*Delay counting(general)*/
//定时器0用于调整步伐
void Timer0A_ISR(void)
{
    
    //清除中断源
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
        DelayTime++;
	//右轮转速大于左轮,调节左右两轮转速并清0两轮码盘计数
        //displayLongData(banlance1);
   /*   
	if(LeftMotorStep < RightMotorStep-3)
	{
	  banlance1 = banlance1+delta;
          banlance2 = banlance2-delta;
	  PWMPulseWidthSet(PWM_BASE, PWM_OUT_0, banlance1);//左
	  PWMPulseWidthSet(PWM_BASE, PWM_OUT_1, banlance2);//右
          LeftMotorStep = 0;
          RightMotorStep = 0;
	}
	//右轮转速小于左轮,调节左右两轮转速并清0两轮码盘计数
	else if(LeftMotorStep > RightMotorStep+3)
	{
	  banlance1 = banlance1-delta;
	  banlance2 = banlance2+delta;
	  PWMPulseWidthSet(PWM_BASE, PWM_OUT_1, banlance2);//Right
	  PWMPulseWidthSet(PWM_BASE, PWM_OUT_0, banlance1);//Left
          LeftMotorStep = 0;
          RightMotorStep = 0;
	}
	//右轮转速等于左轮,清0两轮码盘计数
	else
	{
          LeftMotorStep = 0;
          RightMotorStep = 0;
        }
	//使能定时器0中断
	TimerEnable(TIMER0_BASE, TIMER_A);
    */    
}


/*colliding switch*/
void GPIO_Port_C_ISR(void)
{
  //GPIOPinIntClear(GPIO_PORTC_BASE,KEY0 | KEY1);
 /* ledTurnover(LED1);
  backward();
  delay_ms(500);//后退一段距离
  turn();
  delay_ms(300);//转弯一段时间，逆时针
  forward();*/
  GPIOPinIntClear(GPIO_PORTC_BASE,KEY0 | KEY1);
}

/*infrared detection*/
void Timer1A_ISR(void)
{  
 // int i;
  TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
  //TimerEnable(TIMER1_BASE, TIMER_A);
  //return;
/*
  for(i=0;i<3;i++){
    distence = getDistence(i);
    if(distence > 70){
       // ledTurnover(LED2);
        backward();
        delay_ms(500);//后退一段距离
        turn();
        delay_ms(300);//转弯一段时间，顺时针
        forward();
        break;
    }
  }
  */
  //使能定时器0中断
 // TimerEnable(TIMER1_BASE, TIMER_A);
}

/*motor encoder*/

//右轮码盘
void GPIO_Port_D_ISR(void)
{
  GPIOPinIntClear(GPIO_PORTD_BASE,CODE_R);
  RightMotorStep++;//不能依赖码盘计数
}

//左轮码盘
void GPIO_Port_G_ISR(void)
{
  GPIOPinIntClear(GPIO_PORTG_BASE,CODE_L);
  LeftMotorStep++;
}