/*********************************************************************************************
* File��	main.c
* Author:	embest
* Desc��	c main entry
* History:	
*********************************************************************************************/

/*------------------------------------------------------------------------------------------*/
/*                                     includes files	                                    */
/*------------------------------------------------------------------------------------------*/
#include "header.h"
#include "lsd.h"
#include "ForegroundSeperation.h"

UINT8T buffer[SIZE]; //�洢����jpeg
UINT8T py[SIZE];   
UINT8T pu[SIZE/4];
UINT8T pv[SIZE/4];
RGBTYPE image_RGB[SIZE];// color

RGBTYPE image_Transform[SIZE];  //�ӽǱ任���
RGBTYPE image_Correction[SIZE]; //���۽������

UINT8T  image_Gray[SIZE];
UINT32T image_Integral[SIZE];
UINT8T  image_Edge[SIZE];
UINT8T  image_Gauss[SIZE];
UINT8T  image_Sobel[SIZE];
UINT8T	image_Canny[SIZE];





//��⵽��ֱ�߲������������Ż�
double Line_k[1000]; 
double Line_b[1000];

#ifdef WIN32
IplImage *image_1ch;
#endif
/*********************************************************************************************
* name:		main
*********************************************************************************************/
//void lens_correct(const UINT8T* src, UINT8T* dst);  //���۽���
int main(int argc,char **argv)
{
		UINT8T* picture = buffer;
		UINT32T k = 0;
		UINT32T jpg_size;
		int i, j, number;

#ifdef WIN32
		FILE* fp;
		image_1ch = cvCreateImageHeader(cvSize(C, R), IPL_DEPTH_8U, 1);


                //fp = fopen("C:\\projects\\huawei\\image\\����ͼƬ\\��Ϊ����-20141128\\����A--���߸���\\jpeg_20141128_151936.jpg", "rb");  // ����
                //fp = fopen("C:\\projects\\huawei\\image\\����ͼƬ\\image_undistort.jpg", "rb");  // ���۽�����


		fp = fopen("C:\\Users\\ZoeQIAN\\Pictures\\��Ϊ����-20141128\\����A������������\\5.jpg", "rb");
		jpg_size = filesize(fp);
		printf("size=%d\n", jpg_size);
		fread(buffer, jpg_size, 1, fp);
		fclose(fp);
		printf("����ͼƬ�ɹ���\n");
		
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
		
		undistort_map(image_RGB,image_Correction);
		calc_gray(image_Gray, image_Correction);
		//calc_integral(image_Integral, image_Gray);
		//calc_gaussian_5x5(image_Gauss, image_Gray);
		//calc_sobel_3x3(image_Sobel, image_Gray);
		canny(image_Canny,image_Gray);
		number = lineDetect(Line_k,Line_b);
//������
#ifdef WIN32
		//���ΪͼƬ
		showImage_RGB(image_RGB,"Original");
		
		//������۽������
		showImage_RGB(image_Correction, "Correction");
		//��ˮ��ǰ��������
		ForegroundSeperation(image_Correction);


		showImage_1ch(image_Gray, "Gray");
		showImage_1ch(image_Canny, "canny");


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
