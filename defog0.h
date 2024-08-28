#ifndef  __DEFOG0_H
#define  __DEFOG0_H

#include <iostream>
#include "opencv.hpp"
#include <math.h>

using namespace std;
using namespace cv;

 
void min_filter0(Mat &src, Mat &dst, int k_size);
Mat defog0(Mat src);


#endif