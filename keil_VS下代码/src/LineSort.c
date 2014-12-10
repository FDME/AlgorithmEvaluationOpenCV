#include<stdio.h>
#include"LineSort.h"

int i, j;
int f[MAXLINE];
int v[MAXLINE];
double temp;
int maxLine;
int l;
int tot;

IplImage *imageLines;
UINT8T pGray[SIZE];
CvPoint st;
CvPoint en;

void Swap(double *a, double *b){
	temp = *a;
	*a = *b;
	*b = temp;
}

void ShowLines(int n, double *line_k, double *line_b){
	imageLines = cvCreateImageHeader(cvSize(C, R), IPL_DEPTH_8U, 1);
	memset(pGray, 0, SIZE);
	cvSetData(imageLines, pGray, C);

	for (i = 0; i < n; i++){
		st = cvPoint(0, (int)line_b[i]);
		en = cvPoint(C, (int)(C * line_k[i] + line_b[i]));
		cvLine(imageLines, st, en, CV_RGB(0, 0, 255), 1, CV_AA, 0);
	}
	cvNamedWindow("Lines", 0);
	cvShowImage("Lines", imageLines);
	cvWaitKey(0);
}

int LineSort(int n, double *line_k, double *line_b){
	ShowLines(n, line_k, line_b);
	for (i = 0; i < n; i++) f[i] = 0;
	for (i = 0; i < n; i++) v[i] = 0;
	for (i = 0; i < n; i++)
	for (j = i + 1; j < n; j++) if (line_b[i] > line_b[j]){
		Swap(&line_b[i], &line_b[j]);
		Swap(&line_k[i], &line_k[j]);
	}
	//for (i = n - 1; i >= 0; i--) printf("b = %.3lf, k = %.3lf\n", line_b[i], line_k[i]);
	for (i = n - 1; i >= 0; i--){
		for (j = n - 1; j > i; j--) if (line_k[i] > line_k[j])
		if (f[j] + 1 > f[i]){
			f[i] = f[j] + 1;
			v[i] = j;
		}
		if (f[i] == 0) {
			f[i] = 1;
			v[i] = i;
		}
	}
	maxLine = 0;
	for (i = 0; i < n; i++) if (maxLine < f[i]){
		maxLine = f[i];
		l = i;
	}

	tot = 0;
	while (1){
		line_b[tot] = line_b[l];
		line_k[tot] = line_k[l];
		tot++;
		l = v[l];
		if (l == v[l]) break;
	}
	ShowLines(tot, line_k, line_b);
	return tot;
}