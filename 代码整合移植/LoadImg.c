#include"LoadImg.h"
#include"decoder\jpg_decode.h"
#include"yuv2rgb.h"
#include"test.h"
#ifdef WIN32
#else
#include"camera_control.h"
#endif

extern UINT8T image_R[SIZE];
extern UINT8T image_G[SIZE];
extern UINT8T image_B[SIZE];
extern UINT8T r[SIZE], g[SIZE], b[SIZE];
//option = 0: ����ͷ�ɼ���option = 1: ͼƬ���飻 option = 2����jpeg�ļ�
int LoadImg(int option)
{
	int i, j, k;
	UINT32T jpg_size;
	UINT8T buffer[SIZE]; //�洢����jpeg
	UINT8T py[SIZE];	//�洢����YUV
	UINT8T pu[SIZE / 4];
	UINT8T pv[SIZE / 4];

#ifdef WIN32
	FILE* fp;
#endif
	if (option == 1)
	{
		//���ת��
		for (i = 0; i < R;i++)
		for (j = 0; j < C; j++)
		{
			b[i*C + j] = image_B[i*C + j];
			g[i*C + j] = image_G[i*C + j];
			r[i*C + j] = image_R[i*C + j];
		}
		return 0;
	}
	else //�����
	{
		if (option == 0)
		{
#ifdef WIN32
			logStr("LoadImg: option���ô���\n");
			return -1;
#else
			sys_init();        /* Initial s3c2410's Clock, MMU, Interrupt,Port and UART */
			uart_select(UART0);
			logStr("Take a picture\n");
			jpg_size = camera(buffer);
			uart_select(UART0);
			logStr("Finish transmitting\n");
#endif
		}
		else if (option == 2)
		{
#ifdef WIN32
			//fp = fopen("C:\\HuaWeiImage\\��Ϊ����-20141128\\����A������������\\jpeg_20141128_150639.jpg", "rb");
			//fp = fopen("C:\\Users\\ZoeQIAN\\Pictures\\��Ϊ����-20141128\\����A������������\\5.jpg","rb");
			//fp = fopen("E:\\VS2013_pro\\��Ϊ����-20141128\\����A--����һ���豸\\jpeg_20141128_152454.jpg", "rb");
			if (!(fp = fopen("E:\\VS2013_pro\\jpeg_20141128_151316.jpg","rb")))
			{
				printf("�޷����ļ���\n");  //����ͼƬ
				return -1;
			}
			jpg_size = filesize(fp);
			printf("size=%d\n", jpg_size);
			fread(buffer, jpg_size, 1, fp);
			fclose(fp);
			printf("����ͼƬ�ɹ���\n");
#else
			logStr("LoadImg: option���ô���\n");
			return FALSE;
#endif
		}
		else
		{
			logStr("LoadImg: option���ô���\n");
			return -1;
		}

		//����JPG
		if (jpg_decode(buffer, py, pu, pv, jpg_size)<0)
			logStr("Decoding error!\n");
		else
			logStr("Finish decoding\n");
		
		if (yuv2rgb(py, pu, pv, r,g,b) == -1)
			logStr("RGB error!\n");
		else
			logStr("Finish RGB\n");
		return 0;
	}
}