#include"header.h"
extern float map1[R][C];
extern float map2[R][C];
extern RGBTYPE image_RGB[SIZE];
extern RGBTYPE image_Correction[SIZE];
void undistort_map(){
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
			inter1 = pix_inter(image_RGB[y*C+x],image_RGB[y*C+x+1],t);
			inter2 = pix_inter(image_RGB[(y+1)*C+x],image_RGB[(y+1)*C+x+1],t);
			image_Correction[i*C+j] = pix_inter(inter1,inter2,u);
		}
	//以下是插值
	//for(i = 5; i <= R-5; i = i+3)
	//	for(j = 5; j <= C-5; j = j+3){
	//		inter1 = pix_div(pix_add(image_Correction[(i-1)*C+j-1],image_Correction[(i-1)*C+j+1]),2);
	//		inter2 = pix_div(pix_add(image_Correction[(i+1)*C+j-1],image_Correction[(i+1)*C+j+1]),2);
	//		image_Correction[i*C+j] = pix_div(pix_add(inter1,inter2),2);
	//	}
}
