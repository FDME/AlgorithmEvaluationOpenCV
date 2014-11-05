#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <opencv2/opencv.hpp>
#include <opencv2/opencv_modules.hpp>
#include "GCO\GCoptimization.h"


using namespace cv;
using namespace std;

#define MAXWIDTH  480
#define MAXHEIGHT 640

struct GraphPixel{
	int r, g, b;
	int edge;
};

GraphPixel g[MAXWIDTH][MAXHEIGHT];
int img[MAXHEIGHT][MAXWIDTH] = {};
int tmp[MAXHEIGHT][MAXWIDTH] = {};
int label[MAXHEIGHT][MAXWIDTH] = {};
int numLabels = 4;

#define MAXBLOCKS 100
int numBlocks;
int block[MAXHEIGHT][MAXWIDTH];
bool isVisited[MAXHEIGHT][MAXWIDTH];
int blockCount[MAXHEIGHT][MAXBLOCKS];

int w, h;
Mat imageOriginal, imageGray, imageCanny, imageOutput, imageLabel, imageForeground;
Mat mask, makers;
vector<Vec2f> lines;
vector<vector<Point>> contours;

#pragma region Qian Zifei

class WatershedSegment{
private:
	Mat markers;
public:
	void setMarkers(Mat& markerImage){
		markerImage.convertTo(markers, CV_32S);
	}
	Mat process(Mat& image){
		watershed(image, markers);
		markers.convertTo(markers, CV_8U);
		return markers;
	}
};

void ForegroundSeparation(){
	Mat markers(imageOriginal.size(), CV_8U, Scalar(0));
	for (int i = 0; i != markers.rows; i++)
	for (int j = 0; j < markers.cols / 4; j++)
		markers.at<uchar>(i, j) = 255;
	for (int i = 0; i != markers.rows; i++)
	for (int j = markers.cols / 4; j < markers.cols * 3 / 4; j++)
		markers.at<uchar>(i, j) = 0;
	for (int i = 0; i != markers.rows; i++)
	for (int j = markers.cols * 3 / 4; j < markers.cols; j++)
		markers.at<uchar>(i, j) = 128;

	WatershedSegment segmenter;
	segmenter.setMarkers(markers);

	Mat result = segmenter.process(imageOriginal);
	convertScaleAbs(result, result);
	imshow("hi",result);
	waitKey();
	threshold(result, result, 254, 255, THRESH_BINARY);
	imshow("byebye~",result);
	waitKey();
	bitwise_and(imageOriginal, imageOriginal, imageForeground, result);
	imshow("byebye~", imageOriginal);
	waitKey();
}

#pragma endregion

#pragma region Qu Yingze

#pragma endregion

/*
	Functions:
	ShowImage()				显示图像
	LoadImage()				载入图像
	Preprocess()			预处理
	DetectContours()		边缘检测
	OptimizationCanny()		Canny优化（腐蚀膨胀）
	DetectLines()			直线检测
	CheckContour()			边缘验证（面积、形状）
*/
#pragma region ZheyunYao

void ShowImage(Mat image, string windowName){
	namedWindow(windowName, CV_WINDOW_AUTOSIZE);
	imshow(windowName, image);
	waitKey();
}

void LoadImage(string path = "C:\\HuaWeiImage\\华为拍照_校正\\华为拍照_校正\\正常光照带假面板_2\\u_60.jpg"){
	imageOriginal = imread(path);
	ShowImage(imageOriginal, "Original");
	h = imageOriginal.rows;
	w = imageOriginal.cols;
	printf("%dx%d\n", h, w);

}

void Preprocess(){
	cvtColor(imageOriginal, imageGray, CV_BGR2GRAY);
	//ShowImage(imageGray, "Gray");
	//亮度调整~

}

bool checkContour(vector<Point> contour){
	double area = contourArea(contour);
	if (area < (w * h / 100)) return false;
	RotatedRect rect = minAreaRect(contour);
	if (rect.size.area() > area * 1.5) return false;
	return true;
}

void OptimizeCanny(){
	Mat imageTemp1, imageTemp2, imageTemp3, imageTemp4;
	dilate(imageCanny, imageTemp1, Mat(3, 3, CV_8U), Point(-1, -1), 2);
	erode(imageTemp1, imageTemp2, Mat(3, 3, CV_8U), Point(-1, -1), 2);
	
	ShowImage(imageTemp1, "Dilate");
	ShowImage(imageTemp2, "Erode");
	//Canny(imageTemp2, imageCanny, 100, 200);
	imageCanny = imageTemp2.clone();
	//ShowImage(imageCanny, "Optimized Canny");
}

void DetectContours(double thresh = 100){
	Canny(imageOriginal, imageCanny, thresh, thresh * 2);
	line(imageCanny, Point(0, 0), Point(0, h), Scalar(255, 255, 255), 1);
	ShowImage(imageCanny, "Canny");
	//OptimizeCanny();
	findContours(imageCanny, contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);
	imageOutput = imageOriginal.clone();
	for (int i = 0; i < contours.size(); i++){
		if (checkContour(contours[i])){
			drawContours(imageOutput, contours, i, Scalar(0, 255, 0), 2);
		}
	}
	//ShowImage(imageOutput, "Contours");
}

void DetectLines(){
	HoughLines(imageCanny, lines, 1, CV_PI / 720, 150, 0, 0);
	cout << lines.size() << endl;
	imageOutput = imageOriginal.clone();
	for (int i = 0; i < lines.size(); i++){
		float rho = lines[i][0];
		float theta = lines[i][1];
		float y = rho / sin(theta);
		float x = rho / cos(theta);
		line(imageOutput, Point(0, (int)y), Point((int)x, 0), Scalar(0, 0, 255), 2);
	}
	ShowImage(imageOutput, "Lines");
}

void Image2Array(){
	int i = 0; 
	int j = 0;
	for (MatIterator_<Vec3b> it = imageOriginal.begin<Vec3b>(); it != imageOriginal.end<Vec3b>(); it++){
		g[i][j].b = (*it)[0];
		g[i][j].g = (*it)[1];
		g[i][j].r = (*it)[2];
		j++;
		if (j == w){
			j = 0;
			i++;
		}
	}
	i = 0;
	j = 0;
	for (MatIterator_<uchar> it = imageCanny.begin<uchar>(); it != imageCanny.end<uchar>(); it++){
		g[i][j].edge = (*it);
		j++;
		if (j == w){
			j = 0;
			i++;
		}
	}
}

void ImageLabelInput(){
	imageLabel = imageOriginal.clone();
	MatIterator_<Vec3b> it = imageLabel.begin<Vec3b>();
	for (int i = 0; i < h; i++)
	for (int j = 0; j < w; j++){
		if (label[i][j] == 0){
			(*it)[0] = 0;
			(*it)[1] = 0;
			(*it)[2] = 0;
		}
		else if (label[i][j] == 1)
		{
			(*it)[0] = 255;
			(*it)[1] = 255;
			(*it)[2] = 255;
		}
		else if (label[i][j] == 2)
		{
			(*it)[0] = 0;
			(*it)[1] = 255;
			(*it)[2] = 0;

		}
		else if (label[i][j] == 3)
		{
			(*it)[0] = 0;
			(*it)[1] = 255;
			(*it)[2] = 0;
		}
		else {
			(*it)[0] = 255;
			(*it)[1] = 0;
			(*it)[2] = 0;
		}
		it++;
	}
	ShowImage(imageLabel, "Label");
}

int Rgb2Gray(GraphPixel p){
	return (int)(p.r * 0.299 + p.g * 0.587 + p.b * 0.114);
}

int max(int x, int y, int z){
	return max(max(x, y), z);
}

int ComputeCost(int i, int j, int l){
	int cost = 0;
	GraphPixel p = g[i][j];
	int gray = Rgb2Gray(p);
	if (l == 0){
		cost = max(gray - 10, 0) + 80;
	}
	else
	if (l == 1){
		cost = max(210 - gray, 0) + 10;
	}
	else
	if (l == 2){
		cost = max(max(p.r, p.g, p.b), max(255 - p.r, 255 - p.g, 255 - p.b)) * 2 / 3;
	}
	else
	if (l == 3){
		cost = (p.edge > 0) ? 0 : 300;
	}
	else
	{
		cost = 0;
	}
	return cost * 2;
}

void MultiLabelGraphCut(){
	Image2Array();
	int width = w;
	int height = h;
	int numPixels = w * h;

	int *result = new int[numPixels];

	GCoptimizationGridGraph *gc = new GCoptimizationGridGraph(width, height, numLabels);
	for (int l = 0; l < numLabels; l++)
	for (int i = 0; i < height; i++)
	for (int j = 0; j < width; j++){
		gc->setDataCost(i * width + j, l, ComputeCost(i, j, l));
	}
	int varity = 12;
	gc->setSmoothCost(0, 1, 600 / varity);
	gc->setSmoothCost(1, 0, 600 / varity);
	gc->setSmoothCost(0, 2, 600 / varity);
	gc->setSmoothCost(2, 0, 600 / varity);
	gc->setSmoothCost(1, 2, 600 / varity);
	gc->setSmoothCost(2, 1, 600 / varity);

	gc->setSmoothCost(0, 3, 600 / varity);
	gc->setSmoothCost(3, 0, 600 / varity);
	gc->setSmoothCost(1, 3, 600 / varity);
	gc->setSmoothCost(3, 1, 600 / varity);
	gc->setSmoothCost(2, 3, 12 / varity);
	gc->setSmoothCost(3, 2, 12 / varity);

	//gc->setLabelCost(0);
	printf("before: %d\n", gc->compute_energy());
	gc->swap();
	printf("after: %d\n", gc->compute_energy());
	for (int i = 0; i < numPixels; i++){
		result[i] = gc->whatLabel(i);
		//printf("(%d, %d)\n", i / w, i % w);
		label[i / w][i % w] = result[i];
	}
	
	delete[] result;
}

void Dilate(int l, int ver, int hor){
	for (int i = 0; i < h; i++)
	for (int j = 0; j < w; j++) {
		bool flag = false;
		for (int ii = -ver; ii <= ver; ii++) if ((i + ii >= 0) && (i + ii < h))
		for (int jj = -hor; jj <= hor; jj++) if ((j + jj >= 0) && (j + jj < w)){
			//printf("(%d, %d)\n", i + ii, j + jj);
			if (img[i + ii][j + jj] == l) flag = true;
		}
		if (flag) tmp[i][j] = l; else tmp[i][j] = img[i][j];
	}
	for (int i = 0; i < h; i++)
	for (int j = 0; j < w; j++) img[i][j] = tmp[i][j];
}

void Erode(int l, int ver, int hor){
	int labelCount[10];
	for (int i = 0; i < h; i++)
	for (int j = 0; j < w; j++) if (img[i][j] == l){
		bool flag = true;
		for (int t = 0; t < numLabels; t++) labelCount[t] = 0;
		for (int ii = -ver; ii <= ver; ii++) if ((i + ii >= 0) && (i + ii < h))
		for (int jj = -hor; jj <= hor; jj++) if ((j + jj >= 0) && (j + jj < w)){
			//printf("(%d, %d)\n", i + ii, j + jj);
			if (img[i + ii][j + jj] != l) {
				flag = false;
				labelCount[img[i + ii][j + jj]]++;
			}
		}
		if (flag) tmp[i][j] = l; else {
			int majorLabel = l;
			for (int t = 0; t < numLabels; t++) if (labelCount[t] > labelCount[majorLabel]) majorLabel = t;
			tmp[i][j] = majorLabel;
		}
	}
	for (int i = 0; i < h; i++)
	for (int j = 0; j < w; j++) img[i][j] = tmp[i][j];

}
void DetectSpace_0(){		//label优化
	for (int i = 0; i < h; i++)
	for (int j = 0; j < w; j++) if (label[i][j] == 3) label[i][j] = 2;
	numLabels = 3;
	 
	for (int i = 0; i < h; i++)
	for (int j = 0; j < w; j++) img[i][j] = label[i][j];

	Dilate(2, 4, 5);
	Erode(2, 2, 10);

	for (int i = 0; i < h; i++)
	for (int j = 0; j < w; j++) label[i][j] = img[i][j];
	for (int i = 0; i < h; i++)
	for (int j = 0; j < w; j++) if (makers.at<int>(i, j) == 2) label[i][j] = 255;

}

void DetectSpace_1(){  //分水岭分割前背景
	makers = Mat(imageOriginal.size(), CV_32SC1, Scalar(0));
	for (int i = 0; i < h; i++)
	for (int j = 0; j < w / 4; j++) makers.at<int>(i, j) = 1;

	for (int i = 0; i < h; i++)
	for (int j = 4 * w / 5; j < w; j++) makers.at<int>(i, j) = 2;

	watershed(imageOriginal, makers);
}

int dir[4][2] = { { 0, -1 }, { 0, 1 }, { -1, 0 }, { 1, 0 } };


void dfs(int i, int j, int l){
	if ((i < 0) || (i >= h) || (j < 0) || (j >= w)) return;
	if (isVisited[i][j]) return;
	if (label[i][j] != l) return;
	block[i][j] = numBlocks;
	//printf("%d, %d = %d\n", i, j, numBlocks);
	isVisited[i][j] = true;
	for (int d = 0; d < 4; d++) dfs(i + dir[d][0], j + dir[d][1], label[i][j]);
}

void DetectSpace_2(){ //限制连通域的联通宽度，这个部分放在视角扭正之后效果更佳~！
	numBlocks = 0;
	for (int i = 0; i < h; i++)
	for (int j = 0; j < w; j++) isVisited[i][j] = false;
	for (int i = 0; i < h; i++)
	for (int j = 0; j < w; j++) if (!isVisited[i][j]){
		dfs(i, j, label[i][j]);
		numBlocks++;
	}
	printf("blocks: %d\n", numBlocks);
	for (int i = 0; i < h; i++)
	{
		for (int l = 0; l < numBlocks; l++) blockCount[i][l] = 0;
		for (int j = 0; j < w; j++) blockCount[i][block[i][j]]++;
		for (int j = 0; j < w; j++) if (blockCount[i][block[i][j]] < w / 8) label[i][j] = 3;
	}

}

#pragma endregion


int main(){
	string user = "yzy";

	if (user == "yzy")
	{
		LoadImage();
		Preprocess();
		DetectSpace_1();
		DetectContours();
		//DetectLines();
		MultiLabelGraphCut();
		DetectSpace_0();
		ImageLabelInput();
		DetectSpace_2();
		ImageLabelInput();

		waitKey();
		destroyAllWindows();

	} else
	if (user == "qzf"){

		LoadImage();
		Preprocess();
		ForegroundSeparation();
	} else
	if (user == "qyz"){
		//put your test code here :>s
	}
	return 0;
}
