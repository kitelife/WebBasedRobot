#include "key_led_test.h"

#include "led.h"
#include "keys.h"
#include "delay.h"

extern unsigned char TestNum;

void key_led_test(void)
{
  unsigned int i;
  //light water
  for(i=5;i>0;i--)
  {
    ledTurnoff(LED0);
    delay_ms(100*i);
    ledTurnon(LED0);
    delay_ms(100*i);
    
    ledTurnoff(LED1);
    delay_ms(100*i);
    ledTurnon(LED1);
    delay_ms(100*i);
    
    ledTurnoff(LED2);
    delay_ms(100*i);
    ledTurnon(LED2);
    delay_ms(100*i);
  }
}
/*
see the keys(buttons) test interrupt function
"void GPIO_Port_B_ISR(void)"
in the main.c file
*/