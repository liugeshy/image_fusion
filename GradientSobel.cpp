#include "allfunction.h"

#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

void GradientSobel(const Mat& img, Mat& dx, Mat& dy) {
    // 定义 Sobel 滤波器
    Mat Sv = (Mat_<int>(3, 3) << -1, -2, -1,
        0, 0, 0,
        1, 2, 1);
    Mat Sh = (Mat_<int>(3, 3) << -1, 0, 1,
        -2, 0, 2,
        -1, 0, 1);

    // 使用 filter2D 函数进行卷积
    filter2D(img, dx, CV_64F, Sh, Point(-1, -1), 0, BORDER_DEFAULT);
    filter2D(img, dy, CV_64F, Sv, Point(-1, -1), 0, BORDER_DEFAULT);
}

/*
int main() {
    // 读取输入图像 (灰度图像)
    Mat img = imread("C:/Users/bit/Desktop/1.jpg", IMREAD_GRAYSCALE);
    imwrite("C:/Users/bit/Desktop/1_gray.jpg", img);
    if (img.empty()) {
        cout << "Could not open or find the image" << endl;
        return -1;
    }

    Mat dx, dy;
    cv::Scalar sumdx = cv::sum(dx);
    cout << sumdx[0] << endl;
    // 计算 Sobel 梯度
    GradientSobel(img, dx, dy);

    // 显示结果
    imshow("Original Image", img);
    imshow("Gradient X", dx);
    imshow("Gradient Y", dy);

    waitKey(0);
    return 0;
}
*/