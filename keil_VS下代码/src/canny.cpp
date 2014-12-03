#include"canny.h"

////////////ȫ�ֱ���/////

Bitmap bmp;
FILE *fp=0;
DWORD width =0,height=0,line_width=0;
WORD bit_count =0,pal_length=0,off_bits=0;
DWORD dw = 0;
WORD  w = 0;

//////�Է�������ƺ��ͼ����б�Ե����/////////

void TraceEdge(int y, int x, int nThrLow, LPBYTE pResult, int *pMag, Bitmap sz) 
{ 
//��8�������ؽ��в�ѯ 
int xNum[8] = {1,1,0,-1,-1,-1,0,1}; 
int yNum[8] = {0,1,1,1,0,-1,-1,-1}; 

long yy,xx,k; 

for(k=0;k<8;k++) 
{ 
yy = y+yNum[k]; 
xx = x+xNum[k]; 

if(pResult[yy*sz.line_width+xx]==128 && pMag[yy*sz.line_width+xx]>=nThrLow ) 
{ 
//�õ���Ϊ�߽�� 
pResult[yy*sz.line_width+xx] = 255; 

//�Ըõ�Ϊ�����ٽ��и��� 
TraceEdge(yy,xx,nThrLow,pResult,pMag,sz); 
} 
} 
} 


int main()
{
//////��ȡͼ����� /////
  
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

  ///////////////////��˹�˲�///////////////////////

  int nCenter;   //�������ĵ�
  double dDis;     //������һ�㵽���ĵ�ľ���
  DWORD i;

  double dValue;    // �м����
  double dSum=0.0;

  double sigma=0.8;//��˹ϵ��
  //[-3*sigma,3*sigma]���ڵ����ݣ��Ḳ�Ǿ��󲿷��˲�ϵ����Ҳ����3��٤��ԭ��
  int WindowSize=1+2*ceil(3*sigma);
  nCenter=WindowSize/2;
  double*Kernel=NULL;                          //��˹������
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
		Kernel[i]/=dSum;                  //��һ����˹����
	}
 ///////////////////////////
 //�����������һά��˹�˲���Kernel[i]ƽ���Ҷ�ͼ��Gray
  double dDotMul;//���
  long x,y;
  int nlen;
  nlen=WindowSize/2;
  //�˲�ϵ���ܺ�
  double dWeightSum;
  double *Temp=NULL;//�м���ʱ�Ĵ���
  double *Gray=NULL;//�Ҷ�ͼ
  double *Result=NULL;//ƽ����ͼ��
  Temp=(double*)malloc(bmp.line_width*bmp.height*sizeof(double));
  Gray=(double*)malloc(bmp.line_width*bmp.height*sizeof(double));
  Result=(double*)malloc(bmp.line_width*bmp.height*sizeof(double));
  //��bmpͼ�����������ֿ���Gray
 // memcpy(Gray,bmp.ptr,line_width*height*sizeof(BYTE));

  for(y=0;y<bmp.height;y++)
	  for(x=0;x<bmp.width;x++)
		  Gray[y*bmp.line_width+x]=(double)bmp.ptr[y*bmp.line_width+x];
  //x�����˲�
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
  //y�����˲�
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
 //�����������ݶ�//////
  
  int *Gradx=(int *)malloc(bmp.height*bmp.line_width*sizeof(int));//x�����ݶ�
  int *Grady=(int *)malloc(bmp.height*bmp.line_width*sizeof(int));//y�����ݶ�
  int *Mag  =(int *)malloc(bmp.height*bmp.line_width*sizeof(int));//�ݶȵķ�ֵ

  //x����ķ�����

  for(y=1;y<bmp.height-1;y++)
	 { 
		 for(x=1;x<bmp.width-1;x++)
		 {
			Gradx[y*bmp.line_width+x]=(int)(Result[y*bmp.line_width+x+1]-Result[y*bmp.line_width+x-1]);
		 }
   }

  //y����ķ�����

  for(x=1;x<bmp.width;x++)
	  {
		  for(y=1;y<bmp.height;y++)
		  {
		   Grady[y*bmp.line_width+x]=(int)(Result[(y+1)*bmp.line_width+x]-Result[(y-1)*bmp.line_width+x]);
		  }
       }
  //���ݶ�
  //�м����
  double dSqt1;
  double dSqt2;

  for(y=0;y<bmp.height;y++)
	  {
		  for(x=0;x<bmp.width;x++)
		  {
			//���׷������ݶ�
			  dSqt1=Gradx[y*bmp.line_width+x]*Gradx[y*bmp.line_width+x];
			  dSqt2=Grady[y*bmp.line_width+x]*Grady[y*bmp.line_width+x];
			  Mag[y*bmp.line_width+x]=(int)(sqrt(dSqt1+dSqt2)+0.5);
		  }
      }
 
  //���������

  BYTE* nResult=(BYTE*)malloc(bmp.line_width*bmp.height*sizeof(BYTE));
  int nPos;//��ǰλ��
  int gx;//x������ݶ�
  int gy;//y������ݶ�
  //�м����
  int g1,g2,g3,g4;
  double weight;
  double dTemp,dTemp1,dTemp2;


  //����ͼ���ԵΪ�����ܵķֽ��
  for(x=0;x<bmp.width;x++)
  {
  nResult[x]=0;                                 //��һ��
  nResult[(bmp.height-1)*bmp.line_width+x]=0;   //���һ��
  }
  for(y=0;y<bmp.height;y++)
  {
  nResult[y*bmp.line_width]=0;                  //��һ��
  nResult[y*bmp.line_width+bmp.line_width-1]=0; //���һ��
  }

  for(y=1;y<bmp.height-1;y++)
	{  for(x=1;x<bmp.width-1;x++)
	  {
	     //��ǰ��
		  nPos=y*bmp.line_width+x;
		  if(Mag[nPos]==0)         //�ݶȷ�ֵΪ0
			  nResult[nPos]=0;

		  else                      //��ֵ����
		  {
				//��ǰ����ݶȷ���
			  dTemp=Mag[nPos];
			  //x,y������
			  gx=Gradx[nPos];
			  gy=Grady[nPos];
			  //���������y������x������˵����������������y����
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
			  
			  //�����x,y��������ķ����෴
			//C�ǵ�ǰ���أ���g1-g4�Ĺ�ϵΪ��
			//g2  g1
			//C
			//g3  g4
				else
				{
				g1=Mag[nPos-bmp.line_width+1];
				g3=Mag[nPos+bmp.line_width-1];
				}
			  }
			  //�������ʦx������y������
			  else
			  {
				//��ֵ����
				  weight=abs(gy)/abs(gx);
				  g2=Mag[nPos+1];
				  g4=Mag[nPos-1];
				  //���x��y��������ķ�����������ͬ
				  //��ǰ����C��g1-g4�Ĺ�ϵΪ
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
			  //����g1-g4���ݶȽ��в�ֵ
			  {
					dTemp1=weight*g1+(1-weight)*g2;
					dTemp2=weight*g3+(1-weight)*g4;

					//��ǰ���ص��ݶ��Ǿֲ������ֵ
					//�õ�����Ǳ߽��
					if(dTemp>=dTemp1&&dTemp>=dTemp2)
					{
						nResult[nPos]=128;
					}
					else
					{
						nResult[nPos]=0;//�������Ǳ߽��
					}
			  }
		  
		  }
	  }
  }
///////ͳ��pMag��ֱ��ͼ���ж���ֵ////////
	  long k;
	  int nHist[256];//ֱ��ͼ����
	  int nEdgeNum;
	  int nMaxMag;
	  int nHighCount;
	  nMaxMag=0;
	  double dRatHigh=0.8;
	  double dRatLow=0.4;
	  //��ʼ��
	  for(k=0;k<256;k++)
	  {
	  nHist[k]=0;
	  }
   //ͳ��ֱ��ͼ������ֱ��ͼ������ֵ
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
	  //ͳ�ƾ����������ֵ���ơ����ж�������
	  for(k=1;k<256;k++)
	  {
		if(nHist[k]!=0)
		{
			nMaxMag=k;
		}
		nEdgeNum+=nHist[k];
	  }
   //�ݶȱȸ���ֵС�����ص�����Ŀ
	  nHighCount=(int)(dRatHigh*nEdgeNum+0.5);

	  k=1;
	  nEdgeNum=nHist[1];
	  //�������ֵ
	  while((k<(nMaxMag-1))&&(nEdgeNum<nHighCount))
	  {
		k++;
		nEdgeNum+=nHist[k];
	  }
   int nThrHigh, nThrLow;
   nThrHigh=k;
   nThrLow=(int)((nThrHigh)*dRatLow+0.5);

   //���ú���Ѱ�ұ߽����
   for(y=0;y<bmp.height;y++)
   {
		for(x=0;x<bmp.width;x++)
		{
			nPos=y*bmp.line_width+x;
			if((nResult[nPos]==128)&&(Mag[nPos]>=nThrHigh))
			{
				nResult[nPos]=255;
                //�Ըõ�Ϊ�����ٽ��и��� 
               TraceEdge(y,x,nThrLow,nResult,Mag,bmp); 
			}
		}
   }

   //�����ĵ��Ѿ�������
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
   //����Դͼ��������
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


 //����
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
 // fwrite( &w, 2, 1, fp );//д��λ��
 // dw = 0;
 // fwrite( &dw, 4, 1, fp );
 // dw = bmp.height * ( bmp.width * bmp.bit_count + 31 ) / 32 * 4;//����������Ĵ�С
 // fwrite( &dw, 4, 1, fp );//д���������Ĵ�С��ֵ
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
 // fwrite( bmp.ptr, dw, 1, fp );//д��ͼ�񲿷�

 // fclose(fp);
 // 
 // 
 // return 1;
}