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
RGBTYPE image_RGB[SIZE];

//测试所用图片RGB信息
extern UINT8T imageOringinal_R[R][C];
extern UINT8T imageOringinal_G[R][C];
extern UINT8T imageOringinal_B[R][C];
/*********************************************************************************************
* name:		main
*********************************************************************************************/

int main(int argc,char **argv)
{
		UINT8T* picture = buffer;
		UINT32T k = 0;
		UINT16T jpg_size;
#ifdef WIN32
		FILE* fp;
		fp = fopen("..\\..\\60_4.jpg", "rb");
		jpg_size = filesize(fp);
		printf("size=%d\n", jpg_size);
		UINT8T temp;
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

//检验结果
#ifdef WIN32
		//输出为图片
		//showImage(image_RGB);
		UINT8T pRGB[SIZE * 3]; //数组个数过大，需改reserved stack size
		memset(pRGB, 0, SIZE * 3); //初始化
		IplImage *image;

		for (int i = 0; i < R; i++)  //int不够，必须二重循环
		{
			for (int j = 0; j < C; j++)
			{
				pRGB[i*C * 3 + j * 3] = image_RGB[i*C + j].b;
				pRGB[i*C * 3 + j * 3 + 1] = image_RGB[i*C + j].g;
				pRGB[i*C * 3 + j * 3 + 2] = image_RGB[i*C + j].r;
			}
		}

		image = cvCreateImageHeader(cvSize(C, R), IPL_DEPTH_8U, 3);
		cvSetData(image, pRGB, C * 3);
		cvNamedWindow("1");
		cvShowImage("1", image);
		cvWaitKey();
		
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
