#include"header.h"
////////////ȫ�ֱ���/////

extern UINT8T  image_Gauss[SIZE];
extern UINT8T	image_Canny[SIZE];

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


void canny()
{
	UINT32T x, y;
	//�����������ݶ�//////
	int Gradx[SIZE];
	int Grady[SIZE];
	int Mag[SIZE];

	//x����ķ�����

	for (y = 1; y<R - 1; y++)
	{
		for (x = 1; x<C - 1; x++)
		{
			Gradx[y*C + x] = (int)(image_Gauss[y*C + x + 1] - image_Gauss[y*C + x - 1]);
		}
	}

	//y����ķ�����

	for (x = 1; x<C; x++)
	{
		for (y = 1; y<R; y++)
		{
			Grady[y*C + x] = (int)(image_Gauss[(y + 1)*C + x] - image_Gauss[(y - 1)*C + x]);
		}
	}
	//���ݶ�
	//�м����
	double dSqt1;
	double dSqt2;

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
	int nPos;//��ǰλ��
	int gx;//x������ݶ�
	int gy;//y������ݶ�
	//�м����
	int g1, g2, g3, g4;
	double weight;
	double dTemp, dTemp1, dTemp2;


	//����ͼ���ԵΪ�����ܵķֽ��
	for (x = 0; x<C; x++)
	{
		image_Canny[x] = 0;                                 //��һ��
		image_Canny[(R - 1)*C + x] = 0;   //���һ��
	}
	for (y = 0; y<R; y++)
	{
		image_Canny[y*C] = 0;                  //��һ��
		image_Canny[y*C + C - 1] = 0; //���һ��
	}

	for (y = 1; y<R - 1; y++)
	{
		for (x = 1; x<C - 1; x++)
		{
			//��ǰ��
			nPos = y*C + x;
			if (Mag[nPos] == 0)         //�ݶȷ�ֵΪ0
				image_Canny[nPos] = 0;

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
						image_Canny[nPos] = 128;
					}
					else
					{
						image_Canny[nPos] = 0;//�������Ǳ߽��
					}
				}

			}
		}
	}
	///////ͳ��pMag��ֱ��ͼ���ж���ֵ//////////������
	UINT32T k;
	int nHist[256];//ֱ��ͼ����
	int nEdgeNum;
	int nMaxMag;
	int nHighCount;
	nMaxMag = 0;
	double dRatHigh = 0.8;
	double dRatLow = 0.4;
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
			if (image_Canny[y*C + x] == 128)
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
	int nThrHigh, nThrLow;
	nThrHigh = k;
	nThrLow = (int)((nThrHigh)*dRatLow + 0.5);
	printf("Canny: nThrHigh = %d, nThrLow = %d\n", nThrHigh, nThrLow);
	//���ú���Ѱ�ұ߽����
	for (y = 0; y<R; y++)
	{
		for (x = 0; x<C; x++)
		{
			nPos = y*C + x;
			if ((image_Canny[nPos] == 128) && (Mag[nPos] >= nThrHigh))
			{
				image_Canny[nPos] = 255;
				//�Ըõ�Ϊ�����ٽ��и��� 
				TraceEdge(y, x, nThrLow, image_Canny, Mag);
			}
		}
	}

	//�����ĵ��Ѿ�������
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