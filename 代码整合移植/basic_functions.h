#ifndef __BASIC_FUNCTIONS_H__
#define __BASIC_FUNCTIONS_H__
#include "header.h"
void calc_gray();
void calc_integral(UINT32T* integral, UINT8T* image);
void calc_gaussian_5x5(UINT8T* dst, UINT8T* src);// 快速5*5高斯模糊
void calc_sobel_3x3(UINT8T* dst, UINT8T* src);
UINT32T Mean(UINT8T* image, UINT32T x_start, UINT32T y_start, UINT32T width, UINT32T height);
UINT32T Variance(UINT8T* image, UINT32T x_start, UINT32T y_start, UINT32T width, UINT32T height, UINT32T mean);
#endif