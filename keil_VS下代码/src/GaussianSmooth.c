#include "GaussianSmooth.h"

void GaussianSmooth(double* dst, double* src, double sigma, int channels)
{
	int ksize;
	double tempData[SIZE * 3];
	double result[SIZE * 3];
	sigma = sigma > 0 ? sigma : -sigma;
	//高斯核矩阵的大小为(6*sigma+1)*(6*sigma+1)
	//ksize为奇数
	ksize = ceil(sigma * 3) * 2 + 1;

	//计算一维高斯核
	//sigma < 50
	double kernel[301];

	double scale = -0.5 / (sigma*sigma);
	double cons = 1 / sqrt(-scale / PI);

	double sum = 0;
	int kcenter = ksize / 2;
	int i = 0, j = 0;
	for (i = 0; i < ksize; i++)
	{
		int x = i - kcenter;
		*(kernel + i) = cons * exp(x * x * scale);//一维高斯函数
		sum += *(kernel + i);
	}

	//归一化,确保高斯权值在[0,1]之间
	for (i = 0; i < ksize; i++)
	{
		*(kernel + i) /= sum;
	}

	//x方向一维高斯模糊
	for (int y = 0; y < R; y++)
	{
		for (int x = 0; x < C; x++)
		{
			double mul = 0;
			sum = 0;
			double bmul = 0, gmul = 0, rmul = 0;
			for (i = -kcenter; i <= kcenter; i++)
			{
				if ((x + i) >= 0 && (x + i) < C)
				{
					if (channels == 1)
					{
						mul += *(src + y * C + (x + i))*(*(kernel + kcenter + i));
					}
					else
					{
						bmul += *(src + y*C *channels + (x + i)*channels + 0)*(*(kernel + kcenter + i));
						gmul += *(src + y*C *channels + (x + i)*channels + 1)*(*(kernel + kcenter + i));
						rmul += *(src + y*C *channels + (x + i)*channels + 2)*(*(kernel + kcenter + i));
					}
					sum += (*(kernel + kcenter + i));
				}
			}
			if (channels == 1)
			{
				*(tempData + y*C + x) = mul / sum;
			}
			else
			{
				*(tempData + y*C*channels + x*channels + 0) = bmul / sum;
				*(tempData + y*C*channels + x*channels + 1) = gmul / sum;
				*(tempData + y*C*channels + x*channels + 2) = rmul / sum;
			}
		}
	}


	//y方向一维高斯模糊
	for (int x = 0; x < C; x++)
	{
		for (int y = 0; y < R; y++)
		{
			double mul = 0;
			sum = 0;
			double bmul = 0, gmul = 0, rmul = 0;
			for (i = -kcenter; i <= kcenter; i++)
			{
				if ((y + i) >= 0 && (y + i) < R)
				{
					if (channels == 1)
					{
						mul += *(tempData + (y + i)*C + x)*(*(kernel + kcenter + i));
					}
					else
					{
						bmul += *(tempData + (y + i)*C*channels + x*channels + 0)*(*(kernel + kcenter + i));
						gmul += *(tempData + (y + i)*C*channels + x*channels + 1)*(*(kernel + kcenter + i));
						rmul += *(tempData + (y + i)*C*channels + x*channels + 2)*(*(kernel + kcenter + i));
					}
					sum += (*(kernel + kcenter + i));
				}
			}
			if (channels == 1)
			{
				*(result + y*C + x) = mul / sum;
			}
			else
			{
				*(result + y*C*channels + x*channels + 0) = bmul / sum;
				*(result + y*C*channels + x*channels + 1) = gmul / sum;
				*(result + y*C*channels + x*channels + 2) = rmul / sum;
			}

		}
	}

	//复制结果
	for (i = 0; i < R; i++)
	for (j = 0; j < C; j++)
	for (int k = 0; k < 3; k++)
	{
		dst[i*C*channels + j*channels+k] = result[i*C*channels + j*channels+k];
	}
}