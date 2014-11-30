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
int leftBoundary;
//double cam[] = { 386.951, 0, 233.539, 0, 384.132, 352.891, 0, 0, 1 };
//double dis[] = { -0.44601, 0.266848, -0.00143158, 0.000143152, -0.103006 };
//以上是之前的参数

double cam[] = { 283.561, 0, 246, 0, 285.903, 334.103, 0, 0, 1 };
double dis[] = { -0.313793, 0.122695, 0.00123624, -0.000849487, -0.0250905 };

Mat camMat(3, 3, CV_64F, cam), distCoeffs(1, 5, CV_64F, dis);
Mat imageOriginal, imageGray, imageCanny, imageOutput, imageLabel, imageForeground;
Mat imageOriginalT, imageCannyT, imageLabelT;
Mat mask, makers;
vector<Vec3d> lines;
vector<vector<Point>> contours;

#pragma region Qian Zifei
string fname("C:\\Users\\ZoeQIAN\\Pictures\\华为拍照-20141128\\");
string results("C:\\Users\\ZoeQIAN\\Pictures\\Test\\");
string type("机柜A-中心下移\\");
string num("3");
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
	//imshow("hi",result);
	//waitKey();
	threshold(result, result, 254, 255, THRESH_BINARY);
	//imshow("byebye~",result);
	//waitKey();
	bitwise_and(imageOriginal, imageOriginal, imageForeground, result);
	imshow("Foreground", imageForeground);
	waitKey();
}

void cali()
{
	Mat image;
	vector< vector<Point2f> > image_points;
	vector< vector<Point3f> > object_points;
	vector<Point2f> tmp;
	vector<Point3f> chessboard;
	Size boardSize(6, 9);
	int brdnum = 8;

	for (int i = 0; i != 9; i++)
	for (int j = 0; j != 6; j++)
		chessboard.push_back(Point3i(i, j, 0));


	// bool found = findChessboardCorners(image,boardSize,tmp,CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FAST_CHECK | CV_CALIB_CB_NORMALIZE_IMAGE);
	// if(found)
	// drawChessboardCorners(image,boardSize,Mat(tmp),found);
	// imshow("hello~",image);
	for (int i = 0; i != brdnum; i++){
		string num;
		stringstream ss;
		ss << i + 1;
		ss >> num;
		image = imread("C:\\Users\\ZoeQIAN\\Pictures\\chessboard\\" + num + ".jpg");
		//imshow("yi",image);
		//	copyMakeBorder(image,image,image.rows/5,image.rows/5,image.cols/5,image.cols/5,BORDER_CONSTANT,Scalar(255));
		bool found = findChessboardCorners(image, boardSize, tmp, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FAST_CHECK | CV_CALIB_CB_NORMALIZE_IMAGE);
		if (found){
			image_points.push_back(tmp);
			object_points.push_back(chessboard);
			drawChessboardCorners(image, boardSize, Mat(tmp), found);
			//imshow("hello~",image);
			 //waitKey();
		}
		//waitKey();
	}
	Mat camMat_est;
	Mat distCoeffs_est;
	vector<Mat> rvecs, tvecs;
	cout << "Calibrating...";
	calibrateCamera(object_points, image_points, Size(image.rows, image.cols), camMat_est, distCoeffs_est, rvecs, tvecs);
	//for (int i = 0; i != camMat.rows; i++){
	//	for (int j = 0; j != camMat.cols; j++)
	//		cout << camMat.at<double>(i, j) << "   " << flush;
	//	cout << endl;
	//}
	//for (int i = 0; i != distCoeffs.rows; i++){
	//	for (int j = 0; j != distCoeffs.cols; j++)
	//		cout << distCoeffs.at<double>(i, j) << "   " << flush;
	//	cout << endl;
	//}
	for (int i = 0; i != camMat.rows; i++){
		for (int j = 0; j != camMat.cols; j++)
			cout << camMat_est.at<double>(i, j) << "   " << flush;
		cout << endl;
	}
	for (int i = 0; i != distCoeffs.rows; i++){
		for (int j = 0; j != distCoeffs.cols; j++)
			cout << distCoeffs_est.at<double>(i, j) << "   " << flush;
		cout << endl;
	}
	system("PAUSE");
	cout << "Done" << endl;
	cout << "Undistort..." << endl;
	string name;
	string type;
	//cout << "Enter the type:" << endl;
	//cin >> type;
	//while (1){
	//	cout << "Please enter the image name, q for exit:" << endl;
	//	cin >> name;
	//	if (name == "q")
	//		break;
	//	image = imread("C:\\Users\\ZoeQIAN\\Pictures\\huawei\\" + name + ".jpg");
	//	Mat timg;
	//	imshow("Original", image);
	//	waitKey();
	//	//给图片加边
	//	/*  copyMakeBorder(image,timg,image.rows/5,image.rows/5,image.cols/5,image.cols/5,BORDER_CONSTANT,Scalar(255));
	//	imshow("nani",timg);*/
	//	//imshow("ale",image);
	//	Mat undis(image.size(), CV_8U, Scalar(255));
	//	//Mat undis;
	//	undistort(image, undis, camMat_est, distCoeffs_est);
	//	cout << "Done" << endl;
	//	imshow("Undistort", undis);
	//	waitKey();
	//	char tmp;
	//	cin >> tmp;
	//	destroyWindow("Undistort");
	//}
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

void generate_RGB_files()
{
	//取RGB值存储
	FILE* Rfp, *Gfp, *Bfp;
	Rfp = fopen("imageOriginal_R.c", "w+");
	Gfp = fopen("imageOriginal_G.c", "w+");
	Bfp = fopen("imageOriginal_B.c", "w+");

	fprintf(Rfp, "#include \"2410lib.h\"\nUINT8T imageOriginal_R[%d][%d] = {\n", R, C);
	fprintf(Gfp, "#include \"2410lib.h\"\nUINT8T imageOriginal_G[%d][%d] = {\n", R, C);
	fprintf(Bfp, "#include \"2410lib.h\"\nUINT8T imageOriginal_B[%d][%d] = {\n", R, C);
	CvScalar s;
	for (int i = 0; i < R; i++)
	{
		fprintf(Rfp, "{ ");
		fprintf(Gfp, "{ ");
		fprintf(Bfp, "{ ");
		for (int j = 0; j < C; j++)
		{
			s = cvGet2D(&(IplImage)imageOriginal, i, j);
			fprintf(Bfp, "%d", (int)s.val[0]);//B
			fprintf(Gfp, "%d", (int)s.val[1]);//G
			fprintf(Rfp, "%d", (int)s.val[2]);//R
			if (j != C - 1){
				fprintf(Bfp, ", ");
				fprintf(Gfp, ", ");
				fprintf(Rfp, ", ");
			}

			if (j % 20 == 0&& j !=0)
			{
				fprintf(Bfp, "\n ");
				fprintf(Gfp, "\n ");
				fprintf(Rfp, "\n ");
			}
		}
		fprintf(Rfp, "},\n");
		fprintf(Gfp, "},\n");
		fprintf(Bfp, "},\n");
	}
	fseek(Rfp, -3, SEEK_CUR);
	fseek(Gfp, -3, SEEK_CUR);
	fseek(Bfp, -3, SEEK_CUR);
	fprintf(Rfp, "};");
	fprintf(Gfp, "};");
	fprintf(Bfp, "};");
	fclose(Rfp);
	fclose(Gfp);
	fclose(Bfp);
}

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
	//imwrite(results+type+num+"\\"+windowName+".jpg",image);
	waitKey();
}

void LoadImage(string path = "C:\\HuaWeiImage\\华为拍照\\正常光照\\60~90.jpg"){
	Mat image = imread(path);
	ShowImage(image, "Original");
	imwrite("C:\\Users\\ZoeQIAN\\Desktop\\image.tif",image);
	//Mat newCamMat = getOptimalNewCameraMatrix(camMat,distCoeffs,image.size(),-1);
	undistort(image, imageOriginal, camMat, distCoeffs);
	h = imageOriginal.rows;
	w = imageOriginal.cols;
	printf("%dx%d\n", h, w);
	//Mat R;
	//Mat map1,map2;
	// initUndistortRectifyMap(camMat, distCoeffs, Mat(),
	//	 getOptimalNewCameraMatrix(camMat, distCoeffs, image.size(), 1, image.size(), 0),
	//	 Size(2*image.cols,2*image.rows), CV_16SC2, map1, map2);

	//	initUndistortRectifyMap(camMat,distCoeffs,R, camMat,Size(2*image.cols,2*image.rows),CV_32FC1,map1,map2);
	//remap(image,imageOriginal,map1,map2,INTER_LINEAR);
	ShowImage(imageOriginal, "undistort");

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
	dilate(imageCanny, imageTemp1, Mat(3, 3, CV_8U), Point(-1, -1), 4);
	erode(imageTemp1, imageTemp2, Mat(3, 3, CV_8U), Point(-1, -1), 3);

	//ShowImage(imageTemp1, "Dilate");
	//ShowImage(imageTemp2, "Erode");
	Canny(imageTemp2, imageCanny, 100, 200);
	//imageCanny = imageTemp2.clone();
	//ShowImage(imageCanny, "Optimized Canny");
}

void AdaptiveCanny(){
	double low = 0.0, high = 0.0; //Thresholds for Canny edge detection
	cvtColor(imageOriginal, imageGray, CV_BGR2GRAY);
	//equalizeHist(imageGray, imageEqualHist);//灰度图象直方图均衡化
	double cannyThreshold = Otsu(&(IplImage)imageOriginal);
	//cannyThreshold = 100;
	AdaptiveFindThreshold(&imageGray, &low, &high);
	cout << "Low: " << low << endl << "High: " << high << endl;
	//Canny(imageGray, imageCanny, low, high);
	Canny(imageGray, imageCanny, cannyThreshold, cannyThreshold * 2);
	//ShowImage(imageCanny, "canny1");
	//OptimizeCanny();
}

void DetectContours(double thresh = 100){
	AdaptiveCanny();
	line(imageCanny, Point(0, 0), Point(0, h), Scalar(255, 255, 255), 1);
	ShowImage(imageCanny, "Canny");
	//OptimizeCanny();
	findContours(imageCanny, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
	imageOutput = imageOriginal.clone();
	for (int i = 0; i < contours.size(); i++){
		//if (checkContour(contours[i])){
			drawContours(imageOutput, contours, i, Scalar(0, 255, 0), 1.2);
		//}
	}
	ShowImage(imageOutput, "Contours");
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
	gc->setSmoothCost(0, 2, 300 / varity);
	gc->setSmoothCost(2, 0, 300 / varity);
	gc->setSmoothCost(1, 2, 300 / varity);
	gc->setSmoothCost(2, 1, 300 / varity);

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

	Dilate(2, 6, 4);
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

	for (int j = 0; j < w; j++){
		int count = 0;
		for (int i = 0; i < h; i++) if (makers.at<int>(i, j) == 2) count++;
		if (count > h / 2) {
			leftBoundary = j;
			break;
		}
	}
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
		for (int j = 0; j < w; j++) if (blockCount[i][block[i][j]] < w / 6) label[i][j] = 3;
	}

}

Vec3d CalcLine(vector<Point> points){
	int n = points.size();
	double x_sum = 0;
	double y_sum = 0;
	double xy_sum = 0;
	double x_square_sum = 0;
	for (int i = 0; i < n; i++){
		x_sum += points[i].x;
		y_sum += points[i].y;
		xy_sum += points[i].x * points[i].y;
		x_square_sum += points[i].x * points[i].x;
	}
	double k = -1;
	double b = -1;
	double eps = 0.01;
	if (abs(n * x_square_sum - x_sum * x_sum) > eps) {
		k = (n * xy_sum - x_sum * y_sum) / (n * x_square_sum - x_sum * x_sum);
		b = y_sum / n - k * x_sum / n;
	}
	
	double x_mean = x_sum / n;
	double y_mean = y_sum / n;
	double x_temp = 0, y_temp = 0;
	for (int i = 0; i < n; i++)
	{
		x_temp += (points[i].x - x_mean) *(points[i].x - x_mean);
		y_temp += (points[i].y - y_mean) * (points[i].y - y_mean);
	}
	double x_stdev = sqrt(x_temp / n);
	double y_stdev = sqrt(y_temp / n);
	double r = 0;
	for (int i = 0; i < n; i++)
		r += ((points[i].x - x_mean) / x_stdev)*((points[i].y - y_mean) / y_stdev);
	r /= n;
	return Vec3d(k, b, r);
}

bool checkLine(Vec3d line){
	if (line[0] < -1) return false;
	if (line[0] > 0.5) return false;
	//if (line[1] > h * 19 / 20) return false;
	//if (line[1] < h / 20) return false;
	return true;
}

bool OutOfBox(vector<Point> contour){
	int x_sum = 0;
	for (int i = 0; i < contour.size(); i++) x_sum += contour[i].x;
	if (x_sum < leftBoundary * contour.size()) return false; else return true;
}
void DetectLines(){
	int lengthLimit = 90;
	int split = 30;
	vector<Point> points;
	vector<Point> pointsTemp;
	Vec3d result;
	
	imageOutput = imageOriginal.clone();
	lines.clear();
	for each (vector<Point> contour in contours)
	{
		if (contour.size() < lengthLimit) continue;
		if (OutOfBox(contour)) continue;
		points.clear();
		for (int t = 0; t + split - 1 < contour.size(); t += split){
			pointsTemp.clear();
			for (int i = 0; i < split; i++) points.push_back(contour[t + i]);
			for (int i = 0; i < split; i++) pointsTemp.push_back(contour[t + i]);
			
			if ((abs(CalcLine(points)[2]) < 0.90) || (abs(CalcLine(pointsTemp)[2]) < 0.93)){
				for (int i = 0; i < split; i++) points.pop_back();
				if (points.size() >=  lengthLimit){
					result = CalcLine(points);
					if (checkLine(result)) lines.push_back(result);
					for (int i = 0; i < points.size(); i++) line(imageOutput, points[i], points[i], Scalar(0, 255, 0), 1.5);
					printf("k = %lf, b = %lf\n", result[0], result[1]);
					//ShowImage(imageOutput, "line");
				}
				points.clear();
			}
		}
		if (points.size() > lengthLimit){
			result = CalcLine(points);
			if (checkLine(result)) lines.push_back(result);
			for (int i = 0; i < points.size(); i++) line(imageOutput, points[i], points[i], Scalar(0, 255, 0), 1.5);
			printf("k = %lf, b = %lf\n", result[0], result[1]);
			//ShowImage(imageOutput, "line");
		}
	}
	for (int i = 0; i < lines.size(); i++){
		double k = lines[i][0];
		double b = lines[i][1];
		Point st = Point(0, b);
		Point en = Point(w, b + w * k);
		line(imageOutput, st, en, Scalar(255, 0, 0), 1.5);
		printf("k = %lf, b = %lf\n", k, b);
	}
	ShowImage(imageOutput, "line");
}

void LineSort(){
	int n = lines.size();
	for (int i = 0; i < n; i++)
	for (int j = i + 1; j < n; j++){
		if (lines[i][1] > lines[j][1]){
			Vec3d temp = lines[i];
			lines[i] = lines[j];
			lines[j] = temp;
		}
	}
	vector<int> f;
	vector<int> v;
	f.clear();
	v.clear();
	for (int i = 0; i < n; i++){
		f.push_back(0);
		v.push_back(-1);
		for (int j = 0; j < i; j++) if((lines[i][0] < lines[j][0]) && (lines[i][0] * w / 2 + lines[i][1] > lines[j][0] * w / 2 + lines[j][1])) 
		if (f[j] + 1 > f[i]){
			f[i] = f[j] + 1;
			v[i] = j;
		}
		if (f[i] == 0) f[i] = 1;
		printf("k = %lf, b = %lf, bb = %lf, f = %d, v = %d\n", lines[i][0], lines[i][1], lines[i][0] * w / 2 + lines[i][1], f[i], v[i]);
	}
	int max = 0;
	int l = 0;
	for (int i = 0; i < n; i++) if (f[i] > max){
		max = f[i];
		l = i;
	}
	printf("max: %d\n", max);
	vector<Vec3d> linesSorted;
	linesSorted.clear();
	while (l != -1){
		linesSorted.push_back(lines[l]);
		l = v[l];
	}
	lines = linesSorted;
	imageOutput = imageOriginal.clone();
	for (int i = 0; i < lines.size(); i++){
		double k = lines[i][0];
		double b = lines[i][1];
		Point st = Point(0, b);
		Point en = Point(w, b + w * k);
		line(imageOutput, st, en, Scalar(255, 0, 0), 1.5);
		printf("k = %lf, b = %lf\n", k, b);
	}
	ShowImage(imageOutput, "lineSorted");
}
void PerspectiveTransfrom(){
	LineSort();
	int boundary = leftBoundary;
	int n = lines.size();
	int ul = 0;
	int dl = n - 1;
	bool flag = true;
	while (flag){
		flag = false;
		if ((dl - ul > 1) && (lines[dl][1] - lines[ul + 1][1] > h / 8)){
			ul++;
			flag = true;
		}
		if ((dl - ul > 1) && (lines[dl - 1][1] - lines[ul][1] > h / 8)){
			dl--;
			flag = true;
		}
	}
	double k1 = lines[ul][0];
	double b1 = lines[ul][1];
	double k2 = lines[dl][0];
	double b2 = lines[dl][1];
	double t1 = -b1 / b2;
	double t2 = -(b1 - h) / (b2 - h);
	vector<Point2f> corners(4);
	vector<Point2f> cornersT(4);
	printf("k1 = %lf, b1 = %lf\n", k1, b1);
	printf("k2 = %lf, b2 = %lf\n", k2, b2);

	imageOutput = imageOriginal.clone();
	Point st = Point(0, b1);
	Point en = Point(w, b1 + w * k1);
	line(imageOutput, st, en, Scalar(255, 0, 0), 1.5);
	st = Point(0, b2);
	en = Point(w, b2 + w * k2);
	line(imageOutput, st, en, Scalar(255, 0, 0), 1.5);
	
	corners[0] = Point2f(0, 0);
	corners[1] = Point2f(boundary, ((k1 + t1 * k2) * boundary + (b1 + t1 * b2)) / (1 + t1));
	corners[2] = Point2f(boundary, ((k1 + t2 * k2) * boundary + (b1 + t2 * b2)) / (1 + t2));
	corners[3] = Point2f(0, h -1);
	for (int i = 0; i < 4; i++) {
		line(imageOutput, corners[i], corners[i], Scalar(0, 0, 255), 10);
		//ShowImage(imageOutput, "corners");
		printf("(%lf, %lf)\n", corners[i].x, corners[i].y);
	}

	cornersT[0] = Point2f(0, 0);
	cornersT[1] = Point2f(w - 1, 0);
	cornersT[2] = Point2f(w - 1, h - 1);
	cornersT[3] = Point2f(0, h - 1);

	Mat transform = getPerspectiveTransform(corners, cornersT);
	warpPerspective(imageOriginal, imageOriginalT, transform,Size(w, h));
	ShowImage(imageOriginalT, "trans");
}

int findLeftEdge(){
	for (int x = 0; x < w; x++){
		int count = 0;
		for (int y = 0; y < h; y++) if (label[y][x] == 255) count++;
		if (count > h / 2) return x;
	}
	return w - 1;
}

void DetectSpace_3(){
	double thresh = 100;
	int boundary = findLeftEdge();
	printf("boundary: %d\n", boundary);
	imageOutput = imageOriginal.clone();
	Canny(imageLabel, imageCanny, thresh, thresh * 2);
	line(imageCanny, Point(0, 0), Point(0, h - 1), Scalar(255, 255, 255), 1);
	line(imageCanny, Point(0, 0), Point(w - 1, 0), Scalar(255, 255, 255), 1);
	ShowImage(imageCanny, "Canny");
	OptimizeCanny();
	findContours(imageCanny, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
	int index = 0;
	for each (vector<Point> contour in contours)
	{
		if (contourArea(contour) < boundary * h / 40) continue;
		if (contourArea(contour) > w * h / 5) continue;
		printf("%lf\n", contourArea(contour));
		Point2f vertices[4];
		RotatedRect rect = minAreaRect(contour);
		rect.points(vertices);
		int minx = w;
		int maxx = 0;
		for (int i = 0; i < 4; i++) if (vertices[i].x > maxx) maxx = vertices[i].x;
		for (int i = 0; i < 4; i++) if (vertices[i].x < minx) minx = vertices[i].x;
		if (minx > boundary / 10) continue;
		if (maxx < boundary / 2) continue;
		for (int i = 0; i < 4; i++){
			line(imageOutput, vertices[i], vertices[(i + 1) % 4], Scalar(0, 0, 255), 2);
		}

		//drawContours(imageOutput, contours, index++, Scalar(255, 0, 0), 2);
			//ShowImage(imageOutput, "result");
	}
	ShowImage(imageOutput, "result");
}

#pragma endregion


int main(){
	string user = "qzf";

	if (user == "yzy")
	{
		//LoadImage("C:\\HuaWeiImage\\华为拍照\\正常光照带假面板\\jpeg_20140912_150217.jpg");
		LoadImage("C:\\HuaWeiImage\\华为拍照\\正常光照部分拆除\\jpeg_20140912_152553.jpg");	//载入图片，鱼眼矫正
		//return 0;

		Preprocess();		 					//预处理，暂时没什么用，可加入光照调整
		DetectSpace_1();
		DetectContours();						//边界检测，主要包括canny和findContours
		DetectLines();							//从contours中提取直线
		DetectSpace_1();						//前背景分割
		DetectContours();						//重新进行canny，用于图像分割
		MultiLabelGraphCut();					//图像分割
		ImageLabelInput();						//label图象显示
		DetectSpace_0();						//杂物过滤（横向腐蚀）
		ImageLabelInput();						//label图象显示
		DetectSpace_2();						//联通度优化
		ImageLabelInput();						//label图象显示
		DetectSpace_3();						//空余空间校验，结果显示

		waitKey();
		destroyAllWindows();
	}
	else
	if (user == "qzf"){

		//LoadImage("C:\\HuaWeiImage\\华为拍照\\正常光照带假面板\\jpeg_20140912_150217.jpg");

		//cali();
		LoadImage(fname+type+num+".jpg");		//载入图片，鱼眼矫正

		Preprocess();							//预处理，暂时没什么用，可加入光照调整
		DetectSpace_1();
		DetectContours();						//边界检测，主要包括canny和findContours
		DetectLines();							//从contours中提取直线
		PerspectiveTransfrom();					//视角变换
		imageOriginal = imageOriginalT.clone();	//将视角变换后的图替代原图
		DetectSpace_1();						//前背景分割
		DetectContours();						//重新进行canny，用于图像分割
		MultiLabelGraphCut();					//图像分割
		ImageLabelInput();						//label图象显示
		DetectSpace_0();						//杂物过滤（横向腐蚀）
		ImageLabelInput();						//label图象显示
		DetectSpace_2();						//联通度优化
		ImageLabelInput();						//label图象显示
		DetectSpace_3();						//空余空间校验，结果显示

		waitKey();
		destroyAllWindows();
	}
	else
	if (user == "qyz"){
		//string filename = "E:\\VS2013_pro\\2014_9_12\\正常光照\\60.jpg";
		string filename = "..\\..\\60_1.jpg";
		LoadImage(filename);
		R = imageOriginal.rows;
		C = imageOriginal.cols;
		
		//ShowImage(imageOriginal, "imageOriginal");
		cvtColor(imageOriginal, imageGray, CV_BGR2GRAY);
		double avgGray = cvAvg(&(IplImage)imageGray).val[0];
		cout << "avgGray =" << avgGray << endl;
		
		if (avgGray > 110.0) //获取平均亮度
		{
			retinex();
			ShowImage(imageOriginal, "Retinex");
			//detectEdges(imageRetinex);
		}
		ForegroundSeparation();
		detectEdges(imageForeground);
		//detectLines();
		//checkEdges();
		waitKey();
	}
	return 0;
}
