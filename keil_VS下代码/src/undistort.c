#include "undistort.h"
extern float map1[R][C];
extern float map2[R][C];
extern RGBTYPE image_RGB[SIZE];
extern RGBTYPE image_Correction[SIZE];


//以下是一些像素的运算，我只用到了pix_inter~其他的留着备用，不用可删
RGBTYPE pix_add(RGBTYPE a, RGBTYPE b);
RGBTYPE pix_div(RGBTYPE a, int n);
RGBTYPE pix_mul(RGBTYPE a, float t);
RGBTYPE pix_inter(RGBTYPE t1, RGBTYPE t2, float t);

void undistort_map(RGBTYPE*src, RGBTYPE*dst){
	int i,j;
	float fx,fy;//映射后的浮点坐标
	int x,y;
	float t,u;
	RGBTYPE inter1,inter2;//双线性插值
	for(i = 0; i != R; i++)
		for(j = 0; j != C; j++){
			fx = map1[i][j];
			fy = map2[i][j];
			x = (int)fx; y = (int)fy; //向下取整
			t = fx -x; u = fy-y;
			//printf("%f,%f",t,u);
			inter1 = pix_inter(src[y*C+x],src[y*C+x+1],t);
			inter2 = pix_inter(src[(y+1)*C+x],src[(y+1)*C+x+1],t);
			dst[i*C+j] = pix_inter(inter1,inter2,u);
		}
}