#ifndef  __ALLFUNCTION_H
#define  __ALLFUNCTION_H

#include <iostream>
#include "opencv.hpp"
#include <math.h>
#include <ctime>

using namespace std;
using namespace cv;


void PickName(const string& path1, const string& path2, Mat& img1, Mat& img2, string& name, bool bGray = true);
Mat GuidedFilter4(Mat I, Mat P, int type, int radius, double eps, int s);
void GradientSobel(const Mat& img, Mat& dx, Mat& dy);
void freqspace(int m, int n, Mat& u, Mat& v);
void fftshift(const Mat& inputImg, Mat& outputImg);
double WinAvgPE(const Mat& img, double R);
Mat MannosSkarision(const Mat& r);
double PS(const cv::Mat& img);
cv::Mat Ehn_GF(const cv::Mat& img);
double Relative_PS(const Mat& imgIR, const Mat& imgTV);
Mat atanMatrix(const Mat& src);
Mat Ehn_GF_new(Mat src);

#endif