#include"header.h"
extern float map1[R][C];
extern float map2[R][C];
extern RGBTYPE image_RGB[SIZE];
extern RGBTYPE image_Correction[SIZE];
void undistort_map(RGBTYPE* img);