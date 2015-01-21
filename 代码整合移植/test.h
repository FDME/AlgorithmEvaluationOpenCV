#ifndef __TEST_H__
#define __TEST_H__

#include"header.h"
#ifdef WIN32
int filesize(FILE *fp);
void showImage_RGB(UINT8T* r, UINT8T* g, UINT8T* b, const char* name);
void showImage_1ch(UINT8T* p_1ch, const char* name);
#endif
void logStr( char* str);
#endif
