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

double cam[] = {386.951, 0, 233.539, 0, 384.132, 352.891, 0, 0, 1};
double dis[] = {-0.44601, 0.266848, -0.00143158, 0.000143152, -0.103006};

Mat camMat(3,3,CV_64F,cam), distCoeffs(1,5,CV_64F,dis);
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
	imshow("byebye~", imageForeground);
	waitKey();
}

void cali()
{
    Mat image;
    vector< vector<Point2f> > image_points;
    vector< vector<Point3f> > object_points;
    vector<Point2f> tmp;
    vector<Point3f> chessboard;
    Size boardSize(6,9);
    int brdnum = 8;

    for(int i = 0; i != 9; i++)
        for(int j = 0; j != 6; j++)
            chessboard.push_back(Point3i(i,j,0));

    
    // bool found = findChessboardCorners(image,boardSize,tmp,CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FAST_CHECK | CV_CALIB_CB_NORMALIZE_IMAGE);
    // if(found)
    // drawChessboardCorners(image,boardSize,Mat(tmp),found);
    // imshow("hello~",image);
    for(int i = 0; i != brdnum; i++){
        string num;
        stringstream ss;
        ss << i+1;
        ss >> num;
        image = imread("C:\\Users\\ZoeQIAN\\Documents\\Visual Studio 2012\\Projects\\calibration\\"+num+".jpg");
	//	copyMakeBorder(image,image,image.rows/5,image.rows/5,image.cols/5,image.cols/5,BORDER_CONSTANT,Scalar(255));
        bool found = findChessboardCorners(image,boardSize,tmp,CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FAST_CHECK | CV_CALIB_CB_NORMALIZE_IMAGE);
        if(found){
            image_points.push_back(tmp);
            object_points.push_back(chessboard);
            drawChessboardCorners(image,boardSize,Mat(tmp),found);
            //imshow("hello~",image);
           // waitKey();
        }
    }
    Mat camMat_est;
    Mat distCoeffs_est;
    vector<Mat> rvecs, tvecs;
    cout << "Calibrating...";
    calibrateCamera(object_points, image_points, Size(image.rows,image.cols), camMat_est, distCoeffs_est, rvecs, tvecs);
		for(int i = 0; i != camMat.rows; i++){
			for(int j = 0; j != camMat.cols; j++)
				cout << camMat.at<double>(i,j) << "   " << flush;
			cout << endl;
		}
		for(int i = 0; i != distCoeffs.rows; i++){
			for(int j = 0; j != distCoeffs.cols; j++)
				cout << distCoeffs.at<double>(i,j) << "   " << flush;
			cout << endl;
		}
		for(int i = 0; i != camMat.rows; i++){
			for(int j = 0; j != camMat.cols; j++)
				cout << camMat_est.at<double>(i,j) << "   " << flush;
			cout << endl;
		}
		for(int i = 0; i != distCoeffs.rows; i++){
			for(int j = 0; j != distCoeffs.cols; j++)
				cout << distCoeffs_est.at<double>(i,j) << "   " << flush;
			cout << endl;
		}

    cout << "Done" << endl;
    cout << "Undistort..." << endl;
    string name;
    string type;
    //cout << "Enter the type:" << endl;
    //cin >> type;
    while(1){
        cout << "Please enter the image name, q for exit:" << endl;
        cin >> name;
        if(name == "q")
            break;
        image = imread("C:\\Users\\ZoeQIAN\\Pictures\\huawei\\"+name+".jpg");
		Mat timg;
        imshow("Original",image);
		waitKey();
		//给图片加边
       /*  copyMakeBorder(image,timg,image.rows/5,image.rows/5,image.cols/5,image.cols/5,BORDER_CONSTANT,Scalar(255));
         imshow("nani",timg);*/
		 //imshow("ale",image);
		 Mat undis(timg.size(),CV_8U,Scalar(255));
        //Mat undis;
        undistort(image,undis,camMat_est,distCoeffs_est);
		for(int i = 0; i != camMat_est.rows; i++){
			for(int j = 0; j != camMat_est.cols; j++)
				cout << camMat_est.at<double>(i,j) << "   " << flush;
			cout << endl;
		}
		for(int i = 0; i != distCoeffs_est.rows; i++){
			for(int j = 0; j != distCoeffs_est.cols; j++)
				cout << distCoeffs_est.at<double>(i,j) << "   " << flush;
			cout << endl;
		}
        cout << "Done" << endl;
        imshow("Undistort",undis);
        waitKey();
        char tmp;
        cin >> tmp;
        destroyWindow("Undistort");
    }
    destroyWindow("Undistort");
    // waitKey();
}

#pragma endregion

#pragma region Qu Yingze
Mat imageRetinex, imageContour, imageChecked;
void retinex();
int R, C;
void AdaptiveFindThreshold(const Mat* image, double *low, double *high, int aperture_size = 3);
double Otsu(IplImage* src);
vector<int> edgeIndex; //the indexes of detected rectangles in contours
vector<double> Lines_K;//拟合直线方程参数
vector<double> Lines_B;//拟合直线方程参数
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
//三点判断是否为直线
bool collinear(Point start, Point end, Point mid)
{
	double k1, k2;
	double eps = 0.00001;
	//cout << "P1 x = " << start.x << " y = " << start.y << endl;
	//cout << "P2 x = " << mid.x << " y = " << mid.y << endl;
	//cout << "P3 x = " << end.x << " y = " << end.y << endl;
	k1 = double((end.y - start.y)) / (end.x - start.x);
	k2 = double((mid.y - start.y)) / (mid.x - start.x);
	if (abs(k1 - k2) < eps)
		return true;
	else
		return false;
}
//最小二乘法拟合优度判断是否为直线
bool least_squares(Point* points, int n)
{
	double r = 0;
	int xy_sum = 0, x_sum = 0, y_sum = 0, x_square_sum = 0, y_square_sum = 0;
	for (int i = 0; i < n; i++)
	{
		//xy_sum += points[i].x * points[i].y;
		x_sum += points[i].x;
		y_sum += points[i].y;
		//x_square_sum += points[i].x * points[i].x;
		//y_square_sum += points[i].y * points[i].y;
	}
	double x_mean = (double)x_sum / n;
	double y_mean = (double)y_sum / n;
	double x_temp = 0, y_temp = 0;
	for (int i = 0; i < n; i++)
	{
		x_temp += (points[i].x - x_mean) *(points[i].x - x_mean);
		y_temp += (points[i].y - y_mean) * (points[i].y - y_mean);
	}
	double x_stdev = sqrt(x_temp / n);
	double y_stdev = sqrt(y_temp / n);
	//r = (double)(xy_sum - n*(x_sum / n)*(y_sum / n)) / sqrt((x_square_sum - n *(x_sum / n)*(x_sum / n))*(y_square_sum - n*(y_sum / n)*(y_sum / n)));
	for (int i = 0; i < n; i++)
		r += ((points[i].x - x_mean) / x_stdev)*((points[i].y - y_mean) / y_stdev);
	r /= n;
	//cout << "r =" <<r << endl;
	if (abs(r) > 0.95)
		return true;
	else
		return false;
}

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

	Mat imageLines = imageOriginal.clone();
	Mat LeastSq = imageOriginal.clone();
	OptimizeCanny();
	Canny(imageCanny, imageCanny, cannyThreshold, cannyThreshold * 2);
	ShowImage(imageCanny, "canny2");
	findContours(imageCanny, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
	
	int start, end;
	int size;
	imageContour = imageOriginal;

	for (int i = 0; i < contours.size(); i++)
	{
		size = contours[i].size();
		if (size < 50) continue;
		drawContours(imageContour, contours, i, Scalar(0, 255, 0), 1);

		Point temp;
		bool flag = false;
		double length;
		start = end = 0;
		double k;
		double b;
	
		vector<int> lines_index;
		//根据三点共线法检测直线，结果含非直线
		//for (int j = 20; j < size; j = j + 10)
		//{
		//	if (collinear(contours[i][j - 20], contours[i][j], contours[i][j - 10]))
		//	{
		//		if (flag = false)
		//		{
		//			start = j - 20;
		//			flag = true;
		//		}
		//		end = j;
		//	}
		//	else
		//	{
		//		flag = false;
		//		length = sqrt((contours[i][start].x - contours[i][end].x) * (contours[i][start].x - contours[i][end].x) + (contours[i][start].y - contours[i][end].y) * (contours[i][start].y - contours[i][end].y));
		//		//cout << "length: " << length << endl;
		//		double k = (double)(contours[i][end].y - contours[i][start].y) / (contours[i][end].x - contours[i][start].x);
		//		if (length > 30 && k < 0.5 && k >-0.5)
		//		for (int m = start; m <= end; m++)
		//			line(imageLines, contours[i][m], contours[i][m], Scalar(255, 0, 0), 2);
		//	}
		//}

		//用最小二乘法检测曲线
		for (int j = 0; j < contours[i].size() - 50; j += 10)
		{
			if (least_squares(&contours[i][j], 50))
			{ 
				if (flag = false)
				{
					start = j;
					flag = true;
				}
				end = j + 50;
			}
			else
			{
				flag = false;
				length = sqrt((contours[i][start].x - contours[i][end].x) * (contours[i][start].x - contours[i][end].x) \
							+ (contours[i][start].y - contours[i][end].y) * (contours[i][start].y - contours[i][end].y));
				double k = (double)(contours[i][end].y - contours[i][start].y) / (contours[i][end].x - contours[i][start].x);
				if (length > 50 && k < 0.7 && k > -0.7)
				{
					for (int m = start; m <= end; m++)
						line(imageLines, contours[i][m], contours[i][m], Scalar(255, 0, 0), 2);
					
					//用最小二乘法计算方程
					long x_sum = 0;
					long y_sum = 0;
					long xy_sum = 0;
					long x_square_sum = 0;
					long n = end - start +1;
					for (int m = start; m <= end; m++)
					{
						x_sum += contours[i][m].x;
						y_sum += contours[i][m].y;
						xy_sum += contours[i][m].x * contours[i][m].y;
						x_square_sum += contours[i][m].x*contours[i][m].x;
					}
					
					k = (double)(n * xy_sum - x_sum * y_sum) / (double)(n*x_square_sum - x_sum* x_sum);
					b = (double)y_sum / n - k * x_sum / n;
					
					Lines_K.push_back(k);
					Lines_B.push_back(b);
					
					//画出函数图
					for (int m = start; m <= end; m++)
					{
						Point p(contours[i][m].x, (int)(k*contours[i][m].x + b));
						line(LeastSq, p, p, Scalar(255, 0, 0), 2);
					}
						
				}
				start = j;
			}
		}
	}
	//for (int i = 0; i < Lines_K.size(); i++)
	//	cout << "k = " << Lines_K[i] << ", b =" << Lines_B[i]<<endl;
	ShowImage(imageContour, "contours");
	ShowImage(imageLines, "Lines");
	ShowImage(LeastSq, "LeastSq");
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
*/
#pragma region ZheyunYao

void ShowImage(Mat image, string windowName){
	namedWindow(windowName, CV_WINDOW_AUTOSIZE);
	imshow(windowName, image);
	waitKey();
}

void LoadImage(string path = "C:\\HuaWeiImage\\华为拍照_校正\\华为拍照_校正\\正常光照带假面板_2\\u_60.jpg"){
	Mat image = imread(path);
	ShowImage(image, "Original");
	h = image.rows;
	w = image.cols;
	printf("%dx%d\n", h, w);
	//Mat newCamMat = getOptimalNewCameraMatrix(camMat,distCoeffs,image.size(),-1);
	undistort(image,imageOriginal,camMat,distCoeffs);
	//Mat R;
	//Mat map1,map2;
	// initUndistortRectifyMap(camMat, distCoeffs, Mat(),
	//	 getOptimalNewCameraMatrix(camMat, distCoeffs, image.size(), 1, image.size(), 0),
	//	 Size(2*image.cols,2*image.rows), CV_16SC2, map1, map2);

//	initUndistortRectifyMap(camMat,distCoeffs,R, camMat,Size(2*image.cols,2*image.rows),CV_32FC1,map1,map2);
	//remap(image,imageOriginal,map1,map2,INTER_LINEAR);
	ShowImage(imageOriginal,"undistort");

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
	string user = "qzf";

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

		LoadImage("C:\\Users\\ZoeQIAN\\Pictures\\华为拍照\\正常光照\\60.jpg");
		//cali();
		Preprocess();
		ForegroundSeparation();
	} else
	if (user == "qyz"){
		string filename = "..\\..\\u_60_5.jpg";
		imageOriginal = imread(filename);	//载入图片
		R = imageOriginal.rows;
		C = imageOriginal.cols;

		ShowImage(imageOriginal, "imageOriginal");
		cvtColor(imageOriginal, imageGray, CV_BGR2GRAY);
		double avgGray = cvAvg(&(IplImage)imageGray).val[0];
		cout << "avgGray =" << avgGray << endl;
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
	return 0;
}
