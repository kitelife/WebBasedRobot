#include "delay.h"
/*系统时钟频率8M
-----------------------------------------------------------------------*/

void delay_1ms(void)                 //1ms????
{
  unsigned int i; 
  for(i=0;i<2000;i++)//for每次循环四条指令？
  {}
}
  
void delay_ms(unsigned int n)       //N ms????
{
   unsigned int i=0;
   for (i=0;i<n;i++)
   delay_1ms();
}