#include "allfunction.h"

// 假设你已经实现了这两个函数

double PS(const cv::Mat& img) {
    // 参数初始化
    int R = 32;
    int wS = 40;
    double alpha = 2.0;

    // 计算梯度
    cv::Mat gx, gy;
    GradientSobel(img, gx, gy);
    //cv::Scalar sumgx = cv::sum(gx);
    //cout << sumgx[0] << endl;
    //cv::Scalar sumgy = cv::sum(gy);
    //cout << sumgy[0] << endl;
    cv::Mat g,gx2,gy2;
    pow(gx, 2, gx2);
    pow(gy, 2, gy2);
    sqrt((gx2 + gy2), g);
    //cv::Scalar sumg = cv::sum(g);
    //cout << sumg[0] << endl;
      // 计算梯度幅值
  // 去除边界像素
    cv::Mat img_trimmed = img(cv::Range(1, img.rows - 1), cv::Range(1, img.cols - 1));
    cv::Mat g_trimmed = g(cv::Range(1, g.rows - 1), cv::Range(1, g.cols - 1));

    // 定义窗口处理函数fun1
    auto fun1 = [R](const cv::Mat& block) -> double {
        return WinAvgPE(block, R);
        };

    // 使用blockproc处理图像
    cv::Mat inf = cv::Mat::zeros(img_trimmed.size(), CV_64F);
    for (int i = 0; i <= img_trimmed.rows - wS; i += wS) {
        for (int j = 0; j <= img_trimmed.cols - wS; j += wS) {
            cv::Rect block_rect(j, i, wS, wS);
            cv::Mat block = img_trimmed(block_rect);
            inf.at<double>(i, j) = fun1(block);
        }
    }

    // 定义窗口处理函数fun2
    auto fun2 = [alpha](const cv::Mat& block) -> double {
        cv::Mat block_pow;
        cv::pow(block, alpha, block_pow);
        return cv::sum(block_pow)[0];
        };

    // 计算lambda矩阵
    cv::Mat lambda = cv::Mat::zeros(g_trimmed.size(), CV_64F);
    for (int i = 0; i <= g_trimmed.rows - wS; i += wS) {
        for (int j = 0; j <= g_trimmed.cols - wS; j += wS) {
            cv::Rect block_rect(j, i, wS, wS);
            cv::Mat block = g_trimmed(block_rect);
            lambda.at<double>(i, j) = fun2(block);
        }
    }

    // 计算特征度量F
    double numerator = cv::sum(inf.mul(lambda))[0];
    double denominator = cv::sum(lambda)[0];
    double F = numerator / denominator;

    return F;
}
/*
int main() {
    // 读取图像
    cv::Mat img = cv::imread("C:/Users/bit/Desktop/1.jpg", cv::IMREAD_GRAYSCALE);
    imwrite("C:/Users/bit/Desktop/1_gray.jpg", img);
    if (img.empty()) {
        std::cerr << "图像加载失败！" << std::endl;
        return -1;
    }

    // 调用PS函数
    double result = PS(img);

    std::cout << "特征度量 F: " << result << std::endl;

    return 0;
}
*/