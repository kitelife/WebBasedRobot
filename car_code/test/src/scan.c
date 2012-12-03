#include "scan.h"
#include "nrf24l01.h"
#include "baseheadfile.h"
#include "led.h"

extern L01Buf L01RcvBuf;

unsigned char Scan(void)
{
  unsigned char testnum;
  unsigned char rcvData[32];
  if(L01_Buf_Read(&L01RcvBuf,rcvData)==1)
  {	 
    if((rcvData[0]=='t') && (rcvData[1]=='s'))
    {
       ledTurnon(LED2);//
       testnum=rcvData[2]-'0';
    }
  }
  else 
  {
    testnum=0;
  }
  return testnum;
}