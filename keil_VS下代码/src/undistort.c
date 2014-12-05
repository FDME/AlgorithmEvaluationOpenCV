#include"header.h"
extern UINT8T map1[R][C];
extern UINT8T map2[R][C];
extern RGBTYPE image_RGB[SIZE];
extern RGBTYPE image_undis[SIZE];
void undistort_map(){
	int i,j;
	int x,y;
	for(i = 0; i != R; i++)
		for(j = 0; j != C; j++){
			x = map1[i][j];
			y = map2[i][j];
			image_undis[i*R+j] = image_RGB[y*R+x];
		}
}
