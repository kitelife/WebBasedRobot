#include "nrf24l01.h"

#define NUM_SSI_DATA 3

unsigned char P0_TX_ADDRESS[TX_ADR_WIDTH]={0xe7,0xe7,0xe7}; //发送地址
unsigned char P0_RX_ADDRESS[RX_ADR_WIDTH]={0xe7,0xe7,0xe7}; //接收地址

unsigned long ulDataTx[NUM_SSI_DATA];
unsigned long ulDataRx[NUM_SSI_DATA];
unsigned long ulindex =0;

unsigned char L01IRQSign=0;
L01Buf L01RcvBuf;

void initSSI0()
{
  SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI0);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
  
  GPIOPinConfigure(GPIO_PA2_SSI0CLK);//这貌似是另一个小车上的程序
  GPIOPinConfigure(GPIO_PA4_SSI0RX);
  GPIOPinConfigure(GPIO_PA5_SSI0TX);
  
  GPIOPinTypeSSI(GPIO_PORTA_BASE ,GPIO_PIN_2);
  GPIOPinTypeSSI(GPIO_PORTA_BASE ,GPIO_PIN_4);
  GPIOPinTypeSSI(GPIO_PORTA_BASE ,GPIO_PIN_5); 
  
  long rate;
  rate =  SysCtlClockGet();
  
  SSIConfigSetExpClk(SSI0_BASE, rate, SSI_FRF_MOTO_MODE_0, SSI_MODE_MASTER, rate>>1 , 8);
  
  SSIEnable(SSI0_BASE);

    //
    // Read any residual data from the SSI port.  This makes sure the receive
    // FIFOs are empty, so we don't read any unwanted junk.  This is done here
    // because the SPI SSI mode is full-duplex, which allows you to send and
    // receive at the same time.  The SSIDataGetNonBlocking function returns
    // "true" when data was returned, and "false" when no data was returned.
    // The "non-blocking" function checks if there is any data in the receive
    // FIFO and does not "hang" if there isn't.
    //t
    while(SSIDataGetNonBlocking(SSI0_BASE, &ulDataRx[0]))
    {
    }
}

void initSSI1()
{
  SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI1);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
  
  GPIOPinConfigure(GPIO_PE0_SSI1CLK);
  //GPIOPinConfigure(GPIO_PE1_SSI1FSS);
  GPIOPinConfigure(GPIO_PE2_SSI1RX);
  GPIOPinConfigure(GPIO_PE3_SSI1TX);
  
  GPIOPinTypeSSI(GPIO_PORTE_BASE ,GPIO_PIN_0);
  //GPIOPinTypeSSI(GPIO_PORTE_BASE ,GPIO_PIN_1);
  GPIOPinTypeSSI(GPIO_PORTE_BASE ,GPIO_PIN_2);
  GPIOPinTypeSSI(GPIO_PORTE_BASE ,GPIO_PIN_3); 
  
  long rate;
  rate =  SysCtlClockGet();
  
  SSIConfigSetExpClk(SSI1_BASE, rate, SSI_FRF_MOTO_MODE_0, SSI_MODE_MASTER, rate>>1 , 8);
  
  SSIEnable(SSI1_BASE);
  
  while(SSIDataGetNonBlocking(SSI1_BASE, &ulDataRx[0]))
    {
    }
}

void initControlPins0()
{
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA | SYSCTL_PERIPH_GPIOC);
  
  GPIODirModeSet(GPIO_PORTC_BASE,  CE1 | CSN1, GPIO_DIR_MODE_OUT);
  GPIOPadConfigSet(GPIO_PORTC_BASE, CE1 | CSN1, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD);
  
  GPIODirModeSet(GPIO_PORTA_BASE, IRQ1, GPIO_DIR_MODE_IN); 
  GPIOPadConfigSet(GPIO_PORTA_BASE, IRQ1, GPIO_STRENGTH_4MA, GPIO_PIN_TYPE_STD_WPU);
  GPIOIntTypeSet(GPIO_PORTA_BASE, IRQ1, GPIO_FALLING_EDGE);
  GPIOPinIntEnable(GPIO_PORTA_BASE, IRQ1);
   GPIOPinIntClear(GPIO_PORTA_BASE, IRQ1);
  IntEnable(INT_GPIOA);
}

void initControlPins1()
{
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB | SYSCTL_PERIPH_GPIOE);
  
  GPIODirModeSet(GPIO_PORTB_BASE,  CE2, GPIO_DIR_MODE_OUT);
  GPIOPadConfigSet(GPIO_PORTB_BASE, CE2, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD);
  
  GPIODirModeSet(GPIO_PORTE_BASE,  CSN2, GPIO_DIR_MODE_OUT);
  GPIOPadConfigSet(GPIO_PORTE_BASE, CSN2, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD);
  
  GPIODirModeSet(GPIO_PORTB_BASE, IRQ2, GPIO_DIR_MODE_IN); 
  GPIOPadConfigSet(GPIO_PORTB_BASE, IRQ2, GPIO_STRENGTH_4MA, GPIO_PIN_TYPE_STD_WPU);
  GPIOIntTypeSet(GPIO_PORTB_BASE, IRQ2, GPIO_FALLING_EDGE);
  GPIOPinIntEnable(GPIO_PORTB_BASE, IRQ2);
  GPIOPinIntClear(GPIO_PORTB_BASE, IRQ2);
  IntEnable(INT_GPIOB);
}

/*==================================================
#define nRF1_CEH    GPIOPinWrite(GPIO_PORTC_BASE, CE1, 0xff)
#define nRF1_CEL    GPIOPinWrite(GPIO_PORTC_BASE, CE1, 0x00)
#define nRF1_CSNH   GPIOPinWrite(GPIO_PORTC_BASE, CSN1, 0xff)
#define nRF1_CSNL   GPIOPinWrite(GPIO_PORTC_BASE, CSN1, 0x00)
====================================================*/
void initNrf24l01()
{  
  L01IRQSign=0;
  L01RcvBuf.c_u8Num=0;
  L01RcvBuf.i_u8SavePtr=L01RcvBuf.i_u8GetPtr=0;
  
  initSSI0();
  initControlPins0();
  
  initSSI1();
  initControlPins1();
}

unsigned char SSI_RW_Byte(unsigned long ulBase, unsigned char byte)
{
  unsigned char ucVal; 
  unsigned long ulVal; 
  
  ASSERT((ulBase == SSI0_BASE) || (ulBase == SSI1_BASE));
  
  SSIDataPut(ulBase, byte);
  while(SSIBusy(ulBase));
  SSIDataGet(ulBase, &ulVal);
  ucVal = (unsigned char)ulVal;
  return ucVal;
}

//SPI写寄存器
//reg:指定寄存器地址
//value:写入的值
unsigned char NRF24L01_Write_Reg(unsigned long ulBase, unsigned char reg, unsigned char value)
{
  unsigned char status;
  
  ASSERT((ulBase == SSI0_BASE) || (ulBase == SSI1_BASE));
  
  if(ulBase == SSI0_BASE)             //使能SPI传输
    Clr_nRF1_CSN;
  else
    Clr_nRF2_CSN;
  
  status =SSI_RW_Byte(ulBase,reg);   //发送寄存器号 
  SSI_RW_Byte(ulBase, value);         //写入寄存器的值
  
  if(ulBase == SSI0_BASE)             //禁止SPI传输
    Set_nRF1_CSN;
  else
    Set_nRF2_CSN;
  
  return(status);       	      //返回状态值
}

//读取SPI寄存器值
//reg:要读的寄存器
unsigned char NRF24L01_Read_Reg(unsigned long ulBase, unsigned char reg)
{
  unsigned char reg_val;	
  
  ASSERT((ulBase == SSI0_BASE) || (ulBase == SSI1_BASE));
  
  if(ulBase == SSI0_BASE)             //使能SPI传输
    Clr_nRF1_CSN;
  else
    Clr_nRF2_CSN;	
  
  SSI_RW_Byte(ulBase,reg);                   //发送寄存器号
  reg_val=SSI_RW_Byte(ulBase,0XFF);          //读取寄存器内容

  if(ulBase == SSI0_BASE)             //禁止SPI传输
    Set_nRF1_CSN;
  else
    Set_nRF2_CSN;
  
  return(reg_val);                    //返回状态值
}


//在指定位置读出指定长度的数据
//reg:寄存器地址
//*pBuf:待读出数据指针
//len:读出数据长度
//返回值,此次读到的状态寄存器值 
unsigned char NRF24L01_Read_Buf(unsigned long ulBase, unsigned char reg,unsigned char *pBuf,unsigned char len)
{
  unsigned char status,u8_ctr;
  
  ASSERT((ulBase == SSI0_BASE) || (ulBase == SSI1_BASE));
  
  if(ulBase == SSI0_BASE)                    //使能SPI传输
    Clr_nRF1_CSN;
  else
    Clr_nRF2_CSN;
  
  status=SSI_RW_Byte(ulBase,reg);            //发送寄存器值(位置),并读取状态值   	   
  for(u8_ctr=0; u8_ctr<len; u8_ctr++)
  pBuf[u8_ctr]=SSI_RW_Byte(ulBase,0XFF);     //读出数据
  
  if(ulBase == SSI0_BASE)             //禁止SPI传输
    Set_nRF1_CSN;
  else
    Set_nRF2_CSN;
  
  return status;                             //返回读到的状态值
}

//在指定位置写指定长度的数据
//reg:寄存器(位置)
//*pBuf:数据指针
//len:数据长度
//返回值,此次读到的状态寄存器值
unsigned char NRF24L01_Write_Buf(unsigned long ulBase, unsigned char reg, unsigned char *pBuf, unsigned char len)
{
  unsigned char status,u8_ctr;
  
  ASSERT((ulBase == SSI0_BASE) || (ulBase == SSI1_BASE));
  
  if(ulBase == SSI0_BASE)                    //使能SPI传输
    Clr_nRF1_CSN;
  else
    Clr_nRF2_CSN;
  
  status = SSI_RW_Byte(ulBase,reg);//发送寄存器值(位置),并读取状态值
  for(u8_ctr=0; u8_ctr<len; u8_ctr++)
    SSI_RW_Byte(ulBase,*pBuf++); //写入数据
  
  if(ulBase == SSI0_BASE)             //禁止SPI传输
    Set_nRF1_CSN;
  else
    Set_nRF2_CSN;
  
  return status;          //返回写到的状态值
}


/************************************************
 * Description: Tx a packet with retreat on
 *
 * Arguments:
 * 	ulBase -- indicate which nRF24L01 is used.
 *      txbuf -- pointer of data that need to be transmited
 * Return:
 * 	TRUE -- If phy in idle state
 * 	FALSE -- If tx in progress
 *
 * Date: 2010-05-21
 ***********************************************/
unsigned char TxPacket_retreat(unsigned long ulBase, unsigned char *txbuf)
{
  unsigned char sta;
    
  ASSERT((ulBase == SSI0_BASE) || (ulBase == SSI1_BASE));
  
  if(ulBase == SSI0_BASE)                    //使能SPI传输
    Clr_nRF1_CE;
  else
    Clr_nRF2_CE;
  
  NRF24L01_Write_Buf(ulBase,WR_TX_PLOAD,txbuf,TX_PLOAD_WIDTH);//写数据到TX BUF  32个字节
  	
  while(!NRF24L01_Read_Reg(ulBase,READ_REG|CD));
  
  
  
  if(ulBase == SSI0_BASE)                    //启动发送
    Set_nRF1_CE;
  else
    Set_nRF2_CE;
  
  //while(NRF24L01_IRQ!=0);//等待发送完成
  if(ulBase == SSI0_BASE)
    while((GPIOPinRead(GPIO_PORTA_BASE,IRQ1) & IRQ1) !=0);//等待发送完成
  else
    while((GPIOPinRead(GPIO_PORTB_BASE,IRQ2) & IRQ1) !=0);
  
  
  sta=NRF24L01_Read_Reg(ulBase,READ_REG|STATUS);  //读取状态寄存器的值	   
  
  NRF24L01_Write_Reg(ulBase,WRITE_REG|STATUS,sta); //清除TX_DS或MAX_RT中断标志
  
  if(sta&MAX_TX)//达到最大重发次数
  {
    NRF24L01_Write_Reg(ulBase,FLUSH_TX,0xff);//清除TX FIFO寄存器 
    return MAX_TX; 
  }
  if(sta&TX_OK)//发送完成
  {
  return TX_OK;
  }
  return 0xff;//其他原因发送失败

}

//启动NRF24L01发送一次数据
//txbuf:待发送数据首地址
//返回值:发送完成状况
unsigned char NRF24L01_TxPacket(unsigned long ulBase, unsigned char *txbuf)
{
  unsigned char sta;
  
  ASSERT((ulBase == SSI0_BASE) || (ulBase == SSI1_BASE));
  
  if(ulBase == SSI0_BASE)                    //使能SPI传输
    Clr_nRF1_CE;
  else
    Clr_nRF2_CE;
  
  NRF24L01_Write_Buf(ulBase,WR_TX_PLOAD,txbuf,TX_PLOAD_WIDTH);//写数据到TX BUF  32个字节
  
  if(ulBase == SSI0_BASE)                    //启动发送
    Set_nRF1_CE;
  else
    Set_nRF2_CE;
  
  //while(NRF24L01_IRQ!=0);//等待发送完成
  if(ulBase == SSI0_BASE)
    while((GPIOPinRead(GPIO_PORTA_BASE,IRQ1) & IRQ1) !=0);//等待发送完成
  else
    while((GPIOPinRead(GPIO_PORTB_BASE,IRQ2) & IRQ2) !=0);
  
  
  sta=NRF24L01_Read_Reg(ulBase,READ_REG|STATUS);  //读取状态寄存器的值	   
  NRF24L01_Write_Reg(ulBase,WRITE_REG|STATUS,sta); //清除TX_DS或MAX_RT中断标志
  
 /* if(sta&MAX_TX)//达到最大重发次数
  {
    NRF24L01_Write_Reg(ulBase,FLUSH_TX,0xff);//清除TX FIFO寄存器 
    return MAX_TX; 
  }
  if(sta&TX_OK)//发送完成
  {
    return TX_OK;
  }*/
  return 0xff;//其他原因发送失败
}

//启动NRF24L01接收一次数据
//rxbuf:待接收数据首地址
//返回值:0，接收完成；其他，错误代码
unsigned char NRF24L01_RxPacket(unsigned long ulBase)
{
  unsigned char sta;		    							   
  unsigned char rxbuf[32];
  sta=NRF24L01_Read_Reg(ulBase,READ_REG | STATUS);  //读取状态寄存器的值    	 
  NRF24L01_Write_Reg(ulBase, WRITE_REG | STATUS, sta); //清除TX_DS或MAX_RT中断标志
  if(sta&RX_OK)//接收到数据
  {
    NRF24L01_Read_Buf(ulBase,RD_RX_PLOAD,rxbuf,RX_PLOAD_WIDTH);//读取数据
    NRF24L01_Write_Reg(ulBase,FLUSH_RX,0xff);//清除RX FIFO寄存器 
    L01_Buf_Write( &L01RcvBuf, rxbuf );
    return 0; 
  }	   
  return 1;//没收到任何数据

}	
					    
//该函数初始化NRF24L01到RX模式
//设置RX地址,写RX数据宽度,选择RF频道,波特率和LNA HCURR
//当CE变高后,即进入RX模式,并可以接收数据了		   
void RX_Mode(unsigned long ulBase)
{
//	Clr_NRF24L01_CE;
  ASSERT((ulBase == SSI0_BASE) || (ulBase == SSI1_BASE));
  
  if(ulBase == SSI0_BASE)                    //使能SPI传输
    Clr_nRF1_CE;
  else
    Clr_nRF2_CE;
  NRF24L01_Write_Reg(ulBase,WRITE_REG|SETUP_AW, RX_ADR_WIDTH-2);     
  NRF24L01_Write_Buf(ulBase,WRITE_REG|RX_ADDR_P0,(unsigned char*)P0_RX_ADDRESS,RX_ADR_WIDTH);//写RX节点地址
   
  NRF24L01_Write_Reg(ulBase,WRITE_REG|EN_RXADDR,0x01);//使能通道0的接收地址  	 
  NRF24L01_Write_Reg(ulBase,WRITE_REG|RF_CH,POWER);	     //设置RF通信频率2.4GHZ		  
  NRF24L01_Write_Reg(ulBase,WRITE_REG|RX_PW_P0,RX_PLOAD_WIDTH);//选择通道0的有效数据宽度,32字节 	    
  NRF24L01_Write_Reg(ulBase,WRITE_REG|RF_SETUP,0x0f);//设置RX发射参数,0db增益,2Mbps,低噪声增益开启   
  NRF24L01_Write_Reg(ulBase,WRITE_REG|CONFIG, 0x0f);//配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC,接收模式 
  
  NRF24L01_Write_Reg(ulBase,WRITE_REG|EN_AA,0x00);    //禁止通道0的自动应答  
  NRF24L01_Write_Reg(ulBase,WRITE_REG|SETUP_RETR,0x00);    //自动重发次数为0  
  
  if(ulBase == SSI0_BASE)                    //CE为高,进入接收模式
    Set_nRF1_CE;
  else
    Set_nRF2_CE;
}
							 
//该函数初始化NRF24L01到TX模式
//设置TX地址,写TX数据宽度,设置RX自动应答的地址,填充TX发送数据,选择RF频道,波特率和LNA HCURR(低噪声放大增益)
//PWR_UP,CRC使能
//当CE变高后,即进入RX模式,并可以接收数据了		   
//CE为高大于10us,则启动发送.	 
void TX_Mode(unsigned long ulBase)
{														 
  ASSERT((ulBase == SSI0_BASE) || (ulBase == SSI1_BASE));
  
  if(ulBase == SSI0_BASE)                    //使能SPI传输
    Clr_nRF1_CE;
  else
    Clr_nRF2_CE;
  NRF24L01_Write_Reg(ulBase,WRITE_REG|SETUP_AW, RX_ADR_WIDTH-2); 
  NRF24L01_Write_Buf(ulBase,WRITE_REG|TX_ADDR,(unsigned char*)P0_TX_ADDRESS,TX_ADR_WIDTH);//写TX节点地址 	  
  
  NRF24L01_Write_Reg(ulBase,WRITE_REG|RF_CH,POWER);       //设置RF通道为40
  NRF24L01_Write_Reg(ulBase,WRITE_REG|RF_SETUP,0x0f);  //设置TX发射参数,0db增益,2Mbps,低噪声增益开启   
  NRF24L01_Write_Reg(ulBase,WRITE_REG|CONFIG,0x0e);    //配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC,发射模式,开启所有中断
//	Set_NRF24L01_CE;//CE为高,10us后启动发送
  
  NRF24L01_Write_Reg(ulBase,WRITE_REG|EN_AA,0x00);     //禁止通道0的自动应答      
  NRF24L01_Write_Reg(ulBase,WRITE_REG|SETUP_RETR,0x00);//设置自动重发关闭
  if(ulBase == SSI0_BASE)                    //CE为高,进入接收模式
    Set_nRF1_CE;
  else
    Set_nRF2_CE;
}

/******************************************************************************
*				bool	L01_Buf_Read( L01Buf buf, u8 *value )
*
* Function Name:  L01_Buf_Read
*
* Description  :  这个函数用于对这L01两个缓冲区的读操作
*
* Input        :  L01Buf buf,可取值 L01RcvBuf，L01SendBuf     
*
* Output       :  value, 读取的32位数据
*
* Return       :  读是否有效
*******************************************************************************/
unsigned char L01_Buf_Read( L01Buf *buf, unsigned char *pValue )
{
	int i;
	if(buf->c_u8Num==0)
	{
		return 0;
	}
	
	for(i=0;i<PACKAGE_SIZE;i++)
	{
		*(pValue+i)=buf->p_u8Data[buf->i_u8GetPtr][i];
	}
	buf->i_u8GetPtr = (buf->i_u8GetPtr+1)&(L01_BUF_SIZE-1);	
	buf->c_u8Num--;
	return 1;

}

/******************************************************************************
*				bool L01_Buf_Write( L01Buf buf, u8 *pValue )
*
* Function Name:  USART_Buf_Write
*
* Description  :  这个函数用于对这两个缓冲区的写操作
*
* Input        :  L01Buf buf,可取值 L01RcvBuf，L01SendBuf     
*
* Output       :  value, 写入的32位数据
*
* Return       :  写是否有效
*******************************************************************************/
unsigned char L01_Buf_Write( L01Buf *buf, unsigned char *pValue )
{
	int i;
	if(buf->c_u8Num==L01_BUF_SIZE)
	{
		return 0;
	}
	
	for(i=0;i<PACKAGE_SIZE;i++)
	{
		buf->p_u8Data[buf->i_u8SavePtr][i] = *(pValue+i);
	}	
	buf->i_u8SavePtr = (buf->i_u8SavePtr+1)&(L01_BUF_SIZE-1); 
	buf->c_u8Num++;
	return 1;
}

/*
//旧代码
void GPIO_Port_B_ISR(void)
{
  unsigned char sta;
  unsigned char recvbuf[32];
 // unsigned long a=0;
  long intstatus = GPIOPinIntStatus(GPIO_PORTB_BASE, 0);
  
  if(intstatus & IRQ2){
      sta=NRF24L01_Read_Reg(SSI1_BASE,READ_REG|STATUS);
      if(sta & RX_OK){
        if(NRF24L01_RxPacket(SSI1_BASE,recvbuf) == 0){
          if(recvbuf[0]==0xff){
            //ledTurnover(LED1);
            //trx_width-=4;
            RX_Mode(SSI1_BASE);            
          }else if(recvbuf[0]==0xf8){
            sendback=1;
          }else if(recvbuf[0]==0x00){
         //   ledTurnover(LED2);
            rightrate[recvbuf[0]]++;
          }
        }
      }
      NRF24L01_Write_Reg(SSI1_BASE,WRITE_REG|STATUS,sta); //清除TX_DS或MAX_RT中断标志
  }
  
  GPIOPinIntClear(GPIO_PORTB_BASE,SW1 | SW2 | IRQ2);
}
*/