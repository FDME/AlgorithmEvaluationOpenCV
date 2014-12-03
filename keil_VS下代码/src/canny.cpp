#include"canny.h"

////////////全局变量/////

Bitmap bmp;
FILE *fp=0;
DWORD width =0,height=0,line_width=0;
WORD bit_count =0,pal_length=0,off_bits=0;
DWORD dw = 0;
WORD  w = 0;

//////对非最大抑制后的图像进行边缘搜索/////////

void TraceEdge(int y, int x, int nThrLow, LPBYTE pResult, int *pMag, Bitmap sz) 
{ 
//对8邻域像素进行查询 
int xNum[8] = {1,1,0,-1,-1,-1,0,1}; 
int yNum[8] = {0,1,1,1,0,-1,-1,-1}; 

long yy,xx,k; 

for(k=0;k<8;k++) 
{ 
yy = y+yNum[k]; 
xx = x+xNum[k]; 

if(pResult[yy*sz.line_width+xx]==128 && pMag[yy*sz.line_width+xx]>=nThrLow ) 
{ 
//该点设为边界点 
pResult[yy*sz.line_width+xx] = 255; 

//以该点为中心再进行跟踪 
TraceEdge(yy,xx,nThrLow,pResult,pMag,sz); 
} 
} 
} 


int main()
{
//////读取图像参数 /////
  
  if((fp=fopen("text.bmp","rb"))==NULL)
  {
	printf("can not open the bitmap");
	return 0;
  }
  else printf("read OK");
  fseek(fp,10,0);
  fread(&off_bits,4,1,fp);
  fseek(fp,18,0);
  fread(&width,4,1,fp);
  fread(&height,4,1,fp);
  fseek(fp,28,0);
  fread(&bit_count,2,1,fp);
  pal_length=off_bits-54;
  if(bit_count==8)
  {
	bmp.palette=(LPBYTE)malloc(pal_length*sizeof(BYTE));
    if(!bmp.palette)
		{printf("palette have no memory");
	     return 0;}
  }
  else
  {
	bmp.palette=0;
  }
  if(bmp.palette)
  {
	fseek(fp,54,0);
	fread(bmp.palette,pal_length,1,fp);
  }
  ////////Read the parameters of bitmap///
  line_width=(width*bit_count +31)/32*4;
  bmp.line_width=line_width;
  bmp.width=width;
  bmp.height=height;
  bmp.bit_count=bit_count;
  bmp.pal_length=pal_length;
  bmp.ptr=(LPBYTE)malloc(line_width*height*sizeof(BYTE));

  if(!bmp.ptr)
  {
  printf("can not allocate memory for the bitmap.\n");
  bmp.width=0;
  bmp.height=0;
  bmp.bit_count=0;
  }
  fseek(fp,off_bits,0);
  fread(bmp.ptr,line_width*height,1,fp);
  fclose(fp);
  printf("Bitmap Width :%ld\n",bmp.width);
  printf("Bitmap Heigh :%ld\n",bmp.height);
  printf("Bitmap BitCount:%d\n",bmp.bit_count);

  ///////////////////高斯滤波///////////////////////

  int nCenter;   //数组中心点
  double dDis;     //数组中一点到中心点的距离
  DWORD i;

  double dValue;    // 中间变量
  double dSum=0.0;

  double sigma=0.8;//高斯系数
  //[-3*sigma,3*sigma]以内的数据，会覆盖绝大部分滤波系数，也就是3西伽马原则
  int WindowSize=1+2*ceil(3*sigma);
  nCenter=WindowSize/2;
  double*Kernel=NULL;                          //高斯核数组
  Kernel=(double*)malloc(WindowSize*sizeof(double));
  for(i=0;i<WindowSize;i++)
  {
	dDis=double(i-nCenter);
	dValue=exp(-(1/2)*dDis*dDis/(sigma*sigma))/(sqrt(2*3.1415926)*sigma);
	Kernel[i]=dValue;
	dSum+=dValue;
  }
  
	for(i=0;i<WindowSize;i++)
	{
		Kernel[i]/=dSum;                  //归一化高斯数组
	}
 ///////////////////////////
 //用上面产生的一维高斯滤波器Kernel[i]平滑灰度图像Gray
  double dDotMul;//点乘
  long x,y;
  int nlen;
  nlen=WindowSize/2;
  //滤波系数总和
  double dWeightSum;
  double *Temp=NULL;//中间暂时寄存器
  double *Gray=NULL;//灰度图
  double *Result=NULL;//平滑后图像
  Temp=(double*)malloc(bmp.line_width*bmp.height*sizeof(double));
  Gray=(double*)malloc(bmp.line_width*bmp.height*sizeof(double));
  Result=(double*)malloc(bmp.line_width*bmp.height*sizeof(double));
  //把bmp图像数据区部分拷到Gray
 // memcpy(Gray,bmp.ptr,line_width*height*sizeof(BYTE));

  for(y=0;y<bmp.height;y++)
	  for(x=0;x<bmp.width;x++)
		  Gray[y*bmp.line_width+x]=(double)bmp.ptr[y*bmp.line_width+x];
  //x方向滤波
  for(y=0;y<bmp.height;y++)
  {
  for(x=0;x<bmp.width;x++)
  {
	dDotMul=0;
	dWeightSum=0;
	
	for(i=(-nlen);i<=nlen;i++)
	{
		if((i+x)>=0&&(i+x)<bmp.width)
		{
			dDotMul+=(double)Gray[y*bmp.line_width+x+i]*Kernel[nlen+i];
			dWeightSum+=Kernel[i-nlen];
		}
	}
	Temp[y*bmp.line_width+x]=dDotMul/dWeightSum;
  }
  }
  //y方向滤波
  for(x=0;x<bmp.width;x++)
  {
	for(y=0;y<bmp.height;y++)
	{
		dDotMul=0;
		dWeightSum=0;
		for(i=(-nlen);i<nlen;i++)
		{
			if((i+y)>=0&&(i+y)<bmp.height)
			{
				dDotMul+=(double)Temp[(y+i)*bmp.line_width+x]*Kernel[nlen+i];
				dWeightSum+=Kernel[nlen+i];
			}
		}
		Result[y*bmp.height+x]=(unsigned char)dDotMul/dWeightSum;

	}
  }
  delete []Kernel;
  Kernel=NULL;

  delete []Temp;
  Temp=NULL;
 //方向导数，求梯度//////
  
  int *Gradx=(int *)malloc(bmp.height*bmp.line_width*sizeof(int));//x方向梯度
  int *Grady=(int *)malloc(bmp.height*bmp.line_width*sizeof(int));//y方向梯度
  int *Mag  =(int *)malloc(bmp.height*bmp.line_width*sizeof(int));//梯度的幅值

  //x方向的方向导数

  for(y=1;y<bmp.height-1;y++)
	 { 
		 for(x=1;x<bmp.width-1;x++)
		 {
			Gradx[y*bmp.line_width+x]=(int)(Result[y*bmp.line_width+x+1]-Result[y*bmp.line_width+x-1]);
		 }
   }

  //y方向的方向导数

  for(x=1;x<bmp.width;x++)
	  {
		  for(y=1;y<bmp.height;y++)
		  {
		   Grady[y*bmp.line_width+x]=(int)(Result[(y+1)*bmp.line_width+x]-Result[(y-1)*bmp.line_width+x]);
		  }
       }
  //求梯度
  //中间变量
  double dSqt1;
  double dSqt2;

  for(y=0;y<bmp.height;y++)
	  {
		  for(x=0;x<bmp.width;x++)
		  {
			//二阶范数求梯度
			  dSqt1=Gradx[y*bmp.line_width+x]*Gradx[y*bmp.line_width+x];
			  dSqt2=Grady[y*bmp.line_width+x]*Grady[y*bmp.line_width+x];
			  Mag[y*bmp.line_width+x]=(int)(sqrt(dSqt1+dSqt2)+0.5);
		  }
      }
 
  //非最大抑制

  BYTE* nResult=(BYTE*)malloc(bmp.line_width*bmp.height*sizeof(BYTE));
  int nPos;//当前位置
  int gx;//x方向的梯度
  int gy;//y方向的梯度
  //中间变量
  int g1,g2,g3,g4;
  double weight;
  double dTemp,dTemp1,dTemp2;


  //设置图像边缘为不可能的分界点
  for(x=0;x<bmp.width;x++)
  {
  nResult[x]=0;                                 //第一行
  nResult[(bmp.height-1)*bmp.line_width+x]=0;   //最后一行
  }
  for(y=0;y<bmp.height;y++)
  {
  nResult[y*bmp.line_width]=0;                  //第一列
  nResult[y*bmp.line_width+bmp.line_width-1]=0; //最后一列
  }

  for(y=1;y<bmp.height-1;y++)
	{  for(x=1;x<bmp.width-1;x++)
	  {
	     //当前点
		  nPos=y*bmp.line_width+x;
		  if(Mag[nPos]==0)         //梯度幅值为0
			  nResult[nPos]=0;

		  else                      //幅值非零
		  {
				//当前点的梯度幅度
			  dTemp=Mag[nPos];
			  //x,y方向导数
			  gx=Gradx[nPos];
			  gy=Grady[nPos];
			  //如果方向倒数y分量比x分量大，说明导数方向趋向于y分量
			  if(abs(gy)>abs(gx))
			  {
				weight=abs(gx)/abs(gy);
				g2=Mag[nPos-bmp.line_width];
				g4=Mag[nPos+bmp.line_width];
				if(gx*gy>0)
				{
					g1=Mag[nPos-bmp.line_width-1];
					g3=Mag[nPos+bmp.line_width+1];
				}
			  
			  //如果想x,y两个方向的方向相反
			//C是当前像素，与g1-g4的关系为：
			//g2  g1
			//C
			//g3  g4
				else
				{
				g1=Mag[nPos-bmp.line_width+1];
				g3=Mag[nPos+bmp.line_width-1];
				}
			  }
			  //如果方向导师x分量比y分量大
			  else
			  {
				//插值比例
				  weight=abs(gy)/abs(gx);
				  g2=Mag[nPos+1];
				  g4=Mag[nPos-1];
				  //如果x，y两个方向的方向导数符号相同
				  //当前像素C与g1-g4的关系为
				  //g3
				  //g4  C g2
				  //g1
				  if(gx*gy>0)
				  {
					g1=Mag[nPos+bmp.line_width+1];
					g3=Mag[nPos-bmp.line_width-1];
				  }
				  else
				  {
					g1=Mag[nPos-bmp.line_width+1];
					g3=Mag[nPos+bmp.line_width-1];
				  
				  }
			  }
			  //利用g1-g4对梯度进行插值
			  {
					dTemp1=weight*g1+(1-weight)*g2;
					dTemp2=weight*g3+(1-weight)*g4;

					//当前像素的梯度是局部的最大值
					//该点可能是边界点
					if(dTemp>=dTemp1&&dTemp>=dTemp2)
					{
						nResult[nPos]=128;
					}
					else
					{
						nResult[nPos]=0;//不可能是边界点
					}
			  }
		  
		  }
	  }
  }
///////统计pMag的直方图，判定阈值////////
	  long k;
	  int nHist[256];//直方图数组
	  int nEdgeNum;
	  int nMaxMag;
	  int nHighCount;
	  nMaxMag=0;
	  double dRatHigh=0.8;
	  double dRatLow=0.4;
	  //初始化
	  for(k=0;k<256;k++)
	  {
	  nHist[k]=0;
	  }
   //统计直方图，利用直方图计算阈值
	  for(y=0;y<bmp.height;y++)
	   {
		  for(x=0;x<bmp.width;x++)
		  {
			  if(nResult[y*bmp.line_width+x]==128)
				  {
					nHist[Mag[y*bmp.line_width+x]]++;
				  }
		  }
	  }

	  nEdgeNum=nHist[0];
	  nMaxMag=0;
	  //统计经过“非最大值抑制”后有多少像素
	  for(k=1;k<256;k++)
	  {
		if(nHist[k]!=0)
		{
			nMaxMag=k;
		}
		nEdgeNum+=nHist[k];
	  }
   //梯度比高阈值小的像素点总数目
	  nHighCount=(int)(dRatHigh*nEdgeNum+0.5);

	  k=1;
	  nEdgeNum=nHist[1];
	  //计算高阈值
	  while((k<(nMaxMag-1))&&(nEdgeNum<nHighCount))
	  {
		k++;
		nEdgeNum+=nHist[k];
	  }
   int nThrHigh, nThrLow;
   nThrHigh=k;
   nThrLow=(int)((nThrHigh)*dRatLow+0.5);

   //利用函数寻找边界起点
   for(y=0;y<bmp.height;y++)
   {
		for(x=0;x<bmp.width;x++)
		{
			nPos=y*bmp.line_width+x;
			if((nResult[nPos]==128)&&(Mag[nPos]>=nThrHigh))
			{
				nResult[nPos]=255;
                //以该点为中心再进行跟踪 
               TraceEdge(y,x,nThrLow,nResult,Mag,bmp); 
			}
		}
   }

   //其他的点已经不可能
   for(y=0;y<bmp.height;y++)
   {
		for(x=0;x<bmp.width;x++)
		{
			nPos=y*bmp.line_width+x;
			if(nResult[nPos]!=255)
			{
			   nResult[nPos]=0;

			}
		}
   }
   //覆盖源图像数据区
   nPos=0;
   BYTE t=0;
   for(y=0;y<bmp.height;y++)
	{   for(x=0;x<bmp.width;x++)
	   {
			t=nResult[nPos];
			SET_gray(bmp,y,x,t);
			nPos++;
	   }
   
   }
   delete []Gray;
    Gray=NULL;
   delete []Gradx;
   Gradx=NULL;
   delete []Grady;
   Grady=NULL;
   delete []Mag;
   Mag=NULL;
   delete []nResult;
   nResult=NULL;


 //保存
 // if((fp=fopen("dst.bmp","wb"))=NULL)
 //  {
	//printf("fail to save");
	//return 0;
 //  }
 // w=19778;
 // fwrite(&w,2,1,fp);
 // dw=14;
 // fwrite(&dw,4,1,fp);
 // w=0;
 // fwrite(&w,2,1,fp);
 // fwrite(&w,2,1,fp);
 // if(bmp.bit_count==8) dw=1078;
 // else dw=54;
 // fwrite(&dw,4,1,fp);



 // dw=40;
 // fwrite(&dw,4,1,fp);
 // dw=bmp.width;
 // fwrite(&dw,4,1,fp);
 // dw=bmp.height;
 // fwrite(&dw,4,1,fp);
 // w=0;
 // fwrite(&w,2,1,fp);
 // w = bmp.bit_count;
 // fwrite( &w, 2, 1, fp );//写入位数
 // dw = 0;
 // fwrite( &dw, 4, 1, fp );
 // dw = bmp.height * ( bmp.width * bmp.bit_count + 31 ) / 32 * 4;//求出像素区的大小
 // fwrite( &dw, 4, 1, fp );//写入像素区的大小的值
 // dw = 0;
 // fwrite( &dw, 4, 1, fp );
 // fwrite( &dw, 4, 1, fp );
 // fwrite( &dw, 4, 1, fp );
 // fwrite( &dw, 4, 1, fp );

 // if( bmp.palette != 0 )
	//{
	//    fwrite( bmp.palette, 1024, 1, fp );
	//}

 // dw = bmp.height * ( bmp.width * bmp.bit_count + 31 ) / 32 * 4;
 // fwrite( bmp.ptr, dw, 1, fp );//写入图像部分

 // fclose(fp);
 // 
 // 
 // return 1;
}