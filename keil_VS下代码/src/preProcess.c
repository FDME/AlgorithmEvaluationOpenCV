#include"preProcess.h"
#include"retinex.h"
#include"basic_functions.h"
#include "test.h"
int preProcess(RGBTYPE* dst, RGBTYPE* src)
{
	UINT32T mean, variance;
	UINT8T gray[SIZE];
	
	calc_gray(gray, src);
	mean = Mean(gray, 0, 0, C, R);
	variance = Variance(gray, 0, 0, C, R, mean);
#ifdef WIN32
	printf("preProcess: mean = %d, variance = %d\n", mean, variance);
#endif
	//判断是否需要舍弃图片
	//mean越小，图片越暗
	if (mean < 50)
	{
		logStr("The image is too dark!\n");
		return FALSE;
	}
	else if (mean > 200)
	{
		logStr("The image is too bright!\n");
		return FALSE;
	}
	
	//方差越小，图片质量越不好
	if (variance < 500)
	{
		logStr("The quality of the image is too bad!\n");
		return FALSE;
	}

	//判断是否需要光线增强
	if (mean < 110)
	{
		retinex(dst, src, 30, 1);
		logStr("Finish Retinex\n");
	}
	else
		logStr("No need for Retinex\n");
		
	return TRUE;
}