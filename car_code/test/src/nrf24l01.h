#ifndef NRF24L01_H
#define NRF24L01_H

#include "hw_memmap.h"
#include "hw_types.h"
#include "interrupt.h"   
#include "hw_ints.h" 
#include "debug.h"
#include "delay.h"
#include "gpio.h" 
#include "ssi.h"


#define CE1 GPIO_PIN_5  //PORT C
#define CSN1 GPIO_PIN_4  //PORT C
#define IRQ1 GPIO_PIN_6  //PORT A

#define CE2 GPIO_PIN_3  //PORT B
#define CSN2 GPIO_PIN_1  //PORT E
#define IRQ2 GPIO_PIN_2 //PORT B

#define Set_nRF1_CE    GPIOPinWrite(GPIO_PORTC_BASE, CE1, 0xff)
#define Clr_nRF1_CE    GPIOPinWrite(GPIO_PORTC_BASE, CE1, 0x00)
#define Set_nRF1_CSN   GPIOPinWrite(GPIO_PORTC_BASE, CSN1, 0xff)
#define Clr_nRF1_CSN   GPIOPinWrite(GPIO_PORTC_BASE, CSN1, 0x00)

#define Set_nRF2_CE    GPIOPinWrite(GPIO_PORTB_BASE, CE2, 0xff)
#define Clr_nRF2_CE    GPIOPinWrite(GPIO_PORTB_BASE, CE2, 0x00)
#define Set_nRF2_CSN   GPIOPinWrite(GPIO_PORTE_BASE, CSN2, 0xff)
#define Clr_nRF2_CSN   GPIOPinWrite(GPIO_PORTE_BASE, CSN2, 0x00)


//***************************************************//
#define CONFIG          0x00  // 'Config' register address
#define EN_AA           0x01  // 'Enable Auto Acknowledgment' register address
#define EN_RXADDR       0x02  // 'Enabled RX addresses' register address
#define SETUP_AW        0x03  // 'Setup address width' register address
#define SETUP_RETR      0x04  // 'Setup Auto. Retrans' register address
#define RF_CH           0x05  // 'RF channel' register address
#define RF_SETUP        0x06  // 'RF setup' register address
#define STATUS          0x07  // 'Status' register address//状态寄存器;bit0:TX FIFO满标志;bit3:1,接收数据通道号(最大:6);bit4,达到最多次重发
                                    //bit5:数据发送完成中断;bit6:接收数据中断;

#define MAX_TX  	0x10        //达到最大发送次数中断
#define TX_OK   	0x20        //TX发送完成中断
#define RX_OK   	0x40       //接收到数据中断

#define OBSERVE_TX      0x08  // 'Observe TX' register address
#define CD              0x09  // 'Carrier Detect' register address
#define RX_ADDR_P0      0x0A  // 'RX address pipe0' register address
#define RX_ADDR_P1      0x0B  // 'RX address pipe1' register address
#define RX_ADDR_P2      0x0C  // 'RX address pipe2' register address
#define RX_ADDR_P3      0x0D  // 'RX address pipe3' register address
#define RX_ADDR_P4      0x0E  // 'RX address pipe4' register address
#define RX_ADDR_P5      0x0F  // 'RX address pipe5' register address
#define TX_ADDR         0x10  // 'TX address' register address
#define RX_PW_P0        0x11  // 'RX payload width, pipe0' register address
#define RX_PW_P1        0x12  // 'RX payload width, pipe1' register address
#define RX_PW_P2        0x13  // 'RX payload width, pipe2' register address
#define RX_PW_P3        0x14  // 'RX payload width, pipe3' register address
#define RX_PW_P4        0x15  // 'RX payload width, pipe4' register address
#define RX_PW_P5        0x16  // 'RX payload width, pipe5' register address
#define FIFO_STATUS     0x17  // 'FIFO Status Register' register address
#define MAX_RT  	0x10  // Max #of TX retrans interrupt
#define TX_DS   	0x20  // TX data sent interrupt
#define RX_DR   	0x40  // RX data received

#define READ_REG        0x00  // Define read command to register
#define WRITE_REG       0x20  // Define write command to register
#define RD_RX_PLOAD     0x61  // Define RX payload register address
#define WR_TX_PLOAD     0xA0  // Define TX payload register address
#define FLUSH_TX        0xE1  // Define flush TX register command
#define FLUSH_RX        0xE2  // Define flush RX register command
#define REUSE_TX_PL     0xE3  // Define reuse TX payload register command



//24L01发送接收数据宽度定义
#define TX_ADR_WIDTH    3   //5字节的地址宽度
#define RX_ADR_WIDTH    3   //5字节的地址宽度
#define TX_PLOAD_WIDTH  32  //20字节的用户数据宽度
#define RX_PLOAD_WIDTH  32  //20字节的用户数据宽度
#define POWER 0

#define   L01_BUF_SIZE 64
#define   PACKAGE_SIZE 32
typedef struct  //发送和接收缓冲区定义（环形）
{
	unsigned char   p_u8Data[L01_BUF_SIZE][PACKAGE_SIZE];
        unsigned char   c_u8Num;
	unsigned char   i_u8SavePtr;
	unsigned char   i_u8GetPtr;
}L01Buf;

void initNrf24l01();
unsigned char NRF24L01_Write_Reg(unsigned long ulBase, unsigned char reg, unsigned char value);
unsigned char NRF24L01_Read_Reg(unsigned long ulBase, unsigned char reg);
unsigned char NRF24L01_Read_Buf(unsigned long ulBase, unsigned char reg,unsigned char *pBuf,unsigned char len);
unsigned char NRF24L01_Write_Buf(unsigned long ulBase, unsigned char reg, unsigned char *pBuf, unsigned char len);
unsigned char NRF24L01_TxPacket(unsigned long ulBase, unsigned char *txbuf);
unsigned char NRF24L01_RxPacket(unsigned long ulBase);
void RX_Mode(unsigned long ulBase);
void TX_Mode(unsigned long ulBase);
unsigned char L01_Buf_Read( L01Buf *buf, unsigned char *pValue );
unsigned char L01_Buf_Write( L01Buf *buf, unsigned char *pValue );

#endif