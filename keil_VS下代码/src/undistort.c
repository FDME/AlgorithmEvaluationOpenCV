#include"header.h"
extern float map1[R][C];
extern float map2[R][C];

void undistort_map(){
	UINT32T i,j;
	UINT32T x, y;
	for(i = 0; i != R; i++)
		for(j = 0; j != C; j++){
			x = map1[i][j];
			y = map2[i][j];
			image_Correction[i*C+j] = image_RGB[y*C+x];
			if (i == 400 && j == 200){
				printf("map1[400][200] = %d, map2[400][200] = %d\n", map1[400][200], map2[400][200]);
				printf("x = %d, y = %d\n", x, y);
			}
				

		}

}
