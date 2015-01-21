#include"retinex.h"
#include"GaussianSmooth.h"
#include<stdlib.h>
//extern Mat imageRetinex;

extern float temp0_f[SIZE * 3], temp1_f[SIZE * 3],temp2_f[SIZE*3];
//�����ֵ����׼��
void AvgSdv(float* src_fl, float* mean, float* dev)
{
	float sum[3] = { 0.0, 0.0, 0.0 };
	float temp[3] = { 0.0, 0.0, 0.0 };
	int i, j, k;
	//���ֵ
	for (i = 0; i < R; i++)
	for (j = 0; j < C; j++)
	for (k = 0; k < 3; k++)
		sum[k] += src_fl[i*C * 3 + j * 3 + k];
	
	for (k = 0; k < 3; k++)
		mean[k] = sum[k] / (SIZE);

	//���׼��
	for (i = 0; i < R; i++)
	for (j = 0; j < C; j++)
	for (k = 0; k < 3; k++)
		temp[k] += (src_fl[i*C * 3 + j * 3 + k] - mean[k])*(src_fl[i*C * 3 + j * 3 + k] - mean[k]);
	
	for (k = 0; k < 3; k++)
		dev[k] = sqrt(temp[k] / SIZE);
}
/********************************************************************************
���߶�Retinexͼ����ǿ����
sigmaΪ��˹ģ����׼��
scaleΪ�Աȶ�ϵ��
�����������Ϊr,g,b,gray
*********************************************************************************/
void retinex(UINT8T* r, UINT8T* g, UINT8T* b, UINT8T* gray, int sigma, int scale)
{
	//Retinex��ʽ��Log(R(x, y)) = Log(I(x, y)) - Log(Gauss(I(x, y)))

	int i, j, k;
	//float src_fl[SIZE * 3]; //ԭͼ���������ͼ
	//float src_fl1[SIZE * 3];//Log(I(x, y))
	//float src_fl2[SIZE * 3];//Log(Gauss(I(x, y)))

	float* src_fl;//ԭͼ���������ͼ
	float* src_fl1;//Log(I(x, y))
	//float* src_fl2;//Log(Gauss(I(x, y)))

	float mean[3], dev[3]; //b,g,r
	float min[3];
	float max[3];
	float maxmin[3];
	float temp1, temp2;
	 
	//float[SIZE*3]�Ĵ����鸳ֵ
	src_fl = temp0_f;  
	src_fl1 = temp1_f;
	//src_fl2 = temp2_f;

	//��ά��һά, ��ת����Χ������ͼ��Ԫ������1.0��֤cvlog����
	for (i = 0; i < R; i++)
	for (j = 0; j < C; j++)
	{
		//+1�Ӷ�����log0 = -INF
		src_fl[i*C * 3 + j * 3] = (float)b[i*C+j]+1;
		src_fl[i*C * 3 + j * 3 + 1] = (float)g[i*C + j]+1;
		src_fl[i*C * 3 + j * 3 + 2] = (float)r[i*C + j]+1;
	}

	//����Log(I(x, y)+1)
	for (i = 0; i < R; i++)
	for (j = 0; j < C; j++)
	for (k = 0; k < 3; k++)
	{
		
		src_fl1[i*C * 3 + j * 3 + k] = log(src_fl[i*C * 3 + j * 3 + k]);
	}
	//��˹ģ��
	//calc_gaussian_5x5(src_fl2, src_fl, 3);
	//GaussianSmooth(src_fl2, src_fl, sigma, 3);
	GaussianSmooth(src_fl, src_fl, sigma, 3);
	
	//����Log(Gauss(I(x, y)))
 	for (i = 0; i < R; i++)
	for (j = 0; j < C; j++)
	for (k = 0; k < 3; k++)
		src_fl[i*C * 3 + j * 3 + k] = log(src_fl[i*C * 3 + j * 3 + k]);
	
	//Log(R(x, y)) = Log(I(x, y)) - Log(Gauss(I(x, y)))
	for (i = 0; i < R; i++)
	for (j = 0; j < C; j++)
	for (k = 0; k < 3; k++)
		src_fl[i*C * 3 + j * 3 + k] = src_fl1[i*C * 3 + j * 3 + k] - src_fl[i*C * 3 + j * 3 + k];

	//��������
	//����ͼ��ľ�ֵ�����SSR�㷨�ĺ���֮��
	//ʹ��GIMP��ת��������ʹ��ͼ��ľ�ֵ�������Ϣ���б任
	//û���������ж�


	AvgSdv(src_fl, mean, dev);//����ͼ��ľ�ֵ�ͱ�׼��
	for (k = 0; k<3; k++)
	{
		min[k] = mean[k] - scale*dev[k];
		max[k] = mean[k] + scale*dev[k];
		maxmin[k] = max[k] - min[k];
	}
	
	//����
	for (i = 0; i < R;i++)
	for (j = 0; j < C; j++)
	for (k = 0; k < 3; k++)
	{
		temp1 = 255 * (src_fl[i*C * 3 + j * 3 + k] - min[k]) / maxmin[k];
		temp2 = temp1>255 ? 255 : temp1;
		src_fl[i*C * 3 + j * 3 + k] = temp2<0?0:temp2;
	}
		

	//ת�����
	for (i = 0; i < R; i++)
	for (j = 0; j < C; j++)
	{
		b[i*C + j] = (UINT8T)src_fl[i*C * 3 + j * 3];
		g[i*C + j] = (UINT8T)src_fl[i*C * 3 + j * 3 + 1];
		r[i*C + j] = (UINT8T)src_fl[i*C * 3 + j * 3 + 2];
	}
	/*for (int i = 0; i<src_fl2->width; i++)
	{
		for (int j = 0; j<src_fl2->height; j++)
		{
			data2[j*src_fl->widthStep / 4 + 3 * i + 0] = 255 * (data2[j*src_fl->widthStep / 4 + 3 * i + 0] - min[0]) / maxmin[0];
			data2[j*src_fl->widthStep / 4 + 3 * i + 1] = 255 * (data2[j*src_fl->widthStep / 4 + 3 * i + 1] - min[1]) / maxmin[1];
			data2[j*src_fl->widthStep / 4 + 3 * i + 2] = 255 * (data2[j*src_fl->widthStep / 4 + 3 * i + 2] - min[2]) / maxmin[2];
		}
	}
*/
	//����Ԥ�����ĻҶ�ͼ
	calc_gray();
}
