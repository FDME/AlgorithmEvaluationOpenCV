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

UINT8T buffer[SIZE]; //存储拍摄jpeg
UINT8T py[SIZE];   
UINT8T pu[SIZE/4];
UINT8T pv[SIZE/4];
RGBTYPE image_RGB[SIZE];// color
RGBTYPE image_Transform[SIZE];
RGBTYPE image_undis[SIZE];
UINT8T  image_Gray[SIZE];
UINT32T image_Integral[SIZE];
UINT8T  image_Edge[SIZE];
UINT8T  image_Gauss[SIZE];
UINT8T  image_Sobel[SIZE];
UINT8T	image_Canny[SIZE];
double cam[] = { 283.561, 0, 246, 0, 285.903, 334.103, 0, 0, 1 };
double dis[] = { -0.313793, 0.122695, 0.00123624, -0.000849487, -0.0250905 };
/*********************************************************************************************
* name:		main
*********************************************************************************************/

int main(int argc,char **argv)
{
		UINT8T* picture = buffer;
		UINT32T k = 0;
		UINT32T jpg_size;
		int i, j;

#ifdef WIN32
		FILE* fp;
		IplImage *image;
		IplImage *image_1ch;
		UINT8T pRGB[SIZE * 3]; //数组个数过大，需改reserved stack size
		UINT8T pGray[SIZE];    //用于显示单通道图片



		fp = fopen("..\\..\\60_4.jpg", "rb");
		jpg_size = filesize(fp);
		printf("size=%d\n", jpg_size);
		fread(buffer, jpg_size, 1, fp);
		fclose(fp);
		printf("读入图片成功！\n");
		
#else
		sys_init();        /* Initial s3c2410's Clock, MMU, Interrupt,Port and UART */
		uart_select(UART0);
		uart_printf("Take a picture\n");
		jpg_size = camera();
		uart_select(UART0);
		uart_printf("Finish transmitting\n");
#endif
	/*	while(k<size){
			uart_sendbyte(buffer[k]);
			k++;
		}
		*/
		if(jpg_decode(picture,py,pu,pv,jpg_size)<0)
#ifdef WIN32
			printf("Decoding error!\n");
#else
			uart_printf("Decoding error!\n");
#endif
		else
#ifdef WIN32
			printf("Finish decoding\n");
#else
			uart_printf("Finish decoding\n"); 
#endif
		
		if(yuv2rgb(py, pu, pv, image_RGB) == FALSE)
			
#ifdef WIN32
			printf("RGB error!\n");
#else
			uart_printf("RGB error!\n");
#endif
		else
#ifdef WIN32
			printf("Finish RGB\n"); 
#else
			uart_printf("Finish RGB\n"); 
#endif
		calc_gray(image_Gray, image_RGB);
		calc_integral(image_Integral, image_Gray);
		calc_gaussian_5x5(image_Gauss, image_Gray);
		calc_sobel_3x3(image_Sobel, image_Gray);
		//canny();
//检验结果
#ifdef WIN32
		//输出为图片
		//showImage(image_RGB);
		
		memset(pRGB, 0, SIZE * 3); //初始化
		for (i = 0; i < R; i++)  //int不够，必须二重循环
		{
			for (j = 0; j < C; j++)
			{
				pRGB[i*C * 3 + j * 3] = image_RGB[i*C + j].b;
				pRGB[i*C * 3 + j * 3 + 1] = image_RGB[i*C + j].g;
				pRGB[i*C * 3 + j * 3 + 2] = image_RGB[i*C + j].r;
			}
		}
		image = cvCreateImageHeader(cvSize(C, R), IPL_DEPTH_8U, 3);
		cvSetData(image, pRGB, C * 3);
		cvNamedWindow("rgb",1);
		cvShowImage("rgb", image);
		cvWaitKey(0);

		// 显示单通道数据
		
		memset(pGray, 0, SIZE); //初始化
		//for (i = 0; i < R; i++)  //int不够，必须二重循环
		//{
		//	for (j = 0; j < C; j++)
		//	{
		//		//pGray[i*C + j] = image_Gray[i*C + j];
		//		//pGray[i*C + j] = (UINT8T)((image_Integral[i*C + j])/SIZE);  // show integral
		//		//pGray[i*C + j] = (UINT8T)(image_Gauss[i*C + j]); 
		//		//pGray[i*C + j] = (UINT8T)(image_Sobel[i*C + j]);
		//	}
		//}
		//cvSetData(image_1ch, pGray, C);
		image_1ch = cvCreateImageHeader(cvSize(C, R), IPL_DEPTH_8U, 1);
		cvSetData(image_1ch, image_Canny, C);
		cvNamedWindow("pGray",1);
		cvShowImage("pGray", image_1ch);
		cvWaitKey(0);
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
