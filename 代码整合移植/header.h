#define	INT8T  char
#ifdef WIN32
#include <stdio.h>
#include <opencv\cv.h>
#include<opencv\highgui.h>
#endif

//Types
#define UINT32T unsigned int
#define INT32T	int
#define UINT16T unsigned short
#define INT16T	short int
#define UINT8T  unsigned char

//struct _Point
//{
//	INT16T x, y;
//};
//typedef struct _Point POINT;

//Constants
#define R 640
#define C 480
#define SIZE (R*C)
#define NUM_LINE 300
#define NUM_RESULT 30

//Functions
void SystemInit(); 
void ImageImport();
void Undistort();
void Preprocess();
void LineDetection();
void LineSort();
void ForegroundSeperation();
void Canny();
void ImageSegmention();
void LabelOptimization();
void SpaceDetection();

//functions for test
void LoadImage();
void ShowImage();
void ShowLabel();