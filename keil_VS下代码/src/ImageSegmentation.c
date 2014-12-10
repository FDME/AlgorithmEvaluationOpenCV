#include <stdio.h>
#include "ImageSegmentation.h"

#define INVALID_EDGE (MAXNODE * 12)
#define INVALID_NODE (MAXPIXEL + 3)

INT32T h, w;
INT32T numLabels = 3;
INT32T i, j, k, index, t;

EDGE a[MAXNODE * 12];
INT32T f[MAXNODE], pre[MAXNODE], lv[MAXNODE], gap[MAXNODE], cur[MAXNODE];
INT32T tot;
INT32T N = MAXPIXEL + 2;
INT32T source, sink;
INT32T lb[MAXPIXEL];

INT32T pixelEdgeS, pixelEdgeE;
INT32T sourceEdgeS, sourceEdgeE;
INT32T sinkEdgeS, sinkEdgeE;

extern RGBTYPE image_Correction[SIZE];
extern UINT8T image_Canny[SIZE];

INT32T max2(INT32T a, INT32T b){
	return (a > b) ? a : b;
}

INT32T max3(INT32T a, INT32T b, INT32T c){
	return max2(max2(a, b), c);
}

INT32T RGB2Gray(RGBTYPE p){
	return (INT32T)(p.r * 0.299 + p.g * 0.587 + p.b * 0.114);
}

INT32T Index(INT32T i, INT32T j){
	return i * w + j;
}

INT32T ComputeDataCost(INT32T index, INT32T l){
	INT32T gray, cost;
	gray = RGB2Gray(image_Correction[index]);
	switch (l)
	{
	case 0: {cost = max2(gray - 10, 0) + 80; break;}
	case 1: {cost = max2(210 - gray, 0) + 10; break;}
	case 2: {if (image_Canny[index] > 0) cost = 1; else cost = max2(max3(image_Correction[index].r, image_Correction[index].g, image_Correction[index].b), max3(255 - image_Correction[index].r, 255 - image_Correction[index].g, 255 - image_Correction[index].b)) * 7 / 10;  break; }
	default: {cost = 0; break; }
	}
	//printf("(r=%d g=%d b=%d gray=%d) l=%d cost=%d\n", image_Correction[index].r, image_Correction[index].g, image_Correction[index].b, gray, l, cost);
	return cost;
}

INT32T ComputeSmoothCost(INT32T l1, INT32T l2){
	INT32T varity = 2;
	if (l1 == l2) return 0;
	if (l1 == 0){
		if (l2 == 1) return 120 / varity;
		if (l2 == 2) return 60 / varity;
	}
	else
	if (l1 == 1){
		if (l2 == 2) return 60 / varity;
		if (l2 == 0) return 120 / varity;
	}
	else
	if (l1 == 2){
		if (l2 == 0) return 60 / varity;
		if (l2 == 1) return 60 / varity;
	}
	return 0;
}

void AddEdge(INT32T x, INT32T y, INT32T cap, INT32T revCap){
	a[tot].x = x;
	a[tot].y = y;
	a[tot].cap = cap;
	a[tot].next = f[x];
	f[x] = tot;
	tot++;

	a[tot].x = y;
	a[tot].y = x;
	a[tot].cap = revCap;
	a[tot].next = f[y];
	f[y] = tot;
	tot++;
}




void InitMap(){
	N = MAXPIXEL + 2;
	for (i = 0; i < N; i++) f[i] = INVALID_EDGE;
	tot = 0;
	source = MAXPIXEL + 1;
	sink = MAXPIXEL + 2;
}

void BuildMap(){
	sourceEdgeS = tot;
	sourceEdgeS = tot;
	for (i = 0; i < h; i++)
	for (j = 0; j < w; j++) AddEdge(source, Index(i, j), 0, 0);
	sourceEdgeE = tot;

	sinkEdgeS = tot;
	for (i = 0; i < h; i++)
	for (j = 0; j < w; j++) AddEdge(Index(i, j), sink, 0, 0);	
	sinkEdgeE = tot;

	pixelEdgeS = tot;
	for (i = 0; i < h; i++)
	for (j = 0; j < w; j++){
		if (i > 0) AddEdge(Index(i - 1, j), Index(i, j), 0, 0);
		if (j > 0) AddEdge(Index(i, j - 1), Index(i, j), 0, 0);
	}
	pixelEdgeE = tot;
}

INT32T ComputeEnergy(){
	INT32T energy = 0;
	for (i = 0; i < MAXPIXEL; i++) energy += ComputeDataCost(i, lb[i]);
	for (i = 0; i < MAXPIXEL; i++) energy += (int)ComputeDataCost(i, lb[i]);
	printf("Data Cost: %d\n", energy);
	for (t = pixelEdgeS; t < pixelEdgeE; t += 2) energy += ComputeSmoothCost(lb[a[t].x], lb[a[t].y]);
	printf("Data & Smooth Cost: %d\n", energy);
	return energy;
}


void ModifyMap(INT32T alpha, INT32T beta){
	for (t = 0; t < tot; t++) a[t].cap = 0;
	for (t = sourceEdgeS; t < sourceEdgeE; t += 2){
			if ((lb[a[t].y] == alpha) || (lb[a[t].y] == beta))  a[t].cap = ComputeDataCost(a[t].y, alpha);
	}

	for (int t = sinkEdgeS; t < sinkEdgeE; t += 2){
			if ((lb[a[t].x] == alpha) || (lb[a[t].x] == beta))  a[t].cap = ComputeDataCost(a[t].x, beta);
	}

	for (int t = pixelEdgeS; t < pixelEdgeE; t ++){
		if ((lb[a[t].x] == alpha) || (lb[a[t].x] == beta)){
			if ((lb[a[t].y] == alpha) || (lb[a[t].y] == beta)){
				a[t].cap = ComputeSmoothCost(alpha, beta);
			}
		}
	}
}

INT32T Maxflow(){
	INT32T flow = 0, aug;
	INT32T x, y;
	UINT8T flag;
	INT32T minLv;
	for (i = 0; i < N; i++) pre[i] = INVALID_NODE;
	for (i = 0; i < N; i++) cur[i] = f[i];
	
	for (i = 0; i < N; i++) lv[i] = 1;
	lv[sink] = 0;
	for (i = 0; i < N; i++) gap[i] = 0;
	gap[1] = N - 1;
	gap[0] = 1;

	x = source;
	while (lv[x] < N){
		flag = 0;
		for (t = cur[x]; t != INVALID_EDGE; t = a[t].next){
			y = a[t].y;
			cur[x] = t;
			if (a[t].cap > 0 && lv[x] == lv[y] + 1){
				flag = 1;
				pre[y] = x;
				x = y;
				if (x == sink){
					aug = INF;
					y = pre[x];
					while (1){
						if (a[cur[y]].cap < aug) aug = a[cur[y]].cap;
						if (y == source) break;
						y = pre[y];
					}
					flow += aug;
					y = pre[x];
					while (1){
						a[cur[y]].cap -= aug;
						a[cur[y] ^ 1].cap += aug;
						if (y == source) break;
						y = pre[y];
					}
					x = source;
				}
				break;
			}
		}
		if (flag) continue;
		minLv = N;
		for (t = f[x]; t != INVALID_EDGE; t = a[t].next){
			y = a[t].y;
			if (a[t].cap > 0 && lv[y] < minLv){
				minLv = lv[y];
				cur[x] = t;
			}
		}
		if ((--gap[lv[x]]) == 0) break;
		lv[x] = minLv + 1;
		gap[lv[x]]++;
		if (x != source) x = pre[x];
	}
	printf("Maxflow: %d\n", flow);
	return flow;
}
void TestMaxflow(){
	InitMap();
	N = 6;
	source = 0;
	sink = N - 1;
	AddEdge(source, 1, 100, 0);
	AddEdge(1, 2, 110, 0);
	AddEdge(2, sink, 20, 0);
	AddEdge(source, 3, 30, 0);
	AddEdge(3, 4, 200, 0);
	AddEdge(4, sink, 201, 0);
	AddEdge(2, 4, 11, 0);
	AddEdge(4, 2, 30, 0);
	Maxflow();
}

void ChangeLabel(int alpha, int beta){
	for (t = sinkEdgeS; t < sinkEdgeE; t += 2) if (a[t].cap == 0 && a[t ^ 1].cap > 0) lb[a[t].x] = beta;
	for (t = sourceEdgeS; t < sourceEdgeE; t += 2) if (a[t].cap == 0 && a[t ^ 1].cap > 0) lb[a[t].y] = alpha;
}

void AlphaBetaSwap(){
	INT32T alpha, beta;
	for (alpha = 0; alpha < numLabels; alpha++)
	for (beta = alpha + 1; beta < numLabels; beta++){
		ModifyMap(alpha, beta);
		Maxflow();
		ChangeLabel(alpha, beta);
		//Lb2Label();
		//ImageLabelInput();
	}
}

void LoadImage(){
	return;
	FILE *fp;
	fopen_s(&fp, "C:\\HuaWeiImage\\ARMProject\\ARMProject\\ImageData.txt", "r");
	fscanf_s(fp, "%d %d", &h, &w);
	printf("Image Loaded: %d x %d\n", h, w);
	for (i = 0; i < h; i++)
	for (j = 0; j < w; j++){
		index = i * w + j;
		fscanf_s(fp, "%d %d %d", &image_Correction[index].b, &image_Correction[index].g, &image_Correction[index].r);
	}
	for (i = 0; i < h; i++)
	for (j = 0; j < w; j++){
		index = i * w + j;
		fscanf_s(fp, "%d", &image_Canny[index]);
	}
	fclose(fp);
}

void InitLabel(){
	INT32T i, l, minCost, minCostLabel;
	for (i = 0; i < MAXPIXEL; i++){
		minCost = INF;
		minCostLabel = 0;
		for (l = 0; l < numLabels; l++) if (ComputeDataCost(i, l) < minCost){
			minCost = ComputeDataCost(i, l);
			minCostLabel = l;
		}
		lb[i] = minCostLabel;
	}
}

void SwapOperation(INT32T count){
	INT32T newEnergy, oldEnergy;
	InitMap();
	BuildMap();
	InitLabel();
	newEnergy = ComputeEnergy();
	oldEnergy = newEnergy + 1;
	printf("Before energy: %d\n", newEnergy);
	while ((oldEnergy != newEnergy) && (count-- > 0)){
		oldEnergy = newEnergy;
		AlphaBetaSwap();
		newEnergy = ComputeEnergy();
		printf("After energy: %d\n", newEnergy);
	}
}

void OutputSegmentationLabel(UINT8T *label){
	for (i = 0; i < MAXPIXEL; i++) label[i] = lb[i];

	//return;
	FILE *fp;
	fopen_s(&fp, "Label.txt", "w");
	fprintf(fp, "%d %d\n", h, w);
	for (i = 0; i < MAXPIXEL; i++) fprintf_s(fp, "%d\n", label[i]);
	fprintf_s(fp, "\n");
	fclose(fp);
}

//void main(){
void ImageSegment(UINT8T *label){
	w = C;
	h = R;
	SwapOperation(6);
//	InitLabel();
	OutputSegmentationLabel(label);
	//AlphaBetaSwap();
	//TestMaxflow();
}

void _ImageSegment(RGBTYPE *image_Correction, UINT8T *canny){
}