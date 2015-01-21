#include"lineDetect.h"
#ifdef WIN32
#include<opencv\cv.h>
extern IplImage *image_1ch;
#endif
extern UINT8T  image_Gray[SIZE];

//返回直线个数，计算k,b
int lineDetect(double* k, double* b)
{
	int i, j;
	UINT32T	    	col;
	UINT32T	    	row;
	image_double image_LSD = new_image_double(C, R);
	ntuple_list detected_lines;
	int dim;
	UINT8T pGray[SIZE];
	double angletemp;
	double length;
	double temp_k, temp_b;
	int counter = 0;
#ifdef WIN32
	CvPoint start_pt;
	CvPoint end_pt;
#endif
	// LSD算法检测直线
	for (row = 1; row<(R - 1); row++)
	{
		for (col = 1; col<(C - 1); col++)
		{
			image_LSD->data[row*C + col] = image_Gray[row*C + col];//im_gray是灰度图像，没有颜色通道
		}
	}
	detected_lines = lsd(image_LSD);//detected_lines中存储提取直线的首位坐标及宽度，具体意义见说明文档
	free_image_double(image_LSD);

	// LSD结果显示
#ifdef WIN32
	memset(pGray, 0, SIZE);
	cvSetData(image_1ch, pGray, C);
#endif
	dim = detected_lines->dim;
	//printf("Number of lines detected = %d", detected_lines->size);
	//double angle[770]; //detected_lines->size = 770

	for (j = 0; j < detected_lines->size; j++)
	{
		//cvLine(res_im,start_pt,end_pt,CV_RGB(j%255,(5*j)%255,(9*j)%255),1,CV_AA);
		//��
		temp_k = (detected_lines->values[j*dim + 1] - detected_lines->values[j*dim + 3]) / (detected_lines->values[j*dim + 0] - detected_lines->values[j*dim + 2]);
		angletemp = (int)(atan(temp_k) * 180 / 3.1416);

		//检查倾角是否满足条件
		if (angletemp>20 || angletemp < -20)
			continue;
		//判断长度是否满足条件
		length = sqrt((detected_lines->values[j*dim + 2] - detected_lines->values[j*dim + 0]) * (detected_lines->values[j*dim + 2] - detected_lines->values[j*dim + 0]) + (detected_lines->values[j*dim + 3] - detected_lines->values[j*dim + 1])*(detected_lines->values[j*dim + 3] - detected_lines->values[j*dim + 1]));
		if (length < (double)C / 8)
			continue;
		//printf("j = %d, length = %f\n", j, length);

		//对满足两个条件的线段，求直线方程
		k[counter] = temp_k;
		temp_b = detected_lines->values[j*dim + 1] - temp_k * detected_lines->values[j*dim + 0];
		b[counter] = temp_b;
		counter++;

#ifdef WIN32
		start_pt = cvPoint((int)detected_lines->values[j*dim + 0], (int)detected_lines->values[j*dim + 1]);
		end_pt = cvPoint((int)detected_lines->values[j*dim + 2], (int)detected_lines->values[j*dim + 3]);
		cvLine(image_1ch, start_pt, end_pt, CV_RGB(0, 0, 255), 1, CV_AA, 0);
#endif
	}
#ifdef WIN32
	cvNamedWindow("LSD", 0);
	cvShowImage("LSD", image_1ch);
	cvWaitKey(0);
#endif

	return counter;
}