#include <stdio.h>
#include "LabelOptimization.h"

int h, w;
int numLabels;
int i, j, k, l;
UINT8T label[MAXHEIGHT][MAXWIDTH];
int temp[MAXHEIGHT][MAXWIDTH], block[MAXHEIGHT][MAXWIDTH];
int queue[MAXHEIGHT * MAXWIDTH];
int head, tail;
int numBlocks;

void LoadLabel(UINT8T *labelIn){
	for (i = 0; i < h; i++)
	for (j = 0; j < w; j++) label[i][j] = labelIn[i * w + j];
	return;
	FILE *fp;
	fopen_s(&fp, "Label.txt", "r");
	fscanf_s(fp, "%d %d", &h, &w);
	printf("Image Loaded: %d x %d\n", h, w);
	for (i = 0; i < h; i++)
	for (j = 0; j < w; j++) {
		//printf("%d, %d\n", i, j);
		fscanf_s(fp, "%d", &label[i][j]);
	}
	printf("Image Loaded: %d x %d\n", h, w);
	fclose(fp);
}

void OutputOptimisticLabel(UINT8T *labelIn){
	for (i = 0; i < h; i++)
	for (j = 0; j < w; j++) labelIn[i * w + j] = label[i][j];
	FILE *fp;
	fopen_s(&fp, "Label.txt", "w");
	fprintf_s(fp, "%d %d\n", h, w);
	for (i = 0; i < h; i++)
	for (j = 0; j < w; j++) fprintf_s(fp, "%d\n", label[i][j]);
	fclose(fp);
}

void Dilate(int l, int ver, int hor){
	int flag;
	int di, dj;
	for (i = 0; i < h; i++)
	for (j = 0; j < w; j++){
		flag = 0;
		for (di = -ver; di <= ver; di++) if (i + di >= 0 && i + di < h)
		for (dj = -hor; dj <= hor; dj++) if (j + dj >= 0 && j + dj < w){
			if (label[i + di][j + dj] == l) flag = 1;
			break;
		}
		if (flag) temp[i][j] = l; else temp[i][j] = label[i][j];
	}
	for (i = 0; i < h; i++)
	for (j = 0; j < w; j++) label[i][j] = temp[i][j];
}

void Erode(int l, int ver, int hor){
	int labelCount[MAXLABEL];
	int di, dj;
	for (i = 0; i < h; i++)
	for (j = 0; j < w; j++) if (label[i][j] == l){
		for (l = 0; l < numLabels; l++) labelCount[l] = 0;
		for (di = -ver; di <= ver; di++) if (i + di >= 0 && i + di < h)
		for (dj = -hor; dj <= hor; dj++) if (j + dj >= 0 && j + dj < w){
			labelCount[label[i + di][j + dj]]++;
		}
		for (l = 0; l < numLabels; l++) if (labelCount[l] > labelCount[label[i][j]]) label[i][j] = l;
	}
	for (i = 0; i < h; i++)
	for (j = 0; j < w; j++) label[i][j] = temp[i][j];
}

void Optimize0(){  //消除杂物
	Dilate(2, 6 - (MAXWIDTH / w - 1), 4 - (MAXWIDTH / w - 1));
	Erode(2, 2 - (MAXWIDTH / w - 1), 10 - (MAXWIDTH / w - 1));
}

void Push(int i, int j, int l){
	if (i < 0 || i >= h || j < 0 || j >= w) return;
	if (label[i][j] != l) return;

	if (!temp[i][j]){
		queue[head++] = i * w + j;
		temp[i][j] = 1;
		block[i][j] = numBlocks;
	}
}

void Pop(int *i, int *j){
	*i = queue[tail] / w;
	*j = queue[tail] % w;
	tail++;
}

void Optimize1(){  //连通域限制
	int ti, tj;
	head = tail = 0;
	for (i = 0; i < h; i++)
	for (j = 0; j < w; j++) temp[i][j] = 0;
	numBlocks = 0;
	for (i = 0; i < h; i++)
	for (j = 0; j < w; j++) if (!temp[i][j]){
		l = label[i][j];
		Push(i, j, l);
		while (tail < head){
			Pop(&ti, &tj);
			Push(ti - 1, tj, l);
			Push(ti, tj - 1, l);
			Push(ti, tj + 1, l);
			Push(ti + 1, tj, l);
		}
		numBlocks++;
	}
	printf("Block Count: %d\n", numBlocks);
	for (i = 0; i < h; i++)
	{
		for (l = 0; l < numBlocks; l++) temp[i][l] = 0;
		for (j = 0; j < w; j++) temp[i][block[i][j]]++;
		for (j = 0; j < w; j++) if (temp[i][block[i][j]] < w / 3) label[i][j] = 2;
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