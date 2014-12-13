#include <stdio.h>
#include "LabelOptimization.h"

extern int h, w;
extern int numLabels;
INT32T i_lo, j_lo, k_lo, l;
UINT8T label_lo[MAXHEIGHT][MAXWIDTH];
int temp[MAXHEIGHT][MAXWIDTH], block[MAXHEIGHT][MAXWIDTH];
int queue[MAXHEIGHT * MAXWIDTH];
int head, tail;
int numBlocks;
FILE *fp;

void LoadLabel(UINT8T *labelIn){
	for (i_lo = 0; i_lo < h; i_lo++)
	for (j_lo = 0; j_lo < w; j_lo++) label_lo[i_lo][j_lo] = labelIn[i_lo * w + j_lo];
	return;
#ifdef WIN32
	fopen_s(&fp, "Label.txt", "r");
	fscanf_s(fp, "%d %d", &h, &w);
	printf("Image Loaded: %d x %d\n", h, w);

	for (i_lo = 0; i_lo < h; i_lo++)
	for (j_lo = 0; j_lo < w; j_lo++) {
		//printf("%d, %d\n", i_lo, j_lo);
		fscanf_s(fp, "%d", &label_lo[i_lo][j_lo]);
	}
	printf("Image Loaded: %d x %d\n", h, w);
	fclose(fp);
#endif	
}

void OutputOptimisticLabel(UINT8T *labelIn){
#ifdef WIN32
	FILE *fp;
	for (i_lo = 0; i_lo < h; i_lo++)
	for (j_lo = 0; j_lo < w; j_lo++) labelIn[i_lo * w + j_lo] = label_lo[i_lo][j_lo];
	
	fopen_s(&fp, "Label.txt", "w");
	fprintf_s(fp, "%d %d\n", h, w);
	for (i_lo = 0; i_lo < h; i_lo++)
	for (j_lo = 0; j_lo < w; j_lo++) fprintf_s(fp, "%d\n", label_lo[i_lo][j_lo]);
	fclose(fp);
#endif
}

void Dilate(int l, int ver, int hor){
	int flag;
	int di, dj;
	for (i_lo = 0; i_lo < h; i_lo++)
	for (j_lo = 0; j_lo < w; j_lo++){
		flag = 0;
		for (di = -ver; di <= ver; di++) if (i_lo + di >= 0 && i_lo + di < h)
		for (dj = -hor; dj <= hor; dj++) if (j_lo + dj >= 0 && j_lo + dj < w){
			if (label_lo[i_lo + di][j_lo + dj] == l) flag = 1;
			break;
		}
		if (flag) temp[i_lo][j_lo] = l; else temp[i_lo][j_lo] = label_lo[i_lo][j_lo];
	}
	for (i_lo = 0; i_lo < h; i_lo++)
	for (j_lo = 0; j_lo < w; j_lo++) label_lo[i_lo][j_lo] = temp[i_lo][j_lo];
}

void Erode(int l, int ver, int hor){
	int labelCount[MAXLABEL];
	int di, dj;
	for (i_lo = 0; i_lo < h; i_lo++)
	for (j_lo = 0; j_lo < w; j_lo++) if (label_lo[i_lo][j_lo] == l){
		for (l = 0; l < numLabels; l++) labelCount[l] = 0;
		for (di = -ver; di <= ver; di++) if (i_lo + di >= 0 && i_lo + di < h)
		for (dj = -hor; dj <= hor; dj++) if (j_lo + dj >= 0 && j_lo + dj < w){
			labelCount[label_lo[i_lo + di][j_lo + dj]]++;
		}
		for (l = 0; l < numLabels; l++) if (labelCount[l] > labelCount[label_lo[i_lo][j_lo]]) label_lo[i_lo][j_lo] = l;
	}
	for (i_lo = 0; i_lo < h; i_lo++)
	for (j_lo = 0; j_lo < w; j_lo++) label_lo[i_lo][j_lo] = temp[i_lo][j_lo];
}

void Optimize0(){  //消除杂物
	Dilate(2, 6 - (MAXWIDTH / w - 1), 4 - (MAXWIDTH / w - 1));
	Erode(2, 2 - (MAXWIDTH / w - 1), 10 - (MAXWIDTH / w - 1));
}

void Push(int i_lo, int j_lo, int l){
	if (i_lo < 0 || i_lo >= h || j_lo < 0 || j_lo >= w) return;
	if (label_lo[i_lo][j_lo] != l) return;

	if (!temp[i_lo][j_lo]){
		queue[head++] = i_lo * w + j_lo;
		temp[i_lo][j_lo] = 1;
		block[i_lo][j_lo] = numBlocks;
	}
}

void Pop(int *i_lo, int *j_lo){
	*i_lo = queue[tail] / w;
	*j_lo = queue[tail] % w;
	tail++;
}

void Optimize1(){  //连通域限制
	int ti, tj;
	head = tail = 0;
	for (i_lo = 0; i_lo < h; i_lo++)
	for (j_lo = 0; j_lo < w; j_lo++) temp[i_lo][j_lo] = 0;
	numBlocks = 0;
	for (i_lo = 0; i_lo < h; i_lo++)
	for (j_lo = 0; j_lo < w; j_lo++) if (!temp[i_lo][j_lo]){
		l = label_lo[i_lo][j_lo];
		Push(i_lo, j_lo, l);
		while (tail < head){
			Pop(&ti, &tj);
			Push(ti - 1, tj, l);
			Push(ti, tj - 1, l);
			Push(ti, tj + 1, l);
			Push(ti + 1, tj, l);
		}
		numBlocks++;
	}
#ifdef WIN32
	printf("Block Count: %d\n", numBlocks);
#endif
	for (i_lo = 0; i_lo < h; i_lo++)
	{
		for (l = 0; l < numBlocks; l++) temp[i_lo][l] = 0;
		for (j_lo = 0; j_lo < w; j_lo++) temp[i_lo][block[i_lo][j_lo]]++;
		for (j_lo = 0; j_lo < w; j_lo++) if (temp[i_lo][block[i_lo][j_lo]] < w / 3) label_lo[i_lo][j_lo] = 2;
	}
}

void LabelOptimize(UINT8T *labelIn){
	numLabels = 3;
	w = C;
	h = R;
	LoadLabel(labelIn);
	Optimize0();
	Optimize1();
	OutputOptimisticLabel(labelIn);
}