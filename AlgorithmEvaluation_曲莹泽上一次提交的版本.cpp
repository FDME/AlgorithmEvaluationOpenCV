#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <opencv2/opencv.hpp>
#include <opencv2/opencv_modules.hpp>
#include "GCO/GCoptimization.h"


using namespace cv;
using namespace std;

#define MAXWIDTH  640
#define MAXHEIGHT 480

struct GraphPixel{
	int r, g, b;
	int edge;
};

GraphPixel g[MAXWIDTH][MAXHEIGHT];

int w, h;
Mat imageOriginal, imageGray, imageCanny, imageOutput, imageLabel, imageForeground;
Mat mask;
vector<Vec2f> lines;
vector<vector<Point>> contours;


#pragma region Qian Zifei

class WatershedSegment{
private:
	Mat markers;
public:
	void setMarkers(Mat& markerImage){
		markerImage.convertTo(markers,CV_32S);
	}
	Mat process(Mat& image){
		watershed(image,markers);
		markers.convertTo(markers,CV_8U);
		return markers;
	}
};

void ForegroundSeparation(){
	Mat markers(imageOriginal.size(), CV_8U, Scalar(0));
	for(int i = 0; i != markers.rows; i++)
		for(int j = 0; j < markers.cols/4; j++)
			markers.at<uchar>(i,j) = 255;
	for(int i = 0; i != markers.rows; i++)
		for(int j = markers.cols/4; j < markers.cols*3/4; j++)
			markers.at<uchar>(i,j) = 0;
	for(int i = 0; i != markers.rows; i++)
		for(int j = markers.cols*3/4; j < markers.cols; j++)
			markers.at<uchar>(i,j) = 128;

	WatershedSegment segmenter;
	segmenter.setMarkers(markers);

	Mat result = segmenter.process(imageOriginal);
	convertScaleAbs(result,result);
	// imshow("hi",result);
	threshold(result,result,254,255,THRESH_BINARY);
	// imshow("byebye~",result);
	bitwise_and(imageOriginal,imageOriginal,imageForeground,result);
}

#pragma endregion

#pragma region Qu Yingze
Mat imageRetinex, imageContour, imageChecked;
void retinex();
int R, C;
void AdaptiveFindThreshold(const Mat* image, double *low, double *high, int aperture_size = 3);
double Otsu(IplImage* src);
vector<int> edgeIndex; //the indexes of detected rectangles in contours

void ShowImage(Mat, string);
void OptimizeCanny();

bool checkLine(Point start, Point end)
{
	/*double area = contourArea(contour);
	if (area < (R * C / 100)) return false;
	RotatedRect rect = minAreaRect(contour);
	if (rect.size.area() > area * 1.5) return false;*/

	double k;
	k = (end.y - start.y) / (end.x - start.x);
	if (k > 0 && k < 1)
		return true;
	else
		return false;
}

bool collinear(Point start, Point end, Point mid)
{
	double k1, k2; 
	//cout << "P1 x = " << start.x << " y = " << start.y << endl;
	//cout << "P2 x = " << mid.x << " y = " << mid.y << endl;
	//cout << "P3 x = " << end.x << " y = " << end.y << endl;
	k1 = double((end.y - start.y)) / (end.x - start.x);
	k2 = double((mid.y - start.y)) / (mid.x - start.x);
	if (k1 == k2)
		return true;
	else
		return false;
}
/*
	直线检测可以这样：
	1、首先两个double不要直接通过 “==” 比较大小，可以设定一个小量eps，例如eps = 0.01，然后如果abs(a - b) < eps就认为a、b相等
	2、还有直线检测最好不要用相邻三个，最好有一定间隔，比如可以判断p[i], p[i + 10], p[i + 20]是否共线~
	3、如果要用你自己写的直线检测的话，就不要用CV_CHAIN_APPROX_SIMPLE这个优化啦~要保留所有的边界点才能检测~
	
*/
void detectEdges(Mat& imageOriginal) //采用自适应阈值的canny
{
	
	double low = 0.0, high = 0.0; //Thresholds for Canny edge detection

	cvtColor(imageOriginal, imageGray, CV_BGR2GRAY);
	//equalizeHist(imageGray, imageEqualHist);//灰度图象直方图均衡化
	double cannyThreshold = Otsu(&(IplImage)imageOriginal);
	//cannyThreshold = 100;
	AdaptiveFindThreshold(&imageGray, &low, &high);
	cout << "Low: " << low << endl << "High: " << high << endl;
	//Canny(imageGray, imageCanny, low, high);
	Canny(imageGray, imageCanny, cannyThreshold, cannyThreshold * 2);
	ShowImage(imageCanny, "canny1");

	OptimizeCanny();
	Canny(imageCanny, imageCanny, cannyThreshold, cannyThreshold * 2);
	ShowImage(imageCanny, "canny2");
	findContours(imageCanny, contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);
	
	//RotatedRect rect; 
	//findContours(imageCanny, contours, hierarchy, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
	Point start, end;
	int size;
	imageContour = imageOriginal;
	Mat imageLines = imageOriginal.clone();
	for (int i = 0; i < contours.size(); i++){
		/*rect = minAreaRect(contours[i]);
		if (rect.size.area() > R*C / 100)
		{
			Point2f vertices[4];
			rect.points(vertices);
			for (int i = 0; i < 4; i++)
				line(imageContour, vertices[i], vertices[(i + 1) % 4], Scalar(0, 255, 0));
		}
		*/
		size = contours[i].size();
		if (size < 50) continue;
		drawContours(imageContour, contours, i, Scalar(0, 255, 0), 2);
		
		Point temp;
		bool flag = false;
		double length;
		start = end = contours[i][0];
		for (int j = 2; j < size; j = j + 1)
		{
			if (collinear(contours[i][j - 2], contours[i][j - 1], contours[i][j]))
			{
				if (flag = false)
				{
					start = contours[i][j - 2];
					flag = true;
				}
				end = contours[i][j];
			}
			else
			{
				flag = false;
				length = sqrt((start.x - end.x) * (start.x - end.x) + (start.y - end.y) * (start.y - end.y));
				//cout << "length: " << length << endl;
				if (length > 30)
					line(imageLines, start, end, Scalar(255, 0, 0), 2);
			}
		}

		//start = contours[i][0];
		
		//end = contours[i][size - 1];
		//if (checkLine(start, end))
		//line(imageContour, start, end, Scalar(255,0, 0),2);
	}
	ShowImage(imageContour, "contours");
	ShowImage(imageLines, "Lines");
	
}

void checkEdges()
{
	imageChecked = imageOriginal;
	int counter = 0;
	uchar globalRef, rectRef;
	if (contours.size() == 0)  //未检测到矩形
		return;
	else
	{
		//取灰度图平均值为参考值（待改进！）

		globalRef = 0;
		counter = 0;
		for (int i = 0; i<imageGray.rows; i++)
		{
			uchar* data = imageGray.ptr<uchar>(i);
			for (int j = 0; j<imageGray.cols; j++)
			{

				globalRef += ((data[j] - globalRef) / ++counter);
			}
		}
		//printf("Global reference = %x\n", globalRef);

		//逐一判断各矩形
		int k = 0;
		while (k < edgeIndex.size())
		{
			//计算矩形内平均亮度

			//找到x,y最值
			int minx, maxx, miny, maxy;
			minx = maxx = contours[edgeIndex[k]][0].x;
			miny = maxy = contours[edgeIndex[k]][0].y;
			for (int j = 1; j < contours[edgeIndex[k]].size(); j++)
			{
				if (contours[edgeIndex[k]][j].x < minx) minx = contours[edgeIndex[k]][j].x;
				else if (contours[edgeIndex[k]][j].x > maxx) maxx = contours[edgeIndex[k]][j].x;
				if (contours[edgeIndex[k]][j].y < miny) miny = contours[edgeIndex[k]][j].y;
				else if (contours[edgeIndex[k]][j].y > maxy) maxy = contours[edgeIndex[k]][j].y;
			}
			////计算轮廓内亮度均值
			counter = 0;
			rectRef = 0;
			for (int x = minx; x <= maxx; x++)
			for (int y = miny; y <= maxy; y++)
			{
				if (pointPolygonTest(contours[edgeIndex[k]], Point(x, y), true) >= 0) //当前点在轮廓内或轮廓上
					rectRef += ((imageGray.at<uchar>(y, x) - rectRef) / ++counter);
			}

			//cout << edgeIndex[k] << ": ";
			//printf("%x\n", rectRef);
			//比较：若轮廓内亮度高于等于全图参考值，则判断此轮廓非空闲区域
			if (rectRef >= globalRef)
				edgeIndex.erase(edgeIndex.begin() + k);
			else
				k++;
		}
	}

	for (int i = 0; i < edgeIndex.size(); i++)
	{
		cout << edgeIndex[i] << endl;
		drawContours(imageChecked, contours, edgeIndex[i], Scalar(255, 0, 255), 2);
	}

	ShowImage(imageChecked, "imageChecked");
}
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
	ImageOutput()			图像输出，用于MFR Graph Cut
*/
#pragma region ZheyunYao

void ShowImage(Mat image, string windowName){
	namedWindow(windowName, CV_WINDOW_AUTOSIZE);
	imshow(windowName, image);
	waitKey();
}

void LoadImage(string path = "C:\\HuaWeiImage\\华为拍照_校正\\华为拍照_校正\\正常光照\\u_90.jpg"){
	imageOriginal = imread(path);
	ShowImage(imageOriginal, "Original");
	printf("%dx%d\n", w, h);

}

void Preprocess(){
	cvtColor(imageOriginal, imageGray, CV_BGR2GRAY);
	ShowImage(imageGray, "Gray");
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
	ShowImage(imageCanny, "Optimized Canny");
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

void ImageOutput(){
	FILE *fp;
	fopen_s(&fp, "ImageOutput.txt", "w");
	fprintf(fp, "%d %d\n", imageOriginal.rows, imageOriginal.cols);
	for (MatIterator_<Vec3b> it = imageOriginal.begin<Vec3b>(); it != imageOriginal.end<Vec3b>(); it++){
		fprintf(fp, "%d %d %d\n", (*it)[0], (*it)[1], (*it)[2]);
	}
	for (MatIterator_<uchar> it = imageCanny.begin<uchar>(); it != imageCanny.end<uchar>(); it++){
		fprintf(fp, "%d\n", (*it)? 1 : 0);
	}
	fclose(fp);
}

void ImageLabelInput(){
	FILE *fp;
	fopen_s(&fp, "ImageLabel.txt", "r");
	imageLabel = imageOriginal.clone();
	for (MatIterator_<Vec3b> it = imageLabel.begin<Vec3b>(); it != imageLabel.end<Vec3b>(); it++){
		int label;
		fscanf_s(fp, "%d", &label);
		if (label == 0){
			(*it)[0] = 0;
			(*it)[1] = 0;
			(*it)[2] = 0;
		}
		else if (label == 1)
		{
			(*it)[0] = 255;
			(*it)[1] = 255;
			(*it)[2] = 255;
		}
		else if (label == 2)
		{
			(*it)[0] = 0;
			(*it)[1] = 255;
			(*it)[2] = 0;

		}
		else if (label == 3)
		{
			(*it)[0] = 0;
			(*it)[1] = 255;
			(*it)[2] = 0;
		}
	}
	fclose(fp);
	ShowImage(imageLabel, "Label");
}

int Rgb2Gray(GraphPixel p){
	return (int)(p.r * 0.299 + p.g * 0.587 + p.b * 0.114);
}

void ReadImage(){
	FILE *fp;

	fopen_s(&fp, "ImageOutput.txt", "r");

	fscanf_s(fp, "%d %d", &h, &w);
	for (int i = 0; i < h; i++)
	for (int j = 0; j < w; j++){
		fscanf_s(fp, "%d %d %d", &g[i][j].r, &g[i][j].g, &g[i][j].b);
	}
	for (int i = 0; i < h; i++)
	for (int j = 0; j < w; j++){
		fscanf_s(fp, "%d", &g[i][j].edge);
	}

	fclose(fp);
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
		cost = p.edge ? 0 : 300;
	}
	else
	{
		cost = 0;
	}
	return cost;
}

void MultiLabelGraphCut(){
	ReadImage();
	int width = w;
	int height = h;
	int numPixels = w * h;
	int numLabels = 4;

	int *result = new int[numPixels];

	GCoptimizationGridGraph *gc = new GCoptimizationGridGraph(width, height, numLabels);
	for (int l = 0; l < numLabels; l++)
	for (int i = 0; i < height; i++)
	for (int j = 0; j < width; j++){
		gc->setDataCost(i * width + j, l, ComputeCost(i, j, l));
	}
	int varity = 2;
	gc->setSmoothCost(0, 1, 100 / varity);
	gc->setSmoothCost(1, 0, 100 / varity);
	gc->setSmoothCost(0, 2, 100 / varity);
	gc->setSmoothCost(2, 0, 100 / varity);
	gc->setSmoothCost(1, 2, 10 / varity);
	gc->setSmoothCost(2, 1, 10 / varity);

	gc->setSmoothCost(0, 3, 100 / varity);
	gc->setSmoothCost(3, 0, 100 / varity);
	gc->setSmoothCost(1, 3, 100 / varity);
	gc->setSmoothCost(3, 1, 100 / varity);
	gc->setSmoothCost(2, 3, 6 / varity);
	gc->setSmoothCost(3, 2, 6 / varity);


	//gc->setLabelCost(0);
	printf("before: %d\n", gc->compute_energy());
	gc->swap();
	printf("after: %d\n", gc->compute_energy());
	FILE *fout;
	fopen_s(&fout, "ImageLabel.txt", "w");
	for (int i = 0; i < numPixels; i++){
		result[i] = gc->whatLabel(i);
		fprintf_s(fout, "%d\n", result[i]);
	}
	fclose(fout);

	delete[] result;
}





#pragma endregion

int main(){
	string user = "yzy";

	if (user == "yzy")
	{
		LoadImage();
		Preprocess();
		DetectContours();
		//detectlines();

		ImageOutput();
		MultiLabelGraphCut();
		ImageLabelInput();

		waitKey();
		destroyAllWindows();

	}
	else
	if (user == "qzf"){
		LoadImage("/Users/zoe/Documents/Undergraduate/lab/HUAWEI/pictures/1.jpg");
		Preprocess();
		//ForegroundSeparation();

	}
	else
	if (user == "qyz"){
		string filename = "..\\..\\u_60_2.jpg";
		imageOriginal = imread(filename);	//载入图片
		R = imageOriginal.rows;
		C = imageOriginal.cols;

		ShowImage(imageOriginal, "imageOriginal");
		cvtColor(imageOriginal, imageGray, CV_BGR2GRAY);
		double avgGray = cvAvg(&(IplImage)imageGray).val[0];
		cout << "avgGray =" << avgGray<< endl;
		if (avgGray > 110.0) //获取平均亮度
		{
			retinex();
			ShowImage(imageRetinex, "Retinex");
			detectEdges(imageRetinex);
		}
		else
			detectEdges(imageOriginal);
		//detectLines();
		//checkEdges();
		waitKey();
	}
}