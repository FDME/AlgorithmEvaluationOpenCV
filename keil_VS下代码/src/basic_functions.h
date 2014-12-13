#ifndef __BASIC_FUNCTIONS_H__
#define __BASIC_FUNCTIONS_H__
#include "header.h"
void calc_gray(UINT8T* Gray, RGBTYPE* RGB);
void calc_integral(UINT32T* integral, UINT8T* image);
void calc_gaussian_5x5(UINT8T* dst, UINT8T* src);// ����5*5��˹ģ��
void calc_sobel_3x3(UINT8T* dst, UINT8T* src);
#endif