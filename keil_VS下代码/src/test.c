#include"header.h"
#ifdef WIN32
extern IplImage* image_1ch;
int filesize(FILE *fp)
{
	long pos;
	fseek(fp, 0, SEEK_END);
	pos = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	return pos;
}

//void showImage(RGBTYPE* image_RGB)
//{
//	UINT8T pRGB[SIZE * 3]; //��������������reserved stack size
//	memset(pRGB, 0, SIZE * 3); //��ʼ��
//	IplImage *image;
//
//	for (int i = 0; i < R; i++)  //int�������������ѭ��
//	{
//		for (int j = 0; j < C; j++)
//		{
//			pRGB[i*C * 3 + j * 3] = image_RGB[i*C + j].b;
//			pRGB[i*C * 3 + j * 3 + 1] = image_RGB[i*C + j].g;
//			pRGB[i*C * 3 + j * 3 + 2] = image_RGB[i*C + j].r;
//		}
//	}
//
//	image = cvCreateImageHeader(cvSize(C, R), IPL_DEPTH_8U, 3);
//	cvSetData(image, pRGB, C * 3);
//	cvNamedWindow("1");
//	cvShowImage("1", image);
//	cvWaitKey();
//}


#endif