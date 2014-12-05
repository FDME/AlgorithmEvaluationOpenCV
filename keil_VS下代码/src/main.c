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
double cam[] = { 283.561, 0, 246, 0, 285.903, 334.103, 0, 0, 1 };
double dis[] = { -0.313793, 0.122695, 0.00123624, -0.000849487, -0.0250905 };

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
		int i, j;

	UINT32T	    	col;
	UINT32T	    	row; 
			
// LSD�㷨���ֱ��
image_double image_LSD = new_image_double(C, R);
ntuple_list detected_lines;
int dim;
CvPoint start_pt;
CvPoint end_pt;
	
#ifdef WIN32
		FILE* fp;

                //fp = fopen("C:\\projects\\huawei\\image\\����ͼƬ\\��Ϊ����-20141128\\����A--���߸���\\jpeg_20141128_151936.jpg", "rb");  // ����
                //fp = fopen("C:\\projects\\huawei\\image\\����ͼƬ\\image_undistort.jpg", "rb");  // ���۽�����


		fp = fopen("..\\..\\60_4.jpg", "rb");
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
		calc_gray(image_Gray, image_RGB);
		undistort_map();
		//calc_integral(image_Integral, image_Gray);
		//calc_gaussian_5x5(image_Gauss, image_Gray);
		//calc_sobel_3x3(image_Sobel, image_Gray);
		//canny();
		
		
		//memset(pRGB, 0, SIZE * 3); //��ʼ��
		//for (i = 0; i < R; i++)  //int�������������ѭ��
		//{
		//	for (j = 0; j < C; j++)
		//	{
		//		pRGB[i*C * 3 + j * 3] = image_RGB[i*C + j].b;
		//		pRGB[i*C * 3 + j * 3 + 1] = image_RGB[i*C + j].g;
		//		pRGB[i*C * 3 + j * 3 + 2] = image_RGB[i*C + j].r;
		//	}
		//}


		//���۽���
		//UINT8T* dst;
		//UINT8T lens_result[SIZE * 3];
		//dst = lens_result;
		//lens_correct(pRGB, dst);
		//ת�Ҷȣ�����ͼ����˹�˲���canny
		//calc_gray(image_Gray, image_RGB);
		//calc_integral(image_Integral, image_Gray);
		//calc_gaussian_5x5(image_Gauss, image_Gray); //�ɿ�����canny�е����Խ�ʡ�ռ�
		//calc_sobel_3x3(image_Sobel, image_Gray);
		//canny();

  // LSD�㷨���ֱ�ߣ�Ҫ���Ƕ��㻯����
	for (row=1;row<(R-1);row++)
  {
    for (col=1;col<(C-1);col++)
    {  
      image_LSD->data[row*C+col] = image_Gray[row*C+col];//im_gray�ǻҶ�ͼ��û����ɫͨ��
    }
  }
    detected_lines = lsd(image_LSD);//detected_lines�д洢��ȡֱ�ߵ���λ���꼰��ȣ����������˵���ĵ�
	free_image_double(image_LSD);

		
		
//������
#ifdef WIN32
		//���ΪͼƬ
		showImage_RGB(image_RGB,"Original");
		
		//������۽������
		showImage_RGB(image_Correction, "Correction");

        // ��ʾ��ͨ������
		UINT8T pGray[SIZE];    //������ʾ��ͨ��ͼƬ
		memset(pGray, 0, SIZE); //��ʼ��
		for (i = 0; i < R; i++)  //int�������������ѭ��
		{
			for (j = 0; j < C; j++)
			{
				pGray[i*C + j] = image_Gray[i*C + j];
				//pGray[i*C + j] = (UINT8T)((image_Integral[i*C + j])/SIZE);  // show integral
				//pGray[i*C + j] = (UINT8T)(image_Gauss[i*C + j]); 
				//pGray[i*C + j] = (UINT8T)(image_Sobel[i*C + j]); 
				//pGray[i*C + j] = (UINT8T)(image_Erzhi[i*C + j]);
			}
		}
		image_1ch = cvCreateImageHeader(cvSize(C, R), IPL_DEPTH_8U, 1);
		cvSetData(image_1ch, pGray, C);

		// LSD�㷨���ֱ��,��������ֱ�߻���������ĻҶ�ͼ����,�Ҷ�ͼע����
		dim = detected_lines->dim;
		for (j = 0; j < detected_lines->size; j++)
		{
			start_pt = cvPoint((int)detected_lines->values[j*dim + 0], (int)detected_lines->values[j*dim + 1]);
			end_pt = cvPoint((int)detected_lines->values[j*dim + 2], (int)detected_lines->values[j*dim + 3]);
			//cvLine(res_im,start_pt,end_pt,CV_RGB(j%255,(5*j)%255,(9*j)%255),1,CV_AA);
			cvLine(image_1ch, start_pt, end_pt, CV_RGB(0, 0, 255), 1, CV_AA, 0);
		}
		cvNamedWindow("LSD", 0);
		cvShowImage("LSD", image_1ch);
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
