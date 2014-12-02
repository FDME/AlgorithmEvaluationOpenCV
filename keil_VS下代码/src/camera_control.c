/*********************************************************************************************
* File：	uart_communication.c
* Author:	embest
* Desc：	uart test file
* History:	wang Fub 16th 2006
*		
*********************************************************************************************/
/*------------------------------------------------------------------------------------------*/
/*                                     include files	                                    */
/*------------------------------------------------------------------------------------------*/
#include "header.h"
/*********************************************************************************************
* name:		uart0_test
* func:		uart test function
* para:		none
* ret:		none
* modify:
* comment:		
*********************************************************************************************/
extern UINT8T buffer[65535];
#ifdef WIN32
#else
void ResetSize()  //640*480
{
	uart_sendbyte(0x56);
	uart_sendbyte(0x00);
	uart_sendbyte(0x31);
	uart_sendbyte(0x05);
	uart_sendbyte(0x04);
	uart_sendbyte(0x01);
	uart_sendbyte(0x00);
	uart_sendbyte(0x19);
	uart_sendbyte(0x00);
}

void SendResetCmd()
{
	uart_sendbyte(0x56);
	uart_sendbyte(0x00);
	uart_sendbyte(0x26);
	uart_sendbyte(0x00);
}

void SendTakePhotoCmd()
{
	uart_sendbyte(0x56);
	uart_sendbyte(0x00);
	uart_sendbyte(0x36);
	uart_sendbyte(0x01);
	uart_sendbyte(0x00);
}

void SendReadDataCmd(UINT8T LH,UINT8T LL)
{
	uart_sendbyte(0x56);
	uart_sendbyte(0x00);
	uart_sendbyte(0x32);
	uart_sendbyte(0x0C);
	uart_sendbyte(0x00);
	uart_sendbyte(0x0A);
	uart_sendbyte(0x00);
	uart_sendbyte(0x00);
	uart_sendbyte(0x00);
	uart_sendbyte(0x00);
	uart_sendbyte(0x00);
	uart_sendbyte(0x00);
	uart_sendbyte(LH);
	uart_sendbyte(LL);
	uart_sendbyte(0x00);
	uart_sendbyte(0x0A);
}

void ReadSize()
{
	uart_sendbyte(0x56);
	uart_sendbyte(0x00);
	uart_sendbyte(0x34);
	uart_sendbyte(0x01);
	uart_sendbyte(0x00);
}
void ChangeBaudRate()
{
	uart_sendbyte(0x56);
	uart_sendbyte(0x00);
	uart_sendbyte(0x24);
	uart_sendbyte(0x03);
	uart_sendbyte(0x01);
	uart_sendbyte(0x0D);
	uart_sendbyte(0xA6);
}

UINT16T camera()
{
	UINT8T incomingbyte;
	UINT8T LH, LL;//file size
	UINT16T size;
	UINT16T j = 0, k = 0;
	memset(buffer,'\0',0xFFFF);
	uart_init(0,38400,1); //串口1
	uart_select(UART1);
	delay(4000);
	ResetSize();
	delay(4000);
	SendResetCmd();
	delay(4000);
	SendTakePhotoCmd();
	delay(4000);
	while(rUTRSTAT1 & 0x1)
		incomingbyte = RdURXH1();
	
	ReadSize();
	while(k<9)
	{
		buffer[k] = uart_getch();
		k++;
	}
	k = 0;
	LH = buffer[7];
	LL = buffer[8];

	size = (UINT16T)LH*0x100+LL;
	memset(buffer,'\0',0xFFFF);
	
	SendReadDataCmd(LH,LL);
	while(j<size)
	{
		incomingbyte = uart_getch();
		k++;
		if(k >5) //前五位数据为标识符
		{
			buffer[j] = incomingbyte;
			j++;
		}
	}

	return size;
}
#endif

