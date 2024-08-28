#include "allfunction.h"

Mat atanMatrix(const Mat& src) {
    Mat dst = src.clone();  // ����һ���� src ��С��ͬ�ľ���
    dst.forEach<double>([](double& pixel, const int* position) -> void {
        pixel = atan(pixel);  // ��ÿ������Ӧ�� atan ����
        });
    return dst;
}
/*
int main() {
    // ʾ������
    Mat P = (Mat_<int>(3, 3) << 1, 2, 3,
        4, 5, 6,
        7, 8, 9);

    // Lambda ����
    double lambda = 100;

    // ���� Cj = atan(lambda * P) / atan(lambda)
    Mat Cj = P * lambda;  // ��Ԫ�س��� lambda
    Cj = atanMatrix(Cj);  // ��ÿ��Ԫ��Ӧ�� atan
    Cj /= atan(lambda);   // ���� atan(lambda)

    // ��ʾ���
    cout << "Cj matrix:" << endl << Cj << endl;

    return 0;
}*/