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
#include"yuv2rgb.h"
#include"decoder\jpg_decode.h"
#include"lineDetect.h"
#include"canny.h"
#include"preProcess.h"
#include"test.h"

#ifdef WIN32
#else
#include"camera_control.h"
#endif

UINT8T buffer[SIZE]; //存储拍摄jpeg
UINT8T py[SIZE];   
UINT8T pu[SIZE/4];
UINT8T pv[SIZE/4];
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
		UINT8T* picture = buffer;
		UINT32T k = 0;
		UINT32T jpg_size;
		int i, j, number;

#ifdef WIN32
		FILE* fp;

		//fp = fopen("C:\\HuaWeiImage\\华为拍照-20141128\\机柜A——正常光照\\jpeg_20141128_150639.jpg", "rb");
		fp = fopen("C:\\Users\\ZoeQIAN\\Pictures\\华为拍照-20141128\\机柜A——正常光照\\5.jpg","rb");
		//fp = fopen("E:\\VS2013_pro\\jpeg_20141128_151235.jpg", "rb");  //暗光图片
		jpg_size = filesize(fp);
		printf("size=%d\n", jpg_size);
		fread(buffer, jpg_size, 1, fp);
		fclose(fp);
		printf("读入图片成功！\n");
		
#else
		sys_init();        /* Initial s3c2410's Clock, MMU, Interrupt,Port and UART */
		uart_select(UART0);
		uart_printf("Take a picture\n");
		jpg_size = camera(buffer);
		uart_select(UART0);
		uart_printf("Finish transmitting\n");
#endif
		if(jpg_decode(picture,py,pu,pv,jpg_size)<0)
			logStr("Decoding error!\n");
		else
			logStr("Finish decoding\n");
		
		if(yuv2rgb(py, pu, pv, image_RGB) == FALSE)
			logStr("RGB error!\n");
		else
			logStr("Finish RGB\n"); 

		undistort_map(image_RGB, image_Correction);
		preProcess(image_Preprocess, image_Correction);
		calc_gray(image_Gray, image_Correction);
		canny(image_Canny,image_Gray);
		//ImageSegment(label);
		//LabelOptimize(label);
		//numLines = lineDetect(Line_k,Line_b);
		//numLines = LineSort(numLines, Line_k, Line_b);
		ForegroundSeperation(image_Preprocess,image_Preprocess);

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
		showImage_RGB(image_Preprocess, "preprocess");
		
		showImage_RGB(image_Transform,"Transform");
		//showImage_1ch(image_Gray, "Gray");

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
