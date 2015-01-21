#include"canny.h"
#include"basic_functions.h" //gauss
#include"Otsu.h"
#include"test.h"

//////�Է�������ƺ��ͼ����б�Ե����/////////
void TraceEdge(int y, int x, int nThrLow, UINT8T* pimage_Gauss, int *pMag)
{
	//��8�������ؽ��в�ѯ 
	int xNum[8] = { 1, 1, 0, -1, -1, -1, 0, 1 };
	int yNum[8] = { 0, 1, 1, 1, 0, -1, -1, -1 };

	long yy, xx, k;

	for (k = 0; k<8; k++)
	{
		yy = y + yNum[k];
		xx = x + xNum[k];

		if (pimage_Gauss[yy*C + xx] == 128 && pMag[yy*C + xx] >= nThrLow)
		{
			//�õ���Ϊ�߽�� 
			pimage_Gauss[yy*C + xx] = 255;

			//�Ըõ�Ϊ�����ٽ��и��� 
			TraceEdge(yy, xx, nThrLow, pimage_Gauss, pMag);
		}
	}
}

int canny(UINT8T* dst, UINT8T* src, int option) // option = 0: Otsu������ϸС��Ե��option = 1 : ͳ��ϸС��Ե
{
	//�������� 
	UINT32T x, y;
	//�����������ݶ�//////
	//int Gradx[SIZE];
	//int Grady[SIZE];
	//int Mag[SIZE];
	int* Gradx;
	int* Grady;
	int* Mag;
//�м����
	double dSqt1;
	double dSqt2;
	//x����ķ�����

	//���������
	int nPos;//��ǰλ��
	int gx;//x������ݶ�
	int gy;//y������ݶ�
	//�м����
	int g1, g2, g3, g4;
	double weight;
	double dTemp, dTemp1, dTemp2;
	
	//����Ӧ��ֵ
	UINT32T k;
	int nHist[256];//ֱ��ͼ����
	int nEdgeNum;
	int nMaxMag = 0;
	int nHighCount;
	
	double dRatHigh = 0.8;
	double dRatLow = 0.4;
	int nThrHigh, nThrLow;
	
	//��˹�˲�
	UINT8T gauss[SIZE];

	if (option != 0 && option != 1)
	{
		logStr("Canny: Option error!\n");
		return FALSE;
	}

	Gradx = (int*)malloc(sizeof(int)*SIZE);
	Grady = (int*)malloc(sizeof(int)*SIZE);
	Mag = (int*)malloc(sizeof(int)*SIZE);

	calc_gaussian_5x5(gauss, src);
	//���ݶ�
	for (y = 1; y<R - 1; y++)
	{
		for (x = 1; x<C - 1; x++)
		{
			Gradx[y*C + x] = (int)(gauss[y*C + x + 1] - gauss[y*C + x - 1]);
		}
	}

	//y����ķ�����

	for (x = 1; x<C; x++)
	{
		for (y = 1; y<R; y++)
		{
			Grady[y*C + x] = (int)(gauss[(y + 1)*C + x] - gauss[(y - 1)*C + x]);
		}
	}
	//���ݶ�

	for (y = 0; y<R; y++)
	{
		for (x = 0; x<C; x++)
		{
			//���׷������ݶ�
			dSqt1 = Gradx[y*C + x] * Gradx[y*C + x];
			dSqt2 = Grady[y*C + x] * Grady[y*C + x];
			Mag[y*C + x] = (int)(sqrt(dSqt1 + dSqt2) + 0.5);
		}
	}

	//���������
	
	//����ͼ���ԵΪ�����ܵķֽ��
	for (x = 0; x<C; x++)
	{
		dst[x] = 0;                                 //��һ��
		dst[(R - 1)*C + x] = 0;   //���һ��
	}
	for (y = 0; y<R; y++)
	{
		dst[y*C] = 0;                  //��һ��
		dst[y*C + C - 1] = 0; //���һ��
	}

	for (y = 1; y<R - 1; y++)
	{
		for (x = 1; x<C - 1; x++)
		{
			//��ǰ��
			nPos = y*C + x;
			if (Mag[nPos] == 0)         //�ݶȷ�ֵΪ0
				dst[nPos] = 0;

			else                      //��ֵ����
			{
				//��ǰ����ݶȷ���
				dTemp = Mag[nPos];
				//x,y������
				gx = Gradx[nPos];
				gy = Grady[nPos];
				//���������y������x������˵����������������y����
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

					//�����x,y��������ķ����෴
					//C�ǵ�ǰ���أ���g1-g4�Ĺ�ϵΪ��
					//g2  g1
					//C
					//g3  g4
					else
					{
						g1 = Mag[nPos - C + 1];
						g3 = Mag[nPos + C - 1];
					}
				}
				//���������x������y������
				else
				{
					//��ֵ����
					weight = abs(gy) / abs(gx);
					g2 = Mag[nPos + 1];
					g4 = Mag[nPos - 1];
					//���x��y��������ķ�����������ͬ
					//��ǰ����C��g1-g4�Ĺ�ϵΪ
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
				//����g1-g4���ݶȽ��в�ֵ
				{
					dTemp1 = weight*g1 + (1 - weight)*g2;
					dTemp2 = weight*g3 + (1 - weight)*g4;

					//��ǰ���ص��ݶ��Ǿֲ������ֵ
					//�õ�����Ǳ߽��
					if (dTemp >= dTemp1&&dTemp >= dTemp2)
					{
						dst[nPos] = 128;
					}
					else
					{
						dst[nPos] = 0;//�������Ǳ߽��
					}
				}

			}
		}
	}
	
	//��������Ӧ��ֵ
	if (option == 0)
	{
		nThrHigh = (int)Otsu(src);
		nThrLow = nThrHigh / 2;
	}

	else
	{
		///////ͳ��pMag��ֱ��ͼ���ж���ֵ//////////

		//��ʼ��
		for (k = 0; k<256; k++)
		{
			nHist[k] = 0;
		}
		//ͳ��ֱ��ͼ������ֱ��ͼ������ֵ
		for (y = 0; y<R; y++)
		{
			for (x = 0; x<C; x++)
			{
				if (dst[y*C + x] == 128)
				{
					nHist[Mag[y*C + x]]++;
				}
			}
		}

		nEdgeNum = nHist[0];
		nMaxMag = 0;
		//ͳ�ƾ����������ֵ���ơ����ж�������
		for (k = 1; k<256; k++)
		{
			if (nHist[k] != 0)
			{
				nMaxMag = k;
			}
			nEdgeNum += nHist[k];
		}
		//�ݶȱȸ���ֵС�����ص�����Ŀ
		nHighCount = (int)(dRatHigh*nEdgeNum + 0.5);

		k = 1;
		nEdgeNum = nHist[1];
		//�������ֵ
		while ((k<(nMaxMag - 1)) && (nEdgeNum<nHighCount))
		{
			k++;
			nEdgeNum += nHist[k];
		}

		nThrHigh = k;
		nThrLow = (int)((nThrHigh)*dRatLow + 0.5);
	}
#ifdef WIN32
	printf("Canny: nThrHigh = %d, nThrLow = %d\n", nThrHigh, nThrLow);
#endif

	//���ú���Ѱ�ұ߽����
	for (y = 0; y<R; y++)
	{
		for (x = 0; x<C; x++)
		{
			nPos = y*C + x;
			if ((dst[nPos] == 128) && (Mag[nPos] >= nThrHigh))
			{
				dst[nPos] = 255;
				//�Ըõ�Ϊ�����ٽ��и��� 
				TraceEdge(y, x, nThrLow, dst, Mag);
			}
		}
	}

	//�����ĵ��Ѿ�������
	for (y = 0; y<R; y++)
	{
		for (x = 0; x<C; x++)
		{
			nPos = y*C + x;
			if (dst[nPos] != 255)
			{
				dst[nPos] = 0;

			}
		}
	}

	free(Gradx);
	free(Grady);
	free(Mag);
	return TRUE;
}
