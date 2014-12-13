#include "basic_functions.h"


// 计算灰度  //yuv分量中的y即可
// gray[R][C]
// color[][]
void calc_gray(UINT8T* Gray, RGBTYPE* RGB)
{
	UINT32T  col;
	UINT32T  row;
	UINT32T  tmp;
  
	for (row=0;row<R;row++)
  {
    for (col=0;col<C;col++)
    {
      //tmp = 19595*color[row+col*C]->r + 38469*color[row+col*C]->g + 7471*color[row+col*C]->b;
              tmp = 19595*RGB[row*C+col].r+38469*RGB[row*C+col].g + 7471*RGB[row*C+col].b;
      //gray[row][col] = (UINT8T)((tmp>>16)&0x000000FF);
              Gray[row*C+col] = (UINT8T)((tmp>>16)&0x000000FF);
    }
  }  
}


// 计算积分图
//
// 
void calc_integral(UINT32T* integral, UINT8T* image)
{
	UINT32T	    	col;
	UINT32T	    	row;
	UINT32T       tmp;
  
  // first element
	integral[0] = image[0];

  // first row	
	for (col=1;col<C;col++)
  {
    integral[col] = integral[col-1] + image[col];
  }
  
	for (row=1;row<R;row++)
  {
    integral[row*C] = integral[(row-1)*C] + image[row*C];
    
    for (col=1;col<C;col++)
    {
      tmp = integral[(row-1)*C+col] + integral[row*C+col-1] - integral[(row-1)*C+col-1]; 
      integral[row*C+col] = tmp + image[row*C+col];
    }
  }  
}



// 高斯滤波 - gaussian 5x5
//  (1/273) *
//  	1   4   7   4   1
//  	4  16  26  16   4
//  	7  26  41  26   7
//  	4  16  26  16   4
//  	1   4   7   4   1
void calc_gaussian_5x5(UINT8T* dst, UINT8T* src)
{
	UINT32T	    	col;
	UINT32T	    	row;
	UINT32T       k;
	UINT32T       tmp;

	UINT8T	    	rect5x5[25];
	double	    	pt_acc;
	UINT8T	    	pt_n_val;
  
	for (row=2;row<(R-2);row++)
  {
    for (col=2;col<(C-2);col++)
    {
      for (k=0;k<5;k++)  // 5 row
      {
        // 每行5个元素
        rect5x5[5*k+0] = src[(row-2+k)*C+(col-2)+0];
        rect5x5[5*k+1] = src[(row-2+k)*C+(col-2)+1];
        rect5x5[5*k+2] = src[(row-2+k)*C+(col-2)+2];
        rect5x5[5*k+3] = src[(row-2+k)*C+(col-2)+3];
        rect5x5[5*k+4] = src[(row-2+k)*C+(col-2)+4];
      }
      
      // 可以定点化
    	pt_acc	    =   (double)(rect5x5[0*5+0]) * (1.0/273.0) * (1);
    	pt_acc	    +=  (double)(rect5x5[0*5+1]) * (1.0/273.0) * (4);
    	pt_acc	    +=  (double)(rect5x5[0*5+2]) * (1.0/273.0) * (7);
    	pt_acc	    +=  (double)(rect5x5[0*5+3]) * (1.0/273.0) * (4);
    	pt_acc	    +=  (double)(rect5x5[0*5+4]) * (1.0/273.0) * (1);

    	pt_acc	    +=  (double)(rect5x5[1*5+0]) * (1.0/273.0) * (4);
    	pt_acc	    +=  (double)(rect5x5[1*5+1]) * (1.0/273.0) * (16);
    	pt_acc	    +=  (double)(rect5x5[1*5+2]) * (1.0/273.0) * (26);
    	pt_acc	    +=  (double)(rect5x5[1*5+3]) * (1.0/273.0) * (16);
    	pt_acc	    +=  (double)(rect5x5[1*5+4]) * (1.0/273.0) * (4);

    	pt_acc	    +=  (double)(rect5x5[2*5+0]) * (1.0/273.0) * (7);
    	pt_acc	    +=  (double)(rect5x5[2*5+1]) * (1.0/273.0) * (26);
    	pt_acc	    +=  (double)(rect5x5[2*5+2]) * (1.0/273.0) * (41);
    	pt_acc	    +=  (double)(rect5x5[2*5+3]) * (1.0/273.0) * (26);
    	pt_acc	    +=  (double)(rect5x5[2*5+4]) * (1.0/273.0) * (7);

    	pt_acc	    +=  (double)(rect5x5[3*5+0]) * (1.0/273.0) * (4);
    	pt_acc	    +=  (double)(rect5x5[3*5+1]) * (1.0/273.0) * (16);
    	pt_acc	    +=  (double)(rect5x5[3*5+2]) * (1.0/273.0) * (26);
    	pt_acc	    +=  (double)(rect5x5[3*5+3]) * (1.0/273.0) * (16);
    	pt_acc	    +=  (double)(rect5x5[3*5+4]) * (1.0/273.0) * (4);

    	pt_acc	    +=  (double)(rect5x5[4*5+0]) * (1.0/273.0) * (1);
    	pt_acc	    +=  (double)(rect5x5[4*5+1]) * (1.0/273.0) * (4);
    	pt_acc	    +=  (double)(rect5x5[4*5+2]) * (1.0/273.0) * (7);
    	pt_acc	    +=  (double)(rect5x5[4*5+3]) * (1.0/273.0) * (4);
    	pt_acc	    +=  (double)(rect5x5[4*5+4]) * (1.0/273.0) * (1);

    	pt_n_val	  = (UINT8T)(pt_acc > 255 ? 255 : pt_acc);

      dst[row*C+col] = pt_n_val;       
    }
  }
  
  // 处理边界！上下左右各两行拷贝自原图的相应行
	for (row=0;row<R;row++)
  {
    dst[row*C+0]   = src[row*C+0];  // col0
    dst[row*C+1]   = src[row*C+1];  // col1
    
    dst[row*C+C-2] = src[row*C+C-2];
    dst[row*C+C-1] = src[row*C+C-1];  // last col
  }
  
	for (col=0;col<C;col++)
  {
    dst[0*C+col]   = src[0*C+col];
    dst[1*C+col]   = src[1*C+col];
    
    dst[(R-2)*C+col] = src[(R-2)*C+col];
    dst[(R-1)*C+col] = src[(R-1)*C+col];
  }
}



// sobel滤波 - sobel
void calc_sobel_3x3(UINT8T* dst, UINT8T* src)
{
	UINT32T	    	col;
	UINT32T	    	row;
	UINT32T       k;
	UINT32T       tmp;
  
	UINT8T	      	rect3x3[9];
	double	      	pt_dx;
	double	      	pt_dy;
	double	      	pt_dxy;
	UINT8T	      	pt_n_val;
  
	for (row=1;row<(R-1);row++)
  {
    for (col=1;col<(C-1);col++)
    {
      for (k=0;k<3;k++)  // 3 row
      {
        // 每行3个元素
        rect3x3[3*k+0] = src[(row-1+k)*C+(col-1)+0];
        rect3x3[3*k+1] = src[(row-1+k)*C+(col-1)+1];
        rect3x3[3*k+2] = src[(row-1+k)*C+(col-1)+2];
      }
      
      // 可以定点化
    	pt_dx	    =  rect3x3[0*3+0] * (-1);
    	pt_dx	    += rect3x3[0*3+2] * (+1);
    	pt_dx	    += rect3x3[1*3+0] * (-2);
    	pt_dx	    += rect3x3[1*3+2] * (+2);
    	pt_dx	    += rect3x3[2*3+0] * (-1);
    	pt_dx	    += rect3x3[2*3+2] * (+1);
    	pt_dy	    =  rect3x3[0*3+0] * (+1);
    	pt_dy	    += rect3x3[0*3+1] * (+2);
    	pt_dy	    += rect3x3[0*3+2] * (+1);
    	pt_dy	    += rect3x3[2*3+0] * (-1);
    	pt_dy	    += rect3x3[2*3+1] * (-2);
    	pt_dy	    += rect3x3[2*3+2] * (-1);
    	pt_dxy	    =  pt_dx > 0 ? pt_dx : (-pt_dx);
    	pt_dxy	    += pt_dy > 0 ? pt_dy : (-pt_dy);
//    	pt_n_val	  = (uint8_t)(pt_dxy / 8);
//    	pt_n_val	  = (uint8_t)(pt_dxy > 255 ? 255 : pt_dxy);
    	pt_n_val	  = (UINT8T)(pt_dxy > 511 ? 255 : (pt_dxy / 2));

      dst[row*C+col] = pt_n_val;       
    }
  }  

  // 处理边界！上下左右各1行/列拷贝自dst的相邻行/列
	for (row=0;row<R;row++)
  {
    dst[row*C+0]   = dst[row*C+1];  // col0
    dst[row*C+C-1] = dst[row*C+C-2];  // last col
  }
  
	for (col=0;col<C;col++)
  {
    dst[0*C+col]     = dst[1*C+col];
    dst[(R-1)*C+col] = dst[(R-2)*C+col];
  }  
    
}





