#include <stdio.h>
#include <stdlib.h>
#include <opencv\cv.h>
#include "Header.h"
IplImage *image_1ch;
//Variables
UINT8T r[SIZE], g[SIZE], b[SIZE], gray[SIZE];
UINT8T canny[SIZE];

UINT8T label[SIZE];

INT32T leftBoundary, rightBoundary, upperBoundary, lowerBoundary;

float lineK[NUM_LINE], lineB[NUM_LINE];
//POINT corner[4];

UINT8T labelT[SIZE];
UINT8T resultX[SIZE], resultY[SIZE], resultW[SIZE], resultH[SIZE];

UINT8T temp0[SIZE], temp1[SIZE], temp2[SIZE], temp3[SIZE];
INT32T temp4[SIZE], temp5[SIZE];

INT32T i, j, k;

float temp0_f[SIZE * 3], temp1_f[SIZE * 3], temp2_f[SIZE * 3];//preProcess用

int main(){
#ifdef WIN32
	
	image_1ch = cvCreateImageHeader(cvSize(C, R), IPL_DEPTH_8U, 1); //需在LineDetect之前
	if (LoadImg(1) == 1) return -1;//option = 0: 摄像头采集；option = 1: 图片数组； option = 2：读jpeg文件
	showImage_RGB(r, g, b, "Original");
	if (PreProcess() == -1) return -1;
	showImage_RGB(r, g, b, "Retinex");
	showImage_1ch(gray, "gray");


#endif

}