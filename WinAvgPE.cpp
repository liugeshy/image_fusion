#include "allfunction.h"

using namespace cv;
using namespace std;

// Mannos-Skarison 视觉模型函数
Mat MannosSkarision(const Mat& r) {
    Mat A, B;
    pow((0.114 * r), 1.1, A);
    exp(-A, B);
    Mat fc = 2.6 * (0.0192 + 0.114 * r).mul(B);
    return fc;
}

// 计算图像的加权平均能量
double WinAvgPE(const Mat& img, double R) {
    // 计算二维傅里叶变换并移到频域中心
    Mat imgFloat;
    img.convertTo(imgFloat, CV_64F);  // 将图像转换为双精度浮点型
    Mat planes[] = { Mat_<double>(imgFloat), Mat::zeros(img.size(), CV_64F) };
    Mat complexImg;
    merge(planes, 2, complexImg);

    dft(complexImg, complexImg);

    split(complexImg, planes);
    Mat magnitudeImg;
    magnitude(planes[0], planes[1], magnitudeImg);  // 幅度谱
    //magnitudeImg = magnitudeImg(Rect(1, 1, img.cols - 2, img.rows - 2));  // 去掉边界
    Mat magShifted;
    fftshift(magnitudeImg, magShifted);

    // 频率空间坐标
    int m = magShifted.rows;
    int n = magShifted.cols;
    //cout << m << endl;
    //cout << n << endl;
    Mat u, v;
    freqspace(m, n, u, v);
    //cv::Scalar sumu = cv::sum(u);
    //cout << sumu[0] << endl;
    //cv::Scalar sumv = cv::sum(v);
    //cout << sumv[0] << endl;
    u = u / 2 * R;
    v = v / 2 * R;
    Mat r,u2,v2;
    pow(u, 2, u2);
    pow(v, 2, v2);
    sqrt((u2+v2),r);
    //cv::Scalar sum2 = cv::sum(r);
    //cout << sum2[0] << endl;
    // 计算加权函数
    Mat fw = MannosSkarision(r);
    //cv::Scalar sumfw = cv::sum(fw);
    //cout << sumfw[0] << endl;
    // 加权频谱和能量计算
    Mat F = magShifted.mul(fw);
    double sumF = sum(F.mul(F))[0];  // 计算平方和
    double totalEnergy = sqrt(sumF) / (m * n);

    return totalEnergy;
}

// 辅助函数：生成频率空间坐标
void freqspace(int m, int n, cv::Mat& u, cv::Mat& v) {
    u = cv::Mat(m, n, CV_64F);
    v = cv::Mat(m, n, CV_64F);

    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            u.at<double>(i, j) = (j - n / 2.0) / (n / 2.0);
            v.at<double>(i, j) = (i - m / 2.0) / (m / 2.0);
        }
    }
}

// 辅助函数：将频谱移动到中心
void fftshift(const Mat& inputImg, Mat& outputImg) {
    outputImg = inputImg.clone();
    int cx = outputImg.cols / 2;
    int cy = outputImg.rows / 2;

    Mat q0(outputImg, Rect(0, 0, cx, cy));   // Top-Left - Create a ROI per quadrant
    Mat q1(outputImg, Rect(cx, 0, cx, cy));  // Top-Right
    Mat q2(outputImg, Rect(0, cy, cx, cy));  // Bottom-Left
    Mat q3(outputImg, Rect(cx, cy, cx, cy)); // Bottom-Right

    // Swap quadrants (Top-Left with Bottom-Right)
    Mat tmp;
    q0.copyTo(tmp);
    q3.copyTo(q0);
    tmp.copyTo(q3);

    // Swap quadrant (Top-Right with Bottom-Left)
    q1.copyTo(tmp);
    q2.copyTo(q1);
    tmp.copyTo(q2);
}

/*
int main() {
    // 读取输入图像 (灰度图像)
    Mat img = imread("C:/Users/bit/Desktop/texture_2.jpg", IMREAD_GRAYSCALE);
    imwrite("C:/Users/bit/Desktop/texture_2_gray.jpg", img);
    if (img.empty()) {
        cout << "Could not open or find the image" << endl;
        return -1;
    }

    // 计算加权平均能量
    double R = 32.0;
    double result = WinAvgPE(img, R);

    // 输出结果
    cout << "Weighted Average Energy: " << result << endl;

    return 0;
}
*/