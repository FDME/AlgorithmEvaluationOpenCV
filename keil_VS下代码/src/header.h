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

//#define min(x1,x2) (((x1)<(x2))? (x1):(x2))
//#define max(x1,x2) (((x1)>(x2))? (x1):(x2))

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
#include "2410lib.h"
#endif
//struct定义
#ifndef __HEADER_H__
#define __HEADER_H__

#define SIZE 307200
#define R 640
#define C 480

struct RGBType{
	UINT8T r, g, b;
};
typedef struct RGBType RGBTYPE;

struct Point{
	UINT8T x,y;
};
typedef struct Point POINT;

//函数声明
#ifdef WIN32
//windows下函数声明
void showImage_RGB(RGBTYPE* image_RGB, const char* name);
int filesize(FILE *fp);

#else
//ARM下函数声明
UINT16T camera(void);
#endif
int yuv2rgb(UINT8T* py, UINT8T* pu, UINT8T* pv, RGBTYPE* image_RGB);
int jpg_decode(UINT8T* iPicture, UINT8T* oY, UINT8T* oU, UINT8T* oV, UINT32T size);
void undistort_map();
RGBTYPE pix_add(RGBTYPE a, RGBTYPE b);
RGBTYPE pix_div(RGBTYPE a, int n);
RGBTYPE pix_mul(RGBTYPE a, float t);
RGBTYPE pix_inter(RGBTYPE t1, RGBTYPE t2, float t);
void calc_gray(UINT8T* Gray, RGBTYPE* RGB);
void calc_integral(UINT32T* integral, UINT8T* image);
void calc_gaussian_5x5(UINT8T* dst, UINT8T* src);
void calc_sobel_3x3(UINT8T* dst, UINT8T* src);
#endif //__HEADER_H__