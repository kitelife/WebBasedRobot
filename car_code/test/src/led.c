
#include "led.h"
#include "baseheadfile.h"



void initLed()
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    GPIODirModeSet(GPIO_PORTF_BASE, LED0 | LED1 | LED2, GPIO_DIR_MODE_OUT);
    GPIOPadConfigSet(GPIO_PORTF_BASE, LED0 | LED1 | LED2, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD);
    
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOH);
    GPIODirModeSet(GPIO_PORTH_BASE, GPIO_PIN_2, GPIO_DIR_MODE_IN); 
    GPIOPadConfigSet(GPIO_PORTH_BASE, GPIO_PIN_2, GPIO_STRENGTH_4MA, GPIO_PIN_TYPE_STD_WPU);
}
void ledTurnon(unsigned char pin)
{
  GPIOPinWrite(GPIO_PORTF_BASE,pin,0x00);
}
void ledTurnoff(unsigned char pin)
{
  GPIOPinWrite(GPIO_PORTF_BASE,pin,0xff);
}
void ledTurnover(unsigned char pin)
{
  if(GPIOPinRead(GPIO_PORTF_BASE,pin)==0)
  {
    ledTurnoff(pin);
  }
  else
  {
    ledTurnon(pin);
  }
}