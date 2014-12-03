#include"header.h"
////////////全局变量/////

extern UINT8T  image_Gauss[SIZE];
extern UINT8T	image_Canny[SIZE];

//////对非最大抑制后的图像进行边缘搜索/////////
void TraceEdge(int y, int x, int nThrLow, UINT8T* pimage_Gauss, int *pMag)
{
	//对8邻域像素进行查询 
	int xNum[8] = { 1, 1, 0, -1, -1, -1, 0, 1 };
	int yNum[8] = { 0, 1, 1, 1, 0, -1, -1, -1 };

	long yy, xx, k;

	for (k = 0; k<8; k++)
	{
		yy = y + yNum[k];
		xx = x + xNum[k];

		if (pimage_Gauss[yy*C + xx] == 128 && pMag[yy*C + xx] >= nThrLow)
		{
			//该点设为边界点 
			pimage_Gauss[yy*C + xx] = 255;

			//以该点为中心再进行跟踪 
			TraceEdge(yy, xx, nThrLow, pimage_Gauss, pMag);
		}
	}
}


void canny()
{
	UINT32T x, y;
	//方向导数，求梯度//////
	int Gradx[SIZE];
	int Grady[SIZE];
	int Mag[SIZE];

	//x方向的方向导数

	for (y = 1; y<R - 1; y++)
	{
		for (x = 1; x<C - 1; x++)
		{
			Gradx[y*C + x] = (int)(image_Gauss[y*C + x + 1] - image_Gauss[y*C + x - 1]);
		}
	}

	//y方向的方向导数

	for (x = 1; x<C; x++)
	{
		for (y = 1; y<R; y++)
		{
			Grady[y*C + x] = (int)(image_Gauss[(y + 1)*C + x] - image_Gauss[(y - 1)*C + x]);
		}
	}
	//求梯度
	//中间变量
	double dSqt1;
	double dSqt2;

	for (y = 0; y<R; y++)
	{
		for (x = 0; x<C; x++)
		{
			//二阶范数求梯度
			dSqt1 = Gradx[y*C + x] * Gradx[y*C + x];
			dSqt2 = Grady[y*C + x] * Grady[y*C + x];
			Mag[y*C + x] = (int)(sqrt(dSqt1 + dSqt2) + 0.5);
		}
	}

	//非最大抑制
	int nPos;//当前位置
	int gx;//x方向的梯度
	int gy;//y方向的梯度
	//中间变量
	int g1, g2, g3, g4;
	double weight;
	double dTemp, dTemp1, dTemp2;


	//设置图像边缘为不可能的分界点
	for (x = 0; x<C; x++)
	{
		image_Canny[x] = 0;                                 //第一行
		image_Canny[(R - 1)*C + x] = 0;   //最后一行
	}
	for (y = 0; y<R; y++)
	{
		image_Canny[y*C] = 0;                  //第一列
		image_Canny[y*C + C - 1] = 0; //最后一列
	}

	for (y = 1; y<R - 1; y++)
	{
		for (x = 1; x<C - 1; x++)
		{
			//当前点
			nPos = y*C + x;
			if (Mag[nPos] == 0)         //梯度幅值为0
				image_Canny[nPos] = 0;

			else                      //幅值非零
			{
				//当前点的梯度幅度
				dTemp = Mag[nPos];
				//x,y方向导数
				gx = Gradx[nPos];
				gy = Grady[nPos];
				//如果方向倒数y分量比x分量大，说明导数方向趋向于y分量
				if (abs(gy)>abs(gx))
				{
					weight = abs(gx) / abs(gy);
					g2 = Mag[nPos - C];
					g4 = Mag[nPos + C];
					if (gx*gy>0)
					{
						g1 = Mag[nPos - C - 1];
						g3 = Mag[nPos + C + 1];
					}

					//如果想x,y两个方向的方向相反
					//C是当前像素，与g1-g4的关系为：
					//g2  g1
					//C
					//g3  g4
					else
					{
						g1 = Mag[nPos - C + 1];
						g3 = Mag[nPos + C - 1];
					}
				}
				//如果方向导数x分量比y分量大
				else
				{
					//插值比例
					weight = abs(gy) / abs(gx);
					g2 = Mag[nPos + 1];
					g4 = Mag[nPos - 1];
					//如果x，y两个方向的方向导数符号相同
					//当前像素C与g1-g4的关系为
					//g3
					//g4  C g2
					//g1
					if (gx*gy>0)
					{
						g1 = Mag[nPos + C + 1];
						g3 = Mag[nPos - C - 1];
					}
					else
					{
						g1 = Mag[nPos - C + 1];
						g3 = Mag[nPos + C - 1];

					}
				}
				//利用g1-g4对梯度进行插值
				{
					dTemp1 = weight*g1 + (1 - weight)*g2;
					dTemp2 = weight*g3 + (1 - weight)*g4;

					//当前像素的梯度是局部的最大值
					//该点可能是边界点
					if (dTemp >= dTemp1&&dTemp >= dTemp2)
					{
						image_Canny[nPos] = 128;
					}
					else
					{
						image_Canny[nPos] = 0;//不可能是边界点
					}
				}

			}
		}
	}
	///////统计pMag的直方图，判定阈值//////////待调整
	UINT32T k;
	int nHist[256];//直方图数组
	int nEdgeNum;
	int nMaxMag;
	int nHighCount;
	nMaxMag = 0;
	double dRatHigh = 0.8;
	double dRatLow = 0.4;
	//初始化
	for (k = 0; k<256; k++)
	{
		nHist[k] = 0;
	}
	//统计直方图，利用直方图计算阈值
	for (y = 0; y<R; y++)
	{
		for (x = 0; x<C; x++)
		{
			if (image_Canny[y*C + x] == 128)
			{
				nHist[Mag[y*C + x]]++;
			}
		}
	}

	nEdgeNum = nHist[0];
	nMaxMag = 0;
	//统计经过“非最大值抑制”后有多少像素
	for (k = 1; k<256; k++)
	{
		if (nHist[k] != 0)
		{
			nMaxMag = k;
		}
		nEdgeNum += nHist[k];
	}
	//梯度比高阈值小的像素点总数目
	nHighCount = (int)(dRatHigh*nEdgeNum + 0.5);

	k = 1;
	nEdgeNum = nHist[1];
	//计算高阈值
	while ((k<(nMaxMag - 1)) && (nEdgeNum<nHighCount))
	{
		k++;
		nEdgeNum += nHist[k];
	}
	int nThrHigh, nThrLow;
	nThrHigh = k;
	nThrLow = (int)((nThrHigh)*dRatLow + 0.5);
	printf("Canny: nThrHigh = %d, nThrLow = %d\n", nThrHigh, nThrLow);
	//利用函数寻找边界起点
	for (y = 0; y<R; y++)
	{
		for (x = 0; x<C; x++)
		{
			nPos = y*C + x;
			if ((image_Canny[nPos] == 128) && (Mag[nPos] >= nThrHigh))
			{
				image_Canny[nPos] = 255;
				//以该点为中心再进行跟踪 
				TraceEdge(y, x, nThrLow, image_Canny, Mag);
			}
		}
	}

	//其他的点已经不可能
	for (y = 0; y<R; y++)
	{
		for (x = 0; x<C; x++)
		{
			nPos = y*C + x;
			if (image_Canny[nPos] != 255)
			{
				image_Canny[nPos] = 0;

			}
		}
	}
}