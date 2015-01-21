#include"retinex.h"
#include"basic_functions.h"
#include "test.h"

/*input: r[SIZE], g[SIZE], b[SIZE] //�������RGB��Ϣ��gray
output: r[SIZE], g[SIZE], b[SIZE], gray[SIZE] //Ԥ�����ĻҶ�ͼ���Լ�RGB
return: int. -1: ��Ƭ���ջ��������ѣ����������㡣0: ��Ƭ���ã��ѽ����жϺ�Ԥ����
usege : Ԥ�������ȵ��ڵ�*/
//�ڲ���ʱ������float temp0_f[SIZE * 3], temp1_f[SIZE * 3](������retinex.c), temp2_f[SIZE * 3](������GaussianSmooth.h);
extern UINT8T r[SIZE], g[SIZE], b[SIZE], gray[SIZE];
int PreProcess()
{
	UINT32T mean, variance;

	//�õ�У����ĻҶ�ͼ
	calc_gray(); // ����r,g,b�����gray
	//����Ҷ�ͼ�ľ�ֵ�뷽��
	mean = Mean(gray, 0, 0, C, R);
	variance = Variance(gray, 0, 0, C, R, mean);
#ifdef WIN32
	printf("preProcess: mean = %d, variance = %d\n", mean, variance);
#endif

	//�ж��Ƿ���Ҫ����ͼƬ
	//meanԽС��ͼƬԽ��
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

	//����ԽС��ͼƬ����Խ����
	if (variance < 500)
	{
		logStr("The quality of the image is too bad!\n");
		return -1;
	}

	//�ж��Ƿ���Ҫ������ǿ
	if (mean < 110)
	{
		retinex(r,g, b, gray, 30, 1);
		logStr("Finish Retinex\n");
	}
	else
		logStr("No need for Retinex\n");

	return 0;
}