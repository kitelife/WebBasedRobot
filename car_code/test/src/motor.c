#include "motor.h"
#include "led.h"
unsigned long    LeftMotorStep = 0;                /*  左电机前进距离                  */
unsigned long    RightMotorStep = 0;                /*  右电机前进距离                  */
unsigned long banlanceL = 700;//左  680
unsigned long banlanceR = 700;//右  520
unsigned long delta =20;
//GPIO初始化
void initMotor()
{
    //初始化外设端口
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOG);
    
    //电机正反转控制端口配置
    GPIODirModeSet(GPIO_PORTD_BASE,  MOTOR_R0 | MOTOR_R1, GPIO_DIR_MODE_OUT);
    GPIOPadConfigSet(GPIO_PORTD_BASE, MOTOR_R0 | MOTOR_R1, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD);
    
    GPIODirModeSet(GPIO_PORTG_BASE,  MOTOR_L0 | MOTOR_L1, GPIO_DIR_MODE_OUT);
    GPIOPadConfigSet(GPIO_PORTG_BASE, MOTOR_L0 | MOTOR_L1, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD);

    //编码器中断设置，下降沿触发    
    GPIODirModeSet(GPIO_PORTG_BASE, CODE_L, GPIO_DIR_MODE_IN); 
    GPIOPadConfigSet(GPIO_PORTG_BASE, CODE_L, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD);
    GPIOIntTypeSet(GPIO_PORTG_BASE, CODE_L, GPIO_FALLING_EDGE);
    GPIOPinIntEnable(GPIO_PORTG_BASE, CODE_L);
    
    GPIODirModeSet(GPIO_PORTD_BASE, CODE_R, GPIO_DIR_MODE_IN); 
    GPIOPadConfigSet(GPIO_PORTD_BASE, CODE_R, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD);
    GPIOIntTypeSet(GPIO_PORTD_BASE, CODE_R, GPIO_FALLING_EDGE);
    GPIOPinIntEnable(GPIO_PORTD_BASE, CODE_R);
    
    /*
    HWREG(GPIO_PORTB_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY_DD;
    // Set the commit register for PB7 to allow changing the function
    HWREG(GPIO_PORTB_BASE + GPIO_O_CR) = 0x80;
    // Enable the alternate function for PB7 (NMI)
    HWREG(GPIO_PORTB_BASE + GPIO_O_AFSEL) |= 0x80;
    // Turn on the digital enable for PB7
    HWREG(GPIO_PORTB_BASE + GPIO_O_DEN) |= 0x80;
    
    
    GPIODirModeSet(GPIO_PORTB_BASE,GPIO_PIN_7,GPIO_DIR_MODE_IN); 
    GPIOPadConfigSet(GPIO_PORTB_BASE,GPIO_PIN_7,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD);
    GPIOIntTypeSet( GPIO_PORTB_BASE,GPIO_PIN_7,GPIO_FALLING_EDGE ); 
    // Relock the commit register
    HWREG(GPIO_PORTB_BASE + GPIO_O_LOCK) = 0;
    */
    
    IntEnable(INT_GPIOD);
    IntEnable(INT_GPIOG);
}

//PWM初始化
void initPWM()
{
    //初始化外设端口
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOG);
    
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM);  
    
    //设置PWM信号时钟
    SysCtlPWMClockSet(SYSCTL_PWMDIV_1);
    
    //设置PG2和PD1为PWM输出
    GPIOPinConfigure(GPIO_PG2_PWM0);
    GPIOPinConfigure(GPIO_PD1_PWM1);
    GPIOPinTypePWM(GPIO_PORTG_BASE, PWM_L);
    GPIOPinTypePWM(GPIO_PORTD_BASE, PWM_R);
    
    //配置PWM发生模块1
    PWMGenConfigure(PWM_BASE, PWM_GEN_0, PWM_GEN_MODE_DOWN|PWM_GEN_MODE_NO_SYNC);
    //PWMGenConfigure(PWM_BASE, PWM_GEN_3, PWM_GEN_MODE_DOWN|PWM_GEN_MODE_NO_SYNC);

    PWMGenPeriodSet(PWM_BASE, PWM_GEN_0, 1000);
    //PWMGenPeriodSet(PWM_BASE, PWM_GEN_3, 800);
    
   // PWMPulseWidthSet(PWM_BASE, PWM_OUT_0,700);//左
   // PWMPulseWidthSet(PWM_BASE, PWM_OUT_1,500);//右
    PWMPulseWidthSet(PWM_BASE, PWM_OUT_0,banlanceL);//左
    PWMPulseWidthSet(PWM_BASE, PWM_OUT_1,banlanceR);//右
    //使能PWM2输出
    PWMOutputState(PWM_BASE,PWM_OUT_0_BIT,true);
    //使能PWM3输出
    PWMOutputState(PWM_BASE,PWM_OUT_1_BIT,true);
    
    PWMGenEnable(PWM_BASE, PWM_GEN_0);
}


void leftspeedSet(char direction, unsigned long lspeed)
{
  if(direction==0)//back
  {
    GPIOPinWrite(GPIO_PORTG_BASE, MOTOR_L0, 0x00);     
    GPIOPinWrite(GPIO_PORTG_BASE, MOTOR_L1, 0xff);
  }
  else
  {
    GPIOPinWrite(GPIO_PORTG_BASE, MOTOR_L0, 0xff);     
    GPIOPinWrite(GPIO_PORTG_BASE, MOTOR_L1, 0x00);
  }
  PWMPulseWidthSet(PWM_BASE, PWM_OUT_0, lspeed);//Left
}


void rightspeedSet(char direction,unsigned long rspeed)
{
  if(direction==0)
  {
    GPIOPinWrite(GPIO_PORTD_BASE, MOTOR_R0, 0x00);
    GPIOPinWrite(GPIO_PORTD_BASE, MOTOR_R1, 0xff);
  }
  else
  {
    GPIOPinWrite(GPIO_PORTD_BASE, MOTOR_R0, 0xff);
    GPIOPinWrite(GPIO_PORTD_BASE, MOTOR_R1, 0x00);  
  }
  PWMPulseWidthSet(PWM_BASE, PWM_OUT_1, rspeed);
}

//前进
void forward()
{	
    //左轮
    GPIOPinWrite(GPIO_PORTG_BASE, MOTOR_L0, 0xff);     
    GPIOPinWrite(GPIO_PORTG_BASE, MOTOR_L1, 0x00);
    //右轮
    GPIOPinWrite(GPIO_PORTD_BASE, MOTOR_R0, 0xff);
    GPIOPinWrite(GPIO_PORTD_BASE, MOTOR_R1, 0x00);      	
    //使能开启
    //GPIOPinWrite(GPIO_PORTF_BASE, EN_l, 0xff);
    //GPIOPinWrite(GPIO_PORTF_BASE, EN_r, 0xff); 
}


//后退
void backward()
{	
    //左轮
    GPIOPinWrite(GPIO_PORTG_BASE, MOTOR_L0, 0x00);     
    GPIOPinWrite(GPIO_PORTG_BASE, MOTOR_L1, 0xff);
    //右轮
    GPIOPinWrite(GPIO_PORTD_BASE, MOTOR_R0, 0x00);
    GPIOPinWrite(GPIO_PORTD_BASE, MOTOR_R1, 0xff);
    //使能开启
    //GPIOPinWrite(GPIO_PORTF_BASE, EN_l, 0xff);
    //GPIOPinWrite(GPIO_PORTF_BASE, EN_r, 0xff); 
}

void stop()
{
  //左轮
    GPIOPinWrite(GPIO_PORTG_BASE, MOTOR_L0, 0x00);     
    GPIOPinWrite(GPIO_PORTG_BASE, MOTOR_L1, 0x00);
    //右轮
    GPIOPinWrite(GPIO_PORTD_BASE, MOTOR_R0, 0x00);
    GPIOPinWrite(GPIO_PORTD_BASE, MOTOR_R1, 0x00);
  //PWMPulseWidthSet(PWM_BASE, PWM_OUT_0, 10);//Left
  //PWMPulseWidthSet(PWM_BASE, PWM_OUT_1, 10);
}
//转弯，顺时针
void turn()
{	
    //左轮
    GPIOPinWrite(GPIO_PORTG_BASE, MOTOR_L0, 0xff);     
    GPIOPinWrite(GPIO_PORTG_BASE, MOTOR_L1, 0x00);
    //右轮
    GPIOPinWrite(GPIO_PORTD_BASE, MOTOR_R0, 0x00);
    GPIOPinWrite(GPIO_PORTD_BASE, MOTOR_R1, 0xff);
    //使能开启
    //GPIOPinWrite(GPIO_PORTF_BASE, EN_l, 0xff);
    //GPIOPinWrite(GPIO_PORTF_BASE, EN_r, 0xff); 
}

void stepAdjust()
{
        if(RightMotorStep >LeftMotorStep +20)
	{
          if(banlanceR<=delta)
           return; 
	  banlanceL = banlanceL+delta;
          banlanceR = banlanceR-delta;
	  PWMPulseWidthSet(PWM_BASE, PWM_OUT_0, banlanceL);//左
	  PWMPulseWidthSet(PWM_BASE, PWM_OUT_1, banlanceR);//右
          LeftMotorStep = 0;
          RightMotorStep = 0;
          ledTurnover(LED1);
	}
	//右轮转速小于左轮,调节左右两轮转速并清0两轮码盘计数
	else if(LeftMotorStep > RightMotorStep+20)
	{
          if(banlanceL<=delta)
            return;
	  banlanceL = banlanceL-delta;
	  banlanceR = banlanceR+delta;
	  PWMPulseWidthSet(PWM_BASE, PWM_OUT_1, banlanceR);//Right
	  PWMPulseWidthSet(PWM_BASE, PWM_OUT_0, banlanceL);//Left
          LeftMotorStep = 0;
          RightMotorStep = 0;
          ledTurnover(LED2);
	}
}

//转弯，逆时针
void turnLeft()
{
    //左轮
    GPIOPinWrite(GPIO_PORTG_BASE, MOTOR_L0, 0x00);     
    GPIOPinWrite(GPIO_PORTG_BASE, MOTOR_L1, 0xff);
    //右轮
    GPIOPinWrite(GPIO_PORTD_BASE, MOTOR_R0, 0xff);
    GPIOPinWrite(GPIO_PORTD_BASE, MOTOR_R1, 0x00);
}

//转弯，顺时针
void turnRight()
{
    //左轮
    GPIOPinWrite(GPIO_PORTG_BASE, MOTOR_L0, 0xff);     
    GPIOPinWrite(GPIO_PORTG_BASE, MOTOR_L1, 0x00);
    //右轮
    GPIOPinWrite(GPIO_PORTD_BASE, MOTOR_R0, 0x00);
    GPIOPinWrite(GPIO_PORTD_BASE, MOTOR_R1, 0xff);
}