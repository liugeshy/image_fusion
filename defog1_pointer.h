#ifndef  __DEFOG1_H
#define  __DEFOG1_H

#include <iostream>
#include "opencv.hpp"
#include <math.h>
 

using namespace std;
using namespace cv;

Mat GuidedFilter1(Mat I, Mat P, int type, int radius, double eps);
void  min_filter1(Mat &src, Mat &dst, int k_size, int* max);
Mat defog1(Mat src);


#endif