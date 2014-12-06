//插值遇到的运算
#include"header.h"
//RGBTYPE pix_add(RGBTYPE a, RGBTYPE b){
//	RGBTYPE s;
//	s.r = a.r + b.r;
//	s.g = a.g + b.g;
//	s.b = a.b + b.b;
//	return s;
//}
//
//RGBTYPE pix_div(RGBTYPE a, int n){
//	RGBTYPE s;
//	s.r = a.r/n;
//	s.g = a.g/n;
//	s.b = a.b/n;
//	return s;
//}
//
//RGBTYPE pix_mul(RGBTYPE a, float t){
//	RGBTYPE s;
//	s.r = a.r*t;
//	s.g = a.g*t;
//	s.b = a.b*t;
//	return s;
//}

RGBTYPE pix_inter(RGBTYPE t1, RGBTYPE t2, float t){
	RGBTYPE s;
	float r,g,b;
	s.r = t1.r*(1.0-t) + t2.r*t;
	s.g = t1.g*(1.0-t) + t2.g*t;
	s.b = t1.b*(1.0-t) + t2.b*t;
	return s;
}

