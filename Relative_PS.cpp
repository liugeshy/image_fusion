#include "allfunction.h"

double Relative_PS(const Mat& imgIR, const Mat& imgTV) {
    double ps_ir = PS(imgIR);
    double ps_tv = PS(imgTV);
    double relative_ps = ps_ir / ps_tv;
    return relative_ps;
}/*
int main()
{
    cv::Mat img1 = cv::imread("C:/Users/bit/Desktop/1.jpg", cv::IMREAD_GRAYSCALE);
    cv::Mat img2 = cv::imread("C:/Users/bit/Desktop/texture_1_gray.jpg", cv::IMREAD_GRAYSCALE);
    double i = Relative_PS(img1, img2);
    cout << i << endl;
}
*/