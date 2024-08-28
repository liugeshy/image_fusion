#ifndef  __DEFOG3_H
#define  __DEFOG3_H

#include <iostream>
#include "opencv.hpp"
#include <math.h>
 

using namespace std;
using namespace cv;

Mat GuidedFilter3(Mat I, Mat P, int type, int radius, double eps,int s);
void  min_filter3(Mat &src, Mat &dst, int k_size, int* max);
Mat defog3(Mat src);


#endif