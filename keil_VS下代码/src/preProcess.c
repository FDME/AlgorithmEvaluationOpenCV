#include"preProcess.h"
#include"retinex.h"

void preProcess(RGBTYPE* dst, RGBTYPE* src)
{
	retinex(dst, src, 30, 1);
}