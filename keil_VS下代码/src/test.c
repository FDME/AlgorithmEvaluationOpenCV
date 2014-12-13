#include"test.h"
#ifdef WIN32
extern IplImage *image_1ch;
int filesize(FILE *fp)
{
	long pos;
	fseek(fp, 0, SEEK_END);
	pos = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	return pos;
}

void showImage_RGB(RGBTYPE* image_RGB, const char* name)
{
	UINT8T pRGB[SIZE * 3]; //数组个数过大，需改reserved stack size
	IplImage *image;
	int i,j;
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
	cvNamedWindow(name,0);
	cvShowImage(name, image);
	cvWaitKey(0);
}

void showImage_1ch(UINT8T* p_1ch, const char* name)
{
	// 显示单通道数据

	cvSetData(image_1ch, p_1ch, C);
	cvNamedWindow(name, 0);
	cvShowImage(name, image_1ch);
	cvWaitKey(0);
}
#endif

void logStr(const char* str)
{
#ifdef WIN32
	printf(str);
#else
	uart_printf(str);
#endif
}