/****************************************************************
 * file:	main.c
 * date:	2012-08-11
 * author:      Xiaosong Chen
 * description: Main file of the project
 ***************************************************************/
#include "baselib.h"

#include "delay.h"
#include "nrf24l01.h"
#include "led.h"
#include "motor.h"
#include "keys.h"
#include "ISR.h"

#include "key_led_test.h"
#include "wireless_comm_test.h"
#include "walk_test.h"
#include "motor_encoder_test.h"
/*********************************************************************************************************
  全局变量定义
*********************************************************************************************************/

extern unsigned long  LeftMotorStep ;
extern unsigned long  RightMotorStep ; 
extern L01Buf L01RcvBuf;

unsigned long distence = 0;
unsigned char TestNum=0;
unsigned char receive[32];

unsigned long banlance_l = 560;//左
unsigned long banlance_r = 500;//右

// 条件变量定义
unsigned char car_num = '1';
unsigned char info_cmd = '1';
unsigned char run_cmd = '2';
unsigned char forward_cmd = '3';
unsigned char backward_cmd = '4';
unsigned char turnleft_cmd = '5';
unsigned char turnright_cmd = '6';
unsigned char stop_cmd = '7';

void sysInit()
{
    //配置系统时钟
     
    SysCtlClockSet(SYSCTL_SYSDIV_1  | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_8MHZ); 
    IntMasterEnable();
}

unsigned char Scan(void)
{
  unsigned char rcvData[32];
  if(L01_Buf_Read(&L01RcvBuf,rcvData)==1)
  {
      int index;
      for(index=0; index < 32; index++)
      {
        receive[index] = rcvData[index];
      }
      return 1;
  }
  return 0;
}


void delay(unsigned long int d){        
  for(; d; --d);  
}

// 存储电量的结构体
struct voltage{
  unsigned char a[2];
}x;

// 功能： 检测电量
struct voltage detectVoltage();

// 模数转化初始化
void ADCInit(void);

int main(void) 
{   
  // unsigned int i,j;///
   //unsigned char rcvData[32];
    sysInit();
    delay_ms(50);
    initLed();
    initMotor();
    initPWM();
    initKey();
    initSW();
   // initInfrared();
    ADCInit(); 
    initNrf24l01();
    initTimer();
    ledTurnon(LED0 | LED1 | LED2);
    TX_Mode(SSI1_BASE);
    RX_Mode(SSI0_BASE);

    while(1)
    {
      if(Scan()== 1)
      {
        if((car_num == receive[0]) || ('x' == receive[0]))
        {
          
          unsigned char response[32] = {0};
          int index;
          for(index=0; index < 32; index++)
            response[index] = '0';
          if('0' == receive[1]){
          if(info_cmd == receive[2])
          {
            /*
            response[0] = 'd';
            response[1] = car_num;
            response[2] = '0';
            response[3] = info_cmd;
            */
            // 32位char字符串返回值，0位为小车编号
            response[0] = car_num;
            struct voltage y;
            y = detectVoltage();
            // 1, 2位为电量的值
            response[1] = y.a[0];
            response[2] = y.a[1];
            
            // 3,7位为+或-，表示向前或向后(正速度，负速度)
            // 4,5,6位为左马达的速度
            // 8,9,10位为右马达的速度
            long leftMotor_L0;
            long leftMotor_L1;
            long rightMotor_L0;
            long rightMotor_L1;
            leftMotor_L0 = GPIOPinRead(GPIO_PORTG_BASE, MOTOR_L0); 
            leftMotor_L1 = GPIOPinRead(GPIO_PORTG_BASE, MOTOR_L1); 
            rightMotor_L0 = GPIOPinRead(GPIO_PORTD_BASE, MOTOR_L0); 
            rightMotor_L1 = GPIOPinRead(GPIO_PORTD_BASE, MOTOR_L1); 
            
            if (leftMotor_L0 !=0 && leftMotor_L1 == 0 && rightMotor_L0 != 0 && rightMotor_L1 == 0)//前进
            {                                            
              response[3] = '+';//left motor speed
              response[4] = 48+banlance_l/100;//left motor speed
              response[5] = 48+(banlance_l%100)/10;//left motor speed
              response[6] = 48+banlance_l%10;//left motor speed                             
              response[7] = '+';//right motor speed
              response[8] = 48+banlance_r/100;//right motor speed
              response[9] = 48+(banlance_r%100)/10;//right motor speed
              response[10] = 48+banlance_r%10;//right motor speed	
            }
            else if (leftMotor_L0 ==0 && leftMotor_L1 != 0 && rightMotor_L0 == 0 && rightMotor_L1 != 0)//后退
            {                                       
              response[3] = '-';//left motor speed
              response[4] = 48+banlance_l/100;//left motor speed
              response[5] = 48+(banlance_l%100)/10;//left motor speed
              response[6] = 48+banlance_l%10;//left motor speed                             
              response[7] = '-';//right motor speed
              response[8] = 48+banlance_r/100;//right motor speed
              response[9] = 48+(banlance_r%100)/10;//right motor speed
              response[10] = 48+banlance_r%10;//right motor speed	
            }	
            else if (leftMotor_L0 !=0 && leftMotor_L1 != 0 && rightMotor_L0 != 0 && rightMotor_L1 != 0)//停止
            {                                       
              response[3] = '+';//left motor speed
              response[4] = '0';//left motor speed
              response[5] = '0';//left motor speed
              response[6] = '0';//left motor speed                             
              response[7] = '+';//right motor speed
              response[8] = '0';//right motor speed
              response[9] = '0';//right motor speed
              response[10] = '0';//right motor speed	
            }
            NRF24L01_TxPacket(SSI1_BASE,response);
            delay_ms(8);
          }
          else if(run_cmd == receive[2])
          {
            forward();
          }
          else if(forward_cmd == receive[2])
          {
            forward();
          }
          else if(backward_cmd == receive[2])
          {
            backward();
          }
          else if(turnleft_cmd == receive[2])
          {
            turnLeft();
            delay(150000);
            forward();
          }
          else if(turnright_cmd == receive[2])
          {
            turnRight();
            delay(150000);
            forward();
          
          }
          else if(stop_cmd == receive[2])
          {
            stop();
          }
          }
        }
      }
    }
}


struct voltage detectVoltage()
{
  unsigned long ulVoltage;
  
  ADCProcessorTrigger(ADC_BASE, 0);                                   /*  处理器触发一次A/D转换       *///AD转换即模数转换
  //void ADCProcessorTrigger(unsigned long ulBase, unsigned long ulSequenceNum) 引发一次采样序列的处理器触发
  //ulBase是ADC模块的基址，ulSequenceNum是采样序列的编号
  while (!ADCIntStatus(ADC_BASE, 0, false));                          /*  等待转换结束                */
  //注意此while
  //unsigned long ADCIntStatus(unsigned long ulBase, unsigned long ulSequenceNum, tBoolean bMasked) 获取当前的中断状态
  //ulBase是ADC模块的基址，ulSequenceNum是采样序列的编号，bMasked：需要原始的中断状态则False，需要屏蔽的中断状态则True
  ADCIntClear(ADC_BASE, 0);                                           /*  清除中断标准位              */
  ADCSequenceDataGet(ADC_BASE, 0, &ulVoltage);                        /*  读取转换结果                *///第三个参数为数据存放的地址
  
  ulVoltage = ulVoltage * 3000 / 1023;                                /*  计算实际检测到的电压值(mV)  *///估计一些电压的算法什么的。可以做具体的研究
  ulVoltage = ulVoltage * 3 + 350;                                    /*  计算电池电压值(mV)          */
  
  x.a[0] = '0'+(ulVoltage % 10000) / 1000;
  x.a[1] = '0'+(ulVoltage % 1000 ) / 100;
  return x;    
}

void ADCInit (void)
{
  SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC);                          /*  使能ADC模块                 *///使能一个外设
  SysCtlADCSpeedSet(SYSCTL_ADCSPEED_125KSPS);                         /*  125KSps采样率               *///设置ADC采样数率，Ksps为采样数率的单位为每秒采样千次
  
  ADCSequenceConfigure(ADC_BASE, 0, ADC_TRIGGER_PROCESSOR, 0);        /*  序列0为处理器触发，优先级为0*///配置采样序列的触发源和优先级
  ADCSequenceStepConfigure(ADC_BASE, 0, 0,
                           ADC_CTL_CH0 |//输入通道 
                             ADC_CTL_IE  | //产生一个中断
                               ADC_CTL_END);   //序列的末尾                           /*  配置采样序列发生器的步进    */
  
  ADCHardwareOversampleConfigure(ADC_BASE, 16);                       /*  设置ADC采样平均控制寄存器   */
  //adc.h中有，库pdf里面没有，有对应的Software的
  ADCSequenceEnable(ADC_BASE, 0);                                     /*  使能采样序列0               *///使能一个采样序列
}

