#include"Otsu.h"

double Otsu(UINT8T* src)
{
	//histogram    
	double histogram[256] = { 0 };
	int i, j;
	UINT8T* p;
	
	double threshold;
	long sum0 = 0, sum1 = 0; //�洢ǰ���ĻҶ��ܺͺͱ����Ҷ��ܺ�  
	long cnt0 = 0, cnt1 = 0; //ǰ�����ܸ����ͱ������ܸ���  
	double w0 = 0, w1 = 0; //ǰ���ͱ�����ռ����ͼ��ı���  
	double u0 = 0, u1 = 0;  //ǰ���ͱ�����ƽ���Ҷ�  
	double variance = 0; //�����䷽��  
	double u = 0;
	double maxVariance = 0;

	//����ֱ��ͼ
	for (i = 0; i < R; i++)
	{
		p = &src[C * i];
		for (j = 0; j < C; j++)
		{
			histogram[(int)(*p++)]++;
		}
	}

	for (i = 1; i < 256; i++) //һ�α���ÿ������  
	{
		sum0 = 0;
		sum1 = 0;
		cnt0 = 0;
		cnt1 = 0;
		w0 = 0;
		w1 = 0;
		for (j = 0; j < i; j++)
		{
			cnt0 += histogram[j];
			sum0 += j * histogram[j];
		}

		u0 = (double)sum0 / cnt0;
		w0 = (double)cnt0 / SIZE;

		for (j = i; j <= 255; j++)
		{
			cnt1 += histogram[j];
			sum1 += j * histogram[j];
		}

		u1 = (double)sum1 / cnt1;
		w1 = 1 - w0; // (double)cnt1 / size;  

		u = u0 * w0 + u1 * w1; //ͼ���ƽ���Ҷ�  
		//printf("u = %f\n", u);
		//variance =  w0 * pow((u0 - u), 2) + w1 * pow((u1 - u), 2);  
		variance = w0 * w1 *  (u0 - u1) * (u0 - u1);
		if (variance > maxVariance)
		{
			maxVariance = variance;
			threshold = i;
		}
	}

	return threshold;
}