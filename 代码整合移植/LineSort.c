#include<stdio.h>
#include"LineSort.h"

int i_ls, j_ls;
int f_lo[MAXLINE];
int v[MAXLINE];
double temp_ls;
int maxLine;
int l_ls;
int tot_ls;
#ifdef WIN32
IplImage *imageLines;
UINT8T pGray[SIZE];
CvPoint st;
CvPoint en;
#endif
void Swap(double *a, double *b){
	temp_ls = *a;
	*a = *b;
	*b = temp_ls;
}

#ifdef WIN32
void ShowLines(int n, double *line_k, double *line_b){
	imageLines = cvCreateImageHeader(cvSize(C, R), IPL_DEPTH_8U, 1);
	memset(pGray, 0, SIZE);
	cvSetData(imageLines, pGray, C);

	for (i_ls = 0; i_ls < n; i_ls++){
		st = cvPoint(0, (int)line_b[i_ls]);
		en = cvPoint(C, (int)(C * line_k[i_ls] + line_b[i_ls]));
		cvLine(imageLines, st, en, CV_RGB(0, 0, 255), 1, CV_AA, 0);
	}
	cvNamedWindow("Lines", 0);
	cvShowImage("Lines", imageLines);
	cvWaitKey(0);
}
#endif
int LineSort(int n, double *line_k, double *line_b){
#ifdef WIN32
	ShowLines(n, line_k, line_b);
#endif
	for (i_ls = 0; i_ls < n; i_ls++) f_lo[i_ls] = 0;
	for (i_ls = 0; i_ls < n; i_ls++) v[i_ls] = 0;
	for (i_ls = 0; i_ls < n; i_ls++)
	for (j_ls = i_ls + 1; j_ls < n; j_ls++) if (line_b[i_ls] > line_b[j_ls]){
		Swap(&line_b[i_ls], &line_b[j_ls]);
		Swap(&line_k[i_ls], &line_k[j_ls]);
	}
	//for (i_ls = n - 1; i_ls >= 0; i_ls--) printf("b = %.3lf, k = %.3lf\n", line_b[i_ls], line_k[i_ls]);
	for (i_ls = n - 1; i_ls >= 0; i_ls--){
		for (j_ls = n - 1; j_ls > i_ls; j_ls--) if (line_k[i_ls] > line_k[j_ls])
		if (f_lo[j_ls] + 1 > f_lo[i_ls]){
			f_lo[i_ls] = f_lo[j_ls] + 1;
			v[i_ls] = j_ls;
		}
		if (f_lo[i_ls] == 0) {
			f_lo[i_ls] = 1;
			v[i_ls] = i_ls;
		}
	}
	maxLine = 0;
	for (i_ls = 0; i_ls < n; i_ls++) if (maxLine < f_lo[i_ls]){
		maxLine = f_lo[i_ls];
		l_ls = i_ls;
	}

	tot_ls = 0;
	while (1){
		line_b[tot_ls] = line_b[l_ls];
		line_k[tot_ls] = line_k[l_ls];
		tot_ls++;
		l_ls = v[l_ls];
		if (l_ls == v[l_ls]) break;
	}
#ifdef WIN32
	ShowLines(tot_ls, line_k, line_b);
#endif
	return tot_ls;
}