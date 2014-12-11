#include "LabelOptimization.h"
#include "ImageSegmentation.h"
#include "2410lib.h"
int h, w;
int numLabels;
int i, j, k, l;
UINT8T label[MAXHEIGHT][MAXWIDTH];
int temp[MAXHEIGHT][MAXWIDTH], block[MAXHEIGHT][MAXWIDTH];
int queue[MAXHEIGHT * MAXWIDTH];
int head, tail;
int numBlocks;
UINT8T lb[SIZE];


void LoadLabel(UINT8T *lb){
	for (i = 0; i < h; i++)
	for (j = 0; j < w; j++) label[i][j] = lb[i * w + j];
	return;
}

void OutputOptimisticLabel(UINT8T *labelIn){
	for (i = 0; i < h; i++)
	for (j = 0; j < w; j++) labelIn[i * w + j] = label[i][j];
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

void Optimize0(){  //????
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

void Optimize1(){  //?????
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
	for (i = 0; i < h; i++)
	{
		for (l = 0; l < numBlocks; l++) temp[i][l] = 0;
		for (j = 0; j < w; j++) temp[i][block[i][j]]++;
		for (j = 0; j < w; j++) if (temp[i][block[i][j]] < w / 3) label[i][j] = 2;
	}
}

//void LabelOptimize(UINT8T *labelIn){
int main(){

	numLabels = 3;
	w = C;
	h = R;
		sys_init();        /* Initial s3c2410's Clock, MMU, Interrupt,Port and UART */
		uart_select(UART0);
	uart_printf("Begin!\n");
	ImageSegment();
	LoadLabel(lb);
	Optimize0();
	Optimize1();
	OutputOptimisticLabel(lb);
	uart_printf("Finish!\n");
	while(1){}
}
