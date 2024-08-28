#include "allfunction.h"

Mat atanMatrix(const Mat& src) {
    Mat dst = src.clone();  // 创建一个与 src 大小相同的矩阵
    dst.forEach<double>([](double& pixel, const int* position) -> void {
        pixel = atan(pixel);  // 对每个像素应用 atan 函数
        });
    return dst;
}
/*
int main() {
    // 示例矩阵
    Mat P = (Mat_<int>(3, 3) << 1, 2, 3,
        4, 5, 6,
        7, 8, 9);

    // Lambda 参数
    double lambda = 100;

    // 计算 Cj = atan(lambda * P) / atan(lambda)
    Mat Cj = P * lambda;  // 逐元素乘以 lambda
    Cj = atanMatrix(Cj);  // 对每个元素应用 atan
    Cj /= atan(lambda);   // 除以 atan(lambda)

    // 显示结果
    cout << "Cj matrix:" << endl << Cj << endl;

    return 0;
}*/