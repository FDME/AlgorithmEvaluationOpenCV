/*
		This is the file including functions related to perspective transform.
		Functions are as following,
			void PerspectiveTransform(POINT src[], POINT dst[]);
*/
#include "header.h"
int muav(double a[],int m,int n,double u[],double v[],double eps,int ka);
int ginv(double a[],int m,int n,double aa[],double eps,double u[],double v[],int ka);
void PerspectiveTransform(POINT src[], POINT dst[]);