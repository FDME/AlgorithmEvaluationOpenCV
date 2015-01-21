#include"retinex.h"
#include"basic_functions.h"
#include "test.h"

/*input: r[SIZE], g[SIZE], b[SIZE] //矫正后的RGB信息，gray
output: r[SIZE], g[SIZE], b[SIZE], gray[SIZE] //预处理后的灰度图，以及RGB
return: int. -1: 照片光照或质量不佳，需重新拍摄。0: 照片可用，已进行判断和预处理。
usege : 预处理，亮度调节等*/
//内部临时变量：float temp0_f[SIZE * 3], temp1_f[SIZE * 3](声明在retinex.c), temp2_f[SIZE * 3](声明在GaussianSmooth.h);
extern UINT8T r[SIZE], g[SIZE], b[SIZE], gray[SIZE];
int PreProcess()
{
	UINT32T mean, variance;

	//得到校正后的灰度图
	calc_gray(); // 输入r,g,b；输出gray
	//计算灰度图的均值与方差
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
		return -1;
	}
	else if (mean > 200)
	{
		logStr("The image is too bright!\n");
		return -1;
	}

	//方差越小，图片质量越不好
	if (variance < 500)
	{
		logStr("The quality of the image is too bad!\n");
		return -1;
	}

	//判断是否需要光线增强
	if (mean < 110)
	{
		retinex(r,g, b, gray, 30, 1);
		logStr("Finish Retinex\n");
	}
	else
		logStr("No need for Retinex\n");

	return 0;
}