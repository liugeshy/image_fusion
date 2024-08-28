#ifndef  __DEFOG2_H
#define  __DEFOG2_H

#include <iostream>
#include "opencv.hpp"
#include <math.h>
 

using namespace std;
using namespace cv;

Mat GuidedFilter2(Mat I, Mat P, int type, int radius, double eps);
void  min_filter2(Mat &src, Mat &dst, int k_size, int* max);
Mat defog2(Mat src);
void box_filter(Mat& img, Mat& result, int k_size);

#endif