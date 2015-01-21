#include"retinex.h"
#include"GaussianSmooth.h"
#include<stdlib.h>
//extern Mat imageRetinex;

extern float temp0_f[SIZE * 3], temp1_f[SIZE * 3],temp2_f[SIZE*3];
//计算均值，标准差
void AvgSdv(float* src_fl, float* mean, float* dev)
{
	float sum[3] = { 0.0, 0.0, 0.0 };
	float temp[3] = { 0.0, 0.0, 0.0 };
	int i, j, k;
	//算均值
	for (i = 0; i < R; i++)
	for (j = 0; j < C; j++)
	for (k = 0; k < 3; k++)
		sum[k] += src_fl[i*C * 3 + j * 3 + k];
	
	for (k = 0; k < 3; k++)
		mean[k] = sum[k] / (SIZE);

	//算标准差
	for (i = 0; i < R; i++)
	for (j = 0; j < C; j++)
	for (k = 0; k < 3; k++)
		temp[k] += (src_fl[i*C * 3 + j * 3 + k] - mean[k])*(src_fl[i*C * 3 + j * 3 + k] - mean[k]);
	
	for (k = 0; k < 3; k++)
		dev[k] = sqrt(temp[k] / SIZE);
}
/********************************************************************************
单尺度Retinex图像增强程序
sigma为高斯模糊标准差
scale为对比度系数
输入与输出均为r,g,b,gray
*********************************************************************************/
void retinex(UINT8T* r, UINT8T* g, UINT8T* b, UINT8T* gray, int sigma, int scale)
{
	//Retinex公式，Log(R(x, y)) = Log(I(x, y)) - Log(Gauss(I(x, y)))

	int i, j, k;
	//float src_fl[SIZE * 3]; //原图，量化结果图
	//float src_fl1[SIZE * 3];//Log(I(x, y))
	//float src_fl2[SIZE * 3];//Log(Gauss(I(x, y)))

	float* src_fl;//原图，量化结果图
	float* src_fl1;//Log(I(x, y))
	//float* src_fl2;//Log(Gauss(I(x, y)))

	float mean[3], dev[3]; //b,g,r
	float min[3];
	float max[3];
	float maxmin[3];
	float temp1, temp2;
	 
	//float[SIZE*3]的大数组赋值
	src_fl = temp0_f;  
	src_fl1 = temp1_f;
	//src_fl2 = temp2_f;

	//三维变一维, 并转换范围，所有图像元素增加1.0保证cvlog正常
	for (i = 0; i < R; i++)
	for (j = 0; j < C; j++)
	{
		//+1从而避免log0 = -INF
		src_fl[i*C * 3 + j * 3] = (float)b[i*C+j]+1;
		src_fl[i*C * 3 + j * 3 + 1] = (float)g[i*C + j]+1;
		src_fl[i*C * 3 + j * 3 + 2] = (float)r[i*C + j]+1;
	}

	//计算Log(I(x, y)+1)
	for (i = 0; i < R; i++)
	for (j = 0; j < C; j++)
	for (k = 0; k < 3; k++)
	{
		
		src_fl1[i*C * 3 + j * 3 + k] = log(src_fl[i*C * 3 + j * 3 + k]);
	}
	//高斯模糊
	//calc_gaussian_5x5(src_fl2, src_fl, 3);
	//GaussianSmooth(src_fl2, src_fl, sigma, 3);
	GaussianSmooth(src_fl, src_fl, sigma, 3);
	
	//计算Log(Gauss(I(x, y)))
 	for (i = 0; i < R; i++)
	for (j = 0; j < C; j++)
	for (k = 0; k < 3; k++)
		src_fl[i*C * 3 + j * 3 + k] = log(src_fl[i*C * 3 + j * 3 + k]);
	
	//Log(R(x, y)) = Log(I(x, y)) - Log(Gauss(I(x, y)))
	for (i = 0; i < R; i++)
	for (j = 0; j < C; j++)
	for (k = 0; k < 3; k++)
		src_fl[i*C * 3 + j * 3 + k] = src_fl1[i*C * 3 + j * 3 + k] - src_fl[i*C * 3 + j * 3 + k];

	//量化计算
	//计算图像的均值、方差，SSR算法的核心之二
	//使用GIMP中转换方法：使用图像的均值方差等信息进行变换
	//没有添加溢出判断


	AvgSdv(src_fl, mean, dev);//计算图像的均值和标准差
	for (k = 0; k<3; k++)
	{
		min[k] = mean[k] - scale*dev[k];
		max[k] = mean[k] + scale*dev[k];
		maxmin[k] = max[k] - min[k];
	}
	
	//量化
	for (i = 0; i < R;i++)
	for (j = 0; j < C; j++)
	for (k = 0; k < 3; k++)
	{
		temp1 = 255 * (src_fl[i*C * 3 + j * 3 + k] - min[k]) / maxmin[k];
		temp2 = temp1>255 ? 255 : temp1;
		src_fl[i*C * 3 + j * 3 + k] = temp2<0?0:temp2;
	}
		

	//转换结果
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
	//计算预处理后的灰度图
	calc_gray();
}
