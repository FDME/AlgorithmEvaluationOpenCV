//This is for solving the linear equation in perspectivetransform
extern int muav(double a[],int m,int n,double u[],double v[],double eps,int ka);
int  ginv(double* a,int m,int n,double aa[],double eps,double u[],double v[],int ka)
{
	int i,j,k,l,t,p,q,f;
	i=muav(a,m,n,u,v,eps,ka);
	if(i<0) return(-1);
	j=n;
	if(m<n) j=m;
	j=j-1;
	k=0;
	while((k<=j)&&(a[k*n+k]!=0.0)) k=k+1;
	k=k-1;
	for(i=0;i<=n-1;i++)
		for(j=0;j<=m-1;j++)
		{
			t=i*m+j;
			aa[t]=0.0;
			for(l=0;l<=k;l++)
			{
				f=l*n+i;p=j*m+l;q=l*n+l;
				aa[t]=aa[t]+v[f]*u[p]/a[q];
			}
		}
		return(1);
}