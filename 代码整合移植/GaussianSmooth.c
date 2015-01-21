#include "GaussianSmooth.h"

extern float temp2_f[SIZE * 3];
void GaussianSmooth(float* dst, float* src, int sigma, int channels)
{
	int ksize;
	float* tempData;

	//����һά��˹��
	//sigma < 50
	float kernel[301];
	
	float mul = 0;
	float bmul = 0, gmul = 0, rmul = 0;

	float scale = -0.5 / (sigma*sigma);
	float cons = 1 / sqrt(-scale / PI);

	float sum = 0;
	int kcenter;
	int i = 0, j = 0,x,y;

	tempData = temp2_f;
	//tempData = (float*)malloc(sizeof(float)*SIZE*3);
	//result = (float*)malloc(sizeof(float)*SIZE*3);
	sigma = sigma > 0 ? sigma : -sigma;
	//��˹�˾���Ĵ�СΪ(6*sigma+1)*(6*sigma+1)
	//ksizeΪ����
	ksize = ceil(sigma * 3) * 2 + 1;
	kcenter = ksize / 2;


	for (i = 0; i < ksize; i++)
	{
		x = i - kcenter;
		*(kernel + i) = cons * exp(x * x * scale);//һά��˹����
		sum += *(kernel + i);
	}

	//��һ��,ȷ����˹Ȩֵ��[0,1]֮��
	for (i = 0; i < ksize; i++)
	{
		*(kernel + i) /= sum;
	}

	//x����һά��˹ģ��
	for (y = 0; y < R; y++)
	{
		for (x = 0; x < C; x++)
		{
			mul = 0;
			bmul = 0, gmul = 0, rmul = 0;
			sum = 0;
			
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


	//y����һά��˹ģ��
	for (x = 0; x < C; x++)
	{
		for (y = 0; y < R; y++)
		{
			mul = 0;
			bmul = 0, gmul = 0, rmul = 0;
			sum = 0;
			
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
				*(dst + y*C + x) = mul / sum;
			}
			else
			{
				*(dst + y*C*channels + x*channels + 0) = bmul / sum;
				*(dst + y*C*channels + x*channels + 1) = gmul / sum;
				*(dst + y*C*channels + x*channels + 2) = rmul / sum;
			}

		}
	}
}