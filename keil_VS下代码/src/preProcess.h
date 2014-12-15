#ifndef __PREPROCESS_H__
#define __PREPROCESS_H__

#include"header.h"
//返回FALSE,则需要舍弃图片；返回TRUE，可继续处理
int preProcess(RGBTYPE* dst, RGBTYPE* src);

#endif
