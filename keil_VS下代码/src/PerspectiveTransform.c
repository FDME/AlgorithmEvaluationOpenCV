/*
		This is the file including functions related to perspective transform.
		Functions are as following,
			void PerspectiveTransform(POINT src[], POINT dst[]);
*/
#include "header.h"
extern int muav(double a[],int m,int n,double u[],double v[],double eps,int ka);
extern int ginv(double a[],int m,int n,double aa[],double eps,double u[],double v[],int ka);
void PerspectiveTransform(POINT src[], POINT dst[])
{
	double M[3][3];
	double a[8][8],b[8];
	int i,j,k,x,y;
	double sum;
	double aa[8][8],u[8][8],v[8][8],eps=0.0001,s;
	for(i = 0; i < 4; i++)
	{
		a[i][0] = a[i+4][3] = src[i].x;
        a[i][1] = a[i+4][4] = src[i].y;
        a[i][2] = a[i+4][5] = 1;
        a[i][3] = a[i][4] = a[i][5] =
        a[i+4][0] = a[i+4][1] = a[i+4][2] = 0;
        a[i][6] = -src[i].x*dst[i].x;
        a[i][7] = -src[i].y*dst[i].x;
        a[i+4][6] = -src[i].x*dst[i].y;
        a[i+4][7] = -src[i].y*dst[i].y;
        b[i] = dst[i].x;
        b[i+4] = dst[i].y;
	}
	for(i = 0; i != 8; i++)
	{
		for(j = 0; j != 8; j++)
			printf("%lf ",a[i][j]);
		printf("\n");
	}
	printf("\n");
	for(i = 0; i != 8; i++)
		printf("%lf ",b[i]);
	printf("\n\n");
	//下面的是高斯消元法，不适用
	//需用SVD求解线性方程组
	//for(i = 0; i < 8; i++)
	//	for(j = i+1; j < 8; j++)
	//		for(k = i; k < 8; k++)
	//		{
	//			a[j][k] = a[j][k]*a[i][i] - a[i][k]*a[j][i];
	//			b[k] = b[k]*a[i][i] - b[i]*a[j][i];
	//		}
	//for(i = 7; i >= 0; i--)
	//{
	//	sum = 0;
	//	for(j = 7; j > i; j--)
	//	{
	//		sum += M[j/3][j%3]*a[i][j];
	//	}
	//	M[j/3][j%3] = (b[i] - sum)/a[i][i];
	//}
	//M[2][2] = 1;
	//for(i = 0; i != 8; i++)
	//{
	//	for(j = 0; j != 8; j++)
	//		printf("%lf ",a[i][j]);
	//	printf("\n");
	//}
	//printf("\n");
	//for(i = 0; i != 3; i++)
	//{
	//	for(j = 0; j != 3; j++)
	//		printf("%lf ",M[i][j]);
	//	printf("\n");
	//}
	/*************************SVD求解方程组**************************/
	i=ginv(a,8,8,aa,eps,u,v,9);
	printf("MAT U(%d*%d) IS:\n",8,8);
	printf("MAT A+(%d*%d) is:\n",8,8);
	if(i>0)
	{
        for(i=0;i<8;i++)
		{
	 	  for(j=0;j<8;j++)
			printf("%13.4f",aa[i][j]);
		 printf("\n");
		}
	}
	for(i=0;i<8;i++)
	{
		for(j=0;j<8;j++)
		   printf("%13.4e",u[i][j]);
		printf("\n");
	}
	printf("\n");
    printf("MAT V(%d*%d) IS:\n",8,8);
	for(i=0;i<8;i++)
	{
		for(j=0;j<8;j++)
		   printf("%13.4e",v[i][j]);
		printf("\n");
	}
	printf("\n");
    printf("MAT W(%d*%d) IS:\n",8,8);
	for(i=0;i<8;i++)
	{
		for(j=0;j<8;j++)
		   printf("%13.4e",a[i][j]);
		printf("\n");
	}
    printf("\n");

	for(i=0;i<8;i++)
	{
		s=0.0;
		for(j=0;j<8;j++)
		{ s=s+aa[i][j]*b[j];}
		M[i/3][i%3]=s;
	}
	printf("方程组AX=b的解向量X为:\n\n");
	for(i=0;i<8;i++)
	{
		printf("%13.4f",M[i/3][i%3]);
	}
	printf("\n");
	M[2][2] = 1;
		for(i = 0; i != 3; i++)
	{
		for(j = 0; j != 3; j++)
			printf("%lf ",M[i][j]);
		printf("\n");
	}
	system("PAUSE");
	//for(i = 0; i != SIZE; i++){
	//	x = i/640;
	//	y = i%640;
	//	j = (M[0][0]*x + M[0][1]*y + M[0][2])/(M[2][0]*x + M[2][1]*y + M[2][2]);
	//	k = (M[1][0]*x + M[1][1]*y + M[1][2])/(M[2][0]*x + M[2][1]*y + M[2][2]);
	//	image_Transform[i] = image_RGB[j*640+k];
	//}
	//return M;
}