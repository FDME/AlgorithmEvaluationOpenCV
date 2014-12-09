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
//struct����
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
	UINT16T x,y;
};
typedef struct Point POINT;
struct ws_Queue{
	UINT32T pos[SIZE];
	UINT32T st,en;
};
typedef struct ws_Queue WSQ;
//ȫ�ֱ�������
extern UINT8T buffer[SIZE]; //�洢����jpeg
extern UINT8T py[SIZE];   
extern UINT8T pu[SIZE/4];
extern UINT8T pv[SIZE/4];
extern RGBTYPE image_RGB[SIZE];// color

extern RGBTYPE image_Transform[SIZE];  //�ӽǱ任���
extern RGBTYPE image_Correction[SIZE]; //���۽������

extern UINT8T  image_Gray[SIZE];
extern UINT32T image_Integral[SIZE];
extern UINT8T  image_Edge[SIZE];
extern UINT8T  image_Gauss[SIZE];
extern UINT8T  image_Sobel[SIZE];
extern UINT8T	image_Canny[SIZE];
extern WSQ q[256];

#ifdef WIN32
extern IplImage *image_1ch;
#endif

//��������
#ifdef WIN32
//windows�º�������
void showImage_RGB(RGBTYPE* image_RGB, const char* name);
void showImage_1ch(UINT8T* pGray, const char* name);
int filesize(FILE *fp);

#else
//ARM�º�������
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
void ForegroundSeperation();
void canny(UINT8T* dst, UINT8T* src);
int lineDetect(double* k, double* b);
#endif //__HEADER_H__
#ifdef __cplusplus
}
#endif

