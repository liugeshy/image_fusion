#include "allfunction.h"

using namespace cv;
using namespace std;

// Mannos-Skarison �Ӿ�ģ�ͺ���
Mat MannosSkarision(const Mat& r) {
    Mat A, B;
    pow((0.114 * r), 1.1, A);
    exp(-A, B);
    Mat fc = 2.6 * (0.0192 + 0.114 * r).mul(B);
    return fc;
}

// ����ͼ��ļ�Ȩƽ������
double WinAvgPE(const Mat& img, double R) {
    // �����ά����Ҷ�任���Ƶ�Ƶ������
    Mat imgFloat;
    img.convertTo(imgFloat, CV_64F);  // ��ͼ��ת��Ϊ˫���ȸ�����
    Mat planes[] = { Mat_<double>(imgFloat), Mat::zeros(img.size(), CV_64F) };
    Mat complexImg;
    merge(planes, 2, complexImg);

    dft(complexImg, complexImg);

    split(complexImg, planes);
    Mat magnitudeImg;
    magnitude(planes[0], planes[1], magnitudeImg);  // ������
    //magnitudeImg = magnitudeImg(Rect(1, 1, img.cols - 2, img.rows - 2));  // ȥ���߽�
    Mat magShifted;
    fftshift(magnitudeImg, magShifted);

    // Ƶ�ʿռ�����
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
    // �����Ȩ����
    Mat fw = MannosSkarision(r);
    //cv::Scalar sumfw = cv::sum(fw);
    //cout << sumfw[0] << endl;
    // ��ȨƵ�׺���������
    Mat F = magShifted.mul(fw);
    double sumF = sum(F.mul(F))[0];  // ����ƽ����
    double totalEnergy = sqrt(sumF) / (m * n);

    return totalEnergy;
}

// ��������������Ƶ�ʿռ�����
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

// ������������Ƶ���ƶ�������
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
    // ��ȡ����ͼ�� (�Ҷ�ͼ��)
    Mat img = imread("C:/Users/bit/Desktop/texture_2.jpg", IMREAD_GRAYSCALE);
    imwrite("C:/Users/bit/Desktop/texture_2_gray.jpg", img);
    if (img.empty()) {
        cout << "Could not open or find the image" << endl;
        return -1;
    }

    // �����Ȩƽ������
    double R = 32.0;
    double result = WinAvgPE(img, R);

    // ������
    cout << "Weighted Average Energy: " << result << endl;

    return 0;
}
*/