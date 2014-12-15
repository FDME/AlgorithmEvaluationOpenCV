#ifndef __HEADER_H__
#define __HEADER_H__

#ifdef WIN32
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<opencv/cv.h>
#include<opencv/highgui.h>

#ifndef __2410lib_h__
#define __2410lib_h__

#ifdef __cplusplus
extern "C" {
#endif

	/*------------------------------------------------------------------------------------------*/
	/*	 								constant define						 				    */
	/*------------------------------------------------------------------------------------------*/
#define DOWNLOAD_ADDRESS _RAM_STARTADDRESS
#define DEBUG_OUT uart_printf

#define min(x1,x2) (((x1)<(x2))? (x1):(x2))
#define max(x1,x2) (((x1)>(x2))? (x1):(x2))

#define ONESEC0 (62500)				// 16us resolution, max 1.04 sec
#define ONESEC1 (31250)				// 32us resolution, max 2.09 sec
#define ONESEC2 (15625)				// 64us resolution, max 4.19 sec
#define ONESEC3 (7812)				//128us resolution, max 8.38 sec
#define ONESEC4 (PCLK/128/(0xff+1))	// @60Mhz, 128*4us resolution, max 32.53 sec

//#define NULL 0
#define LCD   0xDD					// LCD symbol
#define UART0 0x0					// UART symbol
#define UART1 0x1
#define UART2 0xBB
#endif  //__2410lib_h__

#ifndef __DEF_H__
#define __DEF_H__

#define UINT32T unsigned int
#define INT32T	int
#define UINT16T unsigned short
#define INT16T	short int
#define UINT8T  unsigned char
#define	INT8T  char

#define TRUE 	1
#define FALSE 	0
#define OK		1
#define FAIL	0
#define FileEnd	1
#define	NotEnd	0 

#define False 0
#define True !False

	/* Sets the result on bPort */
#define BIT_SET(bPort,bBitMask)        (bPort |= bBitMask)
#define BIT_CLR(bPort,bBitMask)        (bPort &= ~bBitMask)

	/* Returns the result */
#define GET_BIT_SET(bPort,bBitMask)    (bPort | bBitMask)
#define GET_BIT_CLR(bPort,bBitMask)    (bPort & ~bBitMask)

	/* Returns 0 if the condition is False & a non-zero value if it is True */
#define TEST_BIT_SET(bPort,bBitMask)   (bPort & bBitMask)
#define TEST_BIT_CLR(bPort,bBitMask)   ((~bPort) & bBitMask)

#endif /*__DEF_H__*/

#else
//#include "2410lib.h"
//以下沿用2410lib.h, 待改

#ifndef __2410lib_h__
#define __2410lib_h__
#ifdef __cplusplus
extern "C" {
#endif

	/*------------------------------------------------------------------------------------------*/
	/*	 								constant define						 				    */
	/*------------------------------------------------------------------------------------------*/
#define DOWNLOAD_ADDRESS _RAM_STARTADDRESS
#define DEBUG_OUT uart_printf

#define min(x1,x2) (((x1)<(x2))? (x1):(x2))
#define max(x1,x2) (((x1)>(x2))? (x1):(x2))

#define ONESEC0 (62500)				// 16us resolution, max 1.04 sec
#define ONESEC1 (31250)				// 32us resolution, max 2.09 sec
#define ONESEC2 (15625)				// 64us resolution, max 4.19 sec
#define ONESEC3 (7812)				//128us resolution, max 8.38 sec
#define ONESEC4 (PCLK/128/(0xff+1))	// @60Mhz, 128*4us resolution, max 32.53 sec

//#define NULL 0
#define LCD   0xDD					// LCD symbol
#define UART0 0x0					// UART symbol
#define UART1 0x1
#define UART2 0xBB
#endif  //__2410lib_h__

#ifndef __DEF_H__
#define __DEF_H__

#define UINT32T unsigned int
#define INT32T	int
#define UINT16T unsigned short
#define INT16T	short int
#define UINT8T  unsigned char
#define	INT8T  char

#define TRUE 	1
#define FALSE 	0
#define OK		1
#define FAIL	0
#define FileEnd	1
#define	NotEnd	0

#define False 0
#define True !False

	/* Sets the result on bPort */
#define BIT_SET(bPort,bBitMask)        (bPort |= bBitMask)
#define BIT_CLR(bPort,bBitMask)        (bPort &= ~bBitMask)

	/* Returns the result */
#define GET_BIT_SET(bPort,bBitMask)    (bPort | bBitMask)
#define GET_BIT_CLR(bPort,bBitMask)    (bPort & ~bBitMask)

	/* Returns 0 if the condition is False & a non-zero value if it is True */
#define TEST_BIT_SET(bPort,bBitMask)   (bPort & bBitMask)
#define TEST_BIT_CLR(bPort,bBitMask)   ((~bPort) & bBitMask)

#endif /*__DEF_H__*/

//2410addr.h
#ifdef __BIG_ENDIAN
#define rUTXH0      (*(volatile unsigned char *)0x50000023) //UART 0 Transmission Hold
#define rURXH0      (*(volatile unsigned char *)0x50000027) //UART 0 Receive buffer
#define rUTXH1      (*(volatile unsigned char *)0x50004023) //UART 1 Transmission Hold
#define rURXH1      (*(volatile unsigned char *)0x50004027) //UART 1 Receive buffer
#define rUTXH2      (*(volatile unsigned char *)0x50008023) //UART 2 Transmission Hold
#define rURXH2      (*(volatile unsigned char *)0x50008027) //UART 2 Receive buffer

#define WrUTXH0(ch) (*(volatile unsigned char *)0x50000023)=(unsigned char)(ch)
#define RdURXH0()   (*(volatile unsigned char *)0x50000027)
#define WrUTXH1(ch) (*(volatile unsigned char *)0x50004023)=(unsigned char)(ch)
#define RdURXH1()   (*(volatile unsigned char *)0x50004027)
#define WrUTXH2(ch) (*(volatile unsigned char *)0x50008023)=(unsigned char)(ch)
#define RdURXH2()   (*(volatile unsigned char *)0x50008027)

#define UTXH0       (0x50000020+3)  //Byte_access address by DMA
#define URXH0       (0x50000024+3)
#define UTXH1       (0x50004020+3)
#define URXH1       (0x50004024+3)
#define UTXH2       (0x50008020+3)
#define URXH2       (0x50008024+3)

#else //Little Endian
#define rUTXH0 (*(volatile unsigned char *)0x50000020) //UART 0 Transmission Hold
#define rURXH0 (*(volatile unsigned char *)0x50000024) //UART 0 Receive buffer
#define rUTXH1 (*(volatile unsigned char *)0x50004020) //UART 1 Transmission Hold
#define rURXH1 (*(volatile unsigned char *)0x50004024) //UART 1 Receive buffer
#define rUTXH2 (*(volatile unsigned char *)0x50008020) //UART 2 Transmission Hold
#define rURXH2 (*(volatile unsigned char *)0x50008024) //UART 2 Receive buffer

#define WrUTXH0(ch) (*(volatile unsigned char *)0x50000020)=(unsigned char)(ch)
#define RdURXH0()   (*(volatile unsigned char *)0x50000024)
#define WrUTXH1(ch) (*(volatile unsigned char *)0x50004020)=(unsigned char)(ch)
#define RdURXH1()   (*(volatile unsigned char *)0x50004024)
#define WrUTXH2(ch) (*(volatile unsigned char *)0x50008020)=(unsigned char)(ch)
#define RdURXH2()   (*(volatile unsigned char *)0x50008024)

#define UTXH0       (0x50000020)    //Byte_access address by DMA
#define URXH0       (0x50000024)
#define UTXH1       (0x50004020)
#define URXH1       (0x50004024)
#define UTXH2       (0x50008020)
#define URXH2       (0x50008024)
#define rUTRSTAT1   (*(volatile unsigned *)0x50004010) //UART 1 Tx/Rx status
#endif

#endif
//struct定义
#define SIZE 307200
#define R 640
#define C 480

struct RGBType{
	UINT8T r, g, b;
};
typedef struct RGBType RGBTYPE;

struct Point{
	INT16T x,y;
};
typedef struct Point POINT;

//全局变量声明
//extern UINT8T buffer[SIZE]; //存储拍摄jpeg
//extern UINT8T py[SIZE];   
//extern UINT8T pu[SIZE/4];
//extern UINT8T pv[SIZE/4];
//extern RGBTYPE image_RGB[SIZE];// color
//
//extern RGBTYPE image_Transform[SIZE];  //视角变换结果
//extern RGBTYPE image_Correction[SIZE]; //鱼眼矫正结果
//
//extern UINT8T  image_Gray[SIZE];
//extern UINT32T image_Integral[SIZE];
//extern UINT8T  image_Edge[SIZE];
//extern UINT8T  image_Gauss[SIZE];
//extern UINT8T  image_Sobel[SIZE];
//extern UINT8T	image_Canny[SIZE];
//#ifdef WIN32
//extern IplImage *image_1ch;
//#endif

//函数声明（待定义）
void delay(int );
void sys_init();
char uart_getch();
void uart_init(int nMainClk, int nBaud, int nChannel);
void uart_select(int ch);
void uart_sendbyte(int data);
void uart_printf(char *fmt,...);

#endif //__HEADER_H__
#ifdef __cplusplus
}
#endif
