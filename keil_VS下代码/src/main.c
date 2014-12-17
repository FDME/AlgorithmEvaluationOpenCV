/*********************************************************************************************
* File：	main.c
* Author:	embest
* Desc：	c main entry
* History:	
*********************************************************************************************/

/*------------------------------------------------------------------------------------------*/
/*                                     includes files	                                    */
/*------------------------------------------------------------------------------------------*/
#include "header.h"
#include "lsd.h"
#include "ForegroundSeperation.h"
#include "LineSort.h"
#include "PerspectiveTransform.h"
#include "ImageSegmentation.h"
#include "LabelOptimization.h"
#include "undistort.h"
#include"basic_functions.h"
#include"lineDetect.h"
#include"canny.h"
#include"preProcess.h"
#include"test.h"
#include"LoadImg.h"

RGBTYPE image_RGB[SIZE];// color

RGBTYPE image_Transform[SIZE];  //视角变换结果
RGBTYPE image_Correction[SIZE]; //鱼眼矫正结果
RGBTYPE image_Preprocess[SIZE]; //暗光增强结果
UINT8T  image_Gray[SIZE];
UINT32T image_Integral[SIZE];
UINT8T  image_Edge[SIZE];
UINT8T  image_Gauss[SIZE];
UINT8T  image_Sobel[SIZE];
UINT8T	image_Canny[SIZE];
UINT8T	label[SIZE];
POINT up_st,up_en,down_st,down_en;  //上边界起始点，下边界起始点。目前仅上边界有效
UINT16T right;//右边界坐标值（垂直）

//检测到的直线参数，个数待优化

double Line_k[2000]; 
double Line_b[2000];
INT32T numLines;
#ifdef WIN32
IplImage *image_1ch;
#endif
/*********************************************************************************************
* name:		main
*********************************************************************************************/
//void lens_correct(const UINT8T* src, UINT8T* dst);  //鱼眼矫正
int main(int argc,char **argv)
{
		UINT32T k = 0;
		
		int i, j, number;
#ifdef WIN32
		image_1ch = cvCreateImageHeader(cvSize(C, R), IPL_DEPTH_8U, 1); //需在LineDetect之前
#endif
		if (LoadImg(image_RGB, 2) == FALSE) return -1;//option = 0: 摄像头采集；option = 1: 图片数组； option = 2：读jpeg文件
		undistort_rgb(image_RGB, image_Correction);
		if (preProcess(image_Correction, image_Correction) == FALSE) return -1;
		calc_gray(image_Gray, image_Correction);
		if(canny(image_Canny,image_Gray, 1)== FALSE) return -1;//option设置错误
		//ImageSegment(label);
		//LabelOptimize(label);
		//numLines = lineDetect(Line_k,Line_b);
		//numLines = LineSort(numLines, Line_k, Line_b);
		ForegroundSeperation(image_Correction,image_Correction);

		//**********PerspectiveTransform测试代码如下：************
		if (1){
			POINT src[4], dst[4];
			src[0].x = 0; src[0].y = 0;
			src[1].x = 0; src[1].y = 200;
			src[2].x = 150; src[2].y = 200;
			src[3].x = 150; src[3].y = 0;

			dst[0].x = 0; dst[0].y = 0;
			dst[1].x = 0; dst[1].y = C;
			dst[2].x = R; dst[2].y = C;
			dst[3].x = R; dst[3].y = 0;
			//no in-place 输入输出不能相同
			PerspectiveTransform(src, dst, image_Correction,image_Transform);
					}
		//*********************************************************

//检验结果
#ifdef WIN32
		//输出为图片
		showImage_RGB(image_RGB,"Original");
		
		//输出鱼眼矫正结果
		showImage_RGB(image_Correction, "Correction");
		//showImage_RGB(image_Preprocess, "preprocess");
		
		showImage_RGB(image_Transform,"Transform");
		//showImage_1ch(image_Gray, "Gray");
		showImage_1ch(image_Canny, "CANNY");
		//UINT8T pGray[SIZE];
		//for (i = 0; i < R; i++)  //int�������������ѭ��
			//for (j = 0; j < C; j++)
			////pGray[i*C + j] = image_Gray[i*C + j];
				//pGray[i*C + j] = (UINT8T)((image_Integral[i*C + j])/SIZE);  
		//showImage_1ch(pGray, "integral");


#else
		/*	//print the result of decoding
		k = 0;
		while(k<150000){
		uart_sendbyte(py[k]);
		k++;
		}
		while(k<Y_SIZE){
		uart_sendbyte(py[k]);
		k++;
		}
		*/
		while(1){}
#endif

}
