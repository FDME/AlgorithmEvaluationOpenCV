/*
		This is the file including functions related to perspective transform.
		Functions are as following,
			void PerspectiveTransform(POINT src[], POINT dst[]);
*/
#include "PerspectiveTransform.h"
int muav(double a[],int m,int n,double u[],double v[],double eps,int ka);
int ginv(double a[],int m,int n,double aa[],double eps,double u[],double v[],int ka);

extern RGBTYPE image_Transform[SIZE];
extern RGBTYPE image_RGB[SIZE]; 

//求逆矩阵
  int brinv(double a[])
   { int is[3],js[3],i,j,k,l,u,v,n=3;
    double d,p;
    for (k=0; k<=n-1; k++)
      { d=0.0;
        for (i=k; i<=n-1; i++)
        for (j=k; j<=n-1; j++)
          { l=i*n+j; p=fabs(a[l]);
            if (p>d) { d=p; is[k]=i; js[k]=j;}
          }
        if (d+1.0==1.0)
          { free(is); free(js); printf("err**not inv\n");
            return(0);
          }
        if (is[k]!=k)
          for (j=0; j<=n-1; j++)
            { u=k*n+j; v=is[k]*n+j;
              p=a[u]; a[u]=a[v]; a[v]=p;
            }
        if (js[k]!=k)
          for (i=0; i<=n-1; i++)
            { u=i*n+k; v=i*n+js[k];
              p=a[u]; a[u]=a[v]; a[v]=p;
            }
        l=k*n+k;
        a[l]=1.0/a[l];
        for (j=0; j<=n-1; j++)
          if (j!=k)
            { u=k*n+j; a[u]=a[u]*a[l];}
        for (i=0; i<=n-1; i++)
          if (i!=k)
            for (j=0; j<=n-1; j++)
              if (j!=k)
                { u=i*n+j;
                  a[u]=a[u]-a[i*n+k]*a[k*n+j];
                }
        for (i=0; i<=n-1; i++)
          if (i!=k)
            { u=i*n+k; a[u]=-a[u]*a[l];}
      }
    for (k=n-1; k>=0; k--)
      { if (js[k]!=k)
          for (j=0; j<=n-1; j++)
            { u=k*n+j; v=js[k]*n+j;
              p=a[u]; a[u]=a[v]; a[v]=p;
            }
        if (is[k]!=k)
          for (i=0; i<=n-1; i++)
            { u=i*n+k; v=i*n+is[k];
              p=a[u]; a[u]=a[v]; a[v]=p;
            }
      }
    return(1);
  }
void PerspectiveTransform(POINT* src, POINT* dst)
{
	double M[9];
	double a[64],b[8];
	int i,j,k,x,y,px,py;
	double sum;
	double aa[64],u[64],v[64],eps=0.0001,s;
	//double ta[64],taa[64],tu[64],tv[64];
	for(i = 0; i < 4; i++)
	{
		a[i*8+0] = a[(i+4)*8+3] = src[i].x;
        a[i*8+1] = a[(i+4)*8+4] = src[i].y;
        a[i*8+2] = a[(i+4)*8+5] = 1;
        a[i*8+3] = a[i*8+4] = a[i*8+5] =
        a[(i+4)*8+0] = a[(i+4)*8+1] = a[(i+4)*8+2] = 0;
        a[i*8+6] = -src[i].x*dst[i].x;
        a[i*8+7] = -src[i].y*dst[i].x;
        a[(i+4)*8+6] = -src[i].x*dst[i].y;
        a[(i+4)*8+7] = -src[i].y*dst[i].y;
        b[i] = dst[i].x;
        b[i+4] = dst[i].y;
	}
	for(i = 0; i != 8; i++)
	{
		for(j = 0; j != 8; j++)
			printf("%lf ",a[i*8+j]);
		printf("\n");
	}
	printf("\n");
	for(i = 0; i != 8; i++)
		printf("%lf ",b[i]);
	printf("\n\n");

	/*************************SVD求解方程组**************************/

	i=ginv(a,8,8,aa,eps,u,v,9);
	printf("MAT U(%d*%d) IS:\n",8,8);
	printf("MAT A+(%d*%d) is:\n",8,8);
	if(i>0)
	{
        for(i=0;i<8;i++)
		{
	 	  for(j=0;j<8;j++)
			printf("%13.4f",aa[i*8+j]);
		 printf("\n");
		}
	}
	for(i=0;i<8;i++)
	{
		for(j=0;j<8;j++)
		   printf("%13.4e",u[i*8+j]);
		printf("\n");
	}
	printf("\n");
    printf("MAT V(%d*%d) IS:\n",8,8);
	for(i=0;i<8;i++)
	{
		for(j=0;j<8;j++)
		   printf("%13.4e",v[i*8+j]);
		printf("\n");
	}
	printf("\n");
    printf("MAT W(%d*%d) IS:\n",8,8);
	for(i=0;i<8;i++)
	{
		for(j=0;j<8;j++)
		   printf("%13.4e",a[i*8+j]);
		printf("\n");
	}
    printf("\n");

	for(i=0;i<8;i++)
	{
		s=0.0;
		for(j=0;j<8;j++)
		{ s=s+aa[i*8+j]*b[j];}
		M[i]=s;
	}
	printf("方程组AX=b的解向量X为:\n\n");
	for(i=0;i<8;i++)
	{
		printf("%13.4f",M[i]);
	}
	printf("\n");
	M[8] = 1;
		for(i = 0; i != 3; i++)
	{
		for(j = 0; j != 3; j++)
			printf("%lf ",M[i*3+j]);
		printf("\n");
	}
	i = brinv(M);
	for(i = 0; i != SIZE; i++){
		x = i/C;
		y = i%C;
		px = (M[0]*x + M[1]*y + M[0*3+2])/(M[2*3+0]*x + M[2*3+1]*y + M[2*3+2]);
		py = (M[1*3+0]*x + M[1*3+1]*y + M[1*3+2])/(M[2*3+0]*x + M[2*3+1]*y + M[2*3+2]);
		image_Transform[i] = image_RGB[py+px*C];
	}
	//return M;
}