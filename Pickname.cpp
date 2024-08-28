#include "allfunction.h"

void PickName(const string& path1, const string& path2, Mat& img1, Mat& img2, string& name, bool bGray) {
    // 读取图像
    img1 = imread(path1);
    img2 = imread(path2);

    // 检查图像是否读取成功
    if (img1.empty() || img2.empty()) {
        cerr << "Could not open or find the image(s)." << endl;
        return;
    }

    // 如果需要，将图像转换为灰度图像
    if (bGray) {
        if (img1.channels() == 3) {
            cvtColor(img1, img1, COLOR_BGR2GRAY);
            cvtColor(img2, img2, COLOR_BGR2GRAY);
        }
    }

    // 将图像转换为double类型
    //img1.convertTo(img1, CV_64FC3, 1 / 255.0);
    //img2.convertTo(img2, CV_64FC3, 1 / 255.0);

    // 提取图像文件名（不带路径和扩展名）
    size_t lastSlash = path1.find_last_of("/\\");
    size_t lastDot = path1.find_last_of(".");
    if (lastDot == string::npos) {
        lastDot = path1.length();
    }
    name = path1.substr(lastSlash + 1, lastDot - lastSlash - 1);
}