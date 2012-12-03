#include "wireless_comm_test.h"
#include "nrf24l01.h"
#include "led.h"

extern unsigned long DelayTime;
extern L01Buf L01RcvBuf;
extern unsigned char Key1;
extern unsigned char Key0;


void wireless_comm_test(void)
{
  unsigned char rcvData[32]={0};
  DelayTime=0;
  unsigned int i,j;
  unsigned char berH=0,berL=0;
  //receive
  do
  {
    if(L01_Buf_Read(&L01RcvBuf,rcvData)==1)
    {	  
       for(i=0;i<31;i++)
       {
         if(rcvData[i]!=rcvData[i+1])
           break;
       }
       if(i==31)
       {
         ledTurnover(LED1);
         berL++;
         if(berL==0)
         {
           berH++;
         }
       }
    }
  }while(DelayTime<16000);//16s
 
  rcvData[0]='b';
  rcvData[1]='e';
  rcvData[2]='r';
  rcvData[3]=berH;
  rcvData[4]=berL;
  NRF24L01_TxPacket(SSI1_BASE,rcvData);//send the bit error rate (3 times for sure)
  delay_ms(8);
  NRF24L01_TxPacket(SSI1_BASE,rcvData);//send the bit error rate
  delay_ms(8);
  NRF24L01_TxPacket(SSI1_BASE,rcvData);//send the bit error rate
  delay_ms(8);
  
  delay_ms(500);// start Transmitting Test
  for(j=0;j<32;j++)
  {
    rcvData[j]=85;//01010101
  }
  for(i=0;i<1000;i++)
  {
    NRF24L01_TxPacket(SSI1_BASE,rcvData);
    ledTurnover(LED0);
    delay_ms(8);
  }
}