#ifndef __CANNY_H__
#define __CANNY_H__
#include"header.h"
int canny(UINT8T* dst, UINT8T* src, int option); // option = 0: Otsu，忽略细小边缘。option = 1 : 统计细小边缘
#endif