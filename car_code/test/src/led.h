#ifndef LED_H
#define LED_H
#include "baseheadfile.h"


#define LED0      GPIO_PIN_1//PF¶Ë¿Ú     led0
#define LED1      GPIO_PIN_2//PF¶Ë¿Ú     led1
#define LED2      GPIO_PIN_3//PF¶Ë¿Ú     led2

void initLed(void);
void ledTurnon(unsigned char pin);
void ledTurnoff(unsigned char pin);
void ledTurnover(unsigned char pin);
#endif