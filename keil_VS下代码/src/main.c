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

UINT8T buffer[SIZE]; //存储拍摄jpeg
UINT8T py[SIZE];   
UINT8T pu[SIZE/4];
UINT8T pv[SIZE/4];
RGBTYPE image_RGB[SIZE];// color

RGBTYPE image_Transform[SIZE];  //视角变换结果
RGBTYPE image_Correction[SIZE]; //鱼眼矫正结果

UINT8T  image_Gray[SIZE];
UINT32T image_Integral[SIZE];
UINT8T  image_Edge[SIZE];
UINT8T  image_Gauss[SIZE];
UINT8T  image_Sobel[SIZE];
UINT8T	image_Canny[SIZE];
double cam[] = { 283.561, 0, 246, 0, 285.903, 334.103, 0, 0, 1 };
double dis[] = { -0.313793, 0.122695, 0.00123624, -0.000849487, -0.0250905 };

#ifdef WIN32
IplImage *image_1ch;
UINT8T pGray[SIZE];    //用于显示单通道图片
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
		int i, j;
	
#ifdef WIN32
		FILE* fp;

                //fp = fopen("C:\\projects\\huawei\\image\\测试图片\\华为拍照-20141128\\机柜A--电线干扰\\jpeg_20141128_151936.jpg", "rb");  // 可用
                //fp = fopen("C:\\projects\\huawei\\image\\测试图片\\image_undistort.jpg", "rb");  // 鱼眼矫正后

		fp = fopen("..\\..\\new.jpg", "rb");
		//fp = fopen("..\\..\\jpeg_20141128_150700.jpg", "rb");
		jpg_size = filesize(fp);
		printf("size=%d\n", jpg_size);
		fread(buffer, jpg_size, 1, fp);
		fclose(fp);
		printf("读入图片成功！\n");
		image_1ch = cvCreateImageHeader(cvSize(C, R), IPL_DEPTH_8U, 1);
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
		
		undistort_map();
		calc_gray(image_Gray, image_Correction);
		//calc_integral(image_Integral, image_Gray);
		calc_gaussian_5x5(image_Gauss, image_Gray);
		//calc_sobel_3x3(image_Sobel, image_Gray);
		canny();
		
		
		//memset(pRGB, 0, SIZE * 3); //初始化
		//for (i = 0; i < R; i++)  //int不够，必须二重循环
		//{
		//	for (j = 0; j < C; j++)
		//	{
		//		pRGB[i*C * 3 + j * 3] = image_RGB[i*C + j].b;
		//		pRGB[i*C * 3 + j * 3 + 1] = image_RGB[i*C + j].g;
		//		pRGB[i*C * 3 + j * 3 + 2] = image_RGB[i*C + j].r;
		//	}
		//}


		//鱼眼矫正
		//UINT8T* dst;
		//UINT8T lens_result[SIZE * 3];
		//dst = lens_result;
		//lens_correct(pRGB, dst);
		//转灰度，积分图，高斯滤波和canny
		//calc_gray(image_Gray, image_RGB);
		//calc_integral(image_Integral, image_Gray);
		//calc_gaussian_5x5(image_Gauss, image_Gray); //可考虑在canny中调用以节省空间
		//calc_sobel_3x3(image_Sobel, image_Gray);
		//canny();

		lineDetect();

		
		
//检验结果
#ifdef WIN32
		//输出为图片
		showImage_RGB(image_RGB,"Original");
		
		//输出鱼眼矫正结果
		showImage_RGB(image_Correction, "Correction");

        // 显示单通道数据
		
		memset(pGray, 0, SIZE); //初始化
		for (i = 0; i < R; i++)  //int不够，必须二重循环
		{
			for (j = 0; j < C; j++)
			{
				//pGray[i*C + j] = image_Gray[i*C + j];
				//pGray[i*C + j] = (UINT8T)((image_Integral[i*C + j])/SIZE);  // show integral
				//pGray[i*C + j] = (UINT8T)(image_Gauss[i*C + j]); 
				//pGray[i*C + j] = (UINT8T)(image_Sobel[i*C + j]); 
				//pGray[i*C + j] = (UINT8T)(image_Erzhi[i*C + j]);
			}
		}
		
		cvSetData(image_1ch, image_Canny, C);
		cvNamedWindow("canny", 0);
		cvShowImage("canny", image_1ch);
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
