#include "walk_test.h"
#include "nrf24l01.h"
#include "motor.h"
#include "delay.h"

extern L01Buf L01RcvBuf;
extern unsigned long DelayTime;

//(1) "ts3"+0+ left speed+ right speed+ hold time+... =>set speed,if the speed is more than 100,keep the old value,if the
//														  time=0, don't set the hold time(infinite)
//(2) "ts3"+1+...  =>stop the test   

void walk_test(void)
{
  unsigned char rcvData[32]={0};

  RX_Mode(SSI1_BASE);
  while(1)//is it appropriate? when the com is disconnected?
  {
    if(L01_Buf_Read(&L01RcvBuf,rcvData)==1 && rcvData[0]=='t' && rcvData[1]=='s' && rcvData[2]=='3' )
    {	  
       if(rcvData[3]=='0')
       {
         //left
         if(rcvData[4]<=100)
         {
           leftspeedSet(1,((unsigned long)rcvData[4])*10);// x% * 1000=x*10
         }
         else if(rcvData[4]>=156)//(-1,-100) 
         {
          leftspeedSet(0,(256-(unsigned long)(rcvData[4]))*10);// x% * 1000=x*10
         }
                   
         //right
         if(rcvData[5]<=100)
         {
           rightspeedSet(1,((unsigned long)rcvData[5])*10);// x% * 1000=x*10
         }
         else if(rcvData[5]>=156)
         {
           rightspeedSet(0,(256-(unsigned long)(rcvData[5]))*10);// x% * 1000=x*10
         }
        
         //hold time               
         if(rcvData[6]>0 && rcvData[6]<=100)//hold time,if hold time is not zero,the robot will be stoped after this move.
         {
           DelayTime=0;
           while(DelayTime<50*(unsigned long)(rcvData[6]));
           stop();
         }
        }
       else //stop the running and break the walk-test
       {
         stop();
         break;
       }
    }
  }
}