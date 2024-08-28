#include "allfunction.h"

void PickName(const string& path1, const string& path2, Mat& img1, Mat& img2, string& name, bool bGray) {
    // ��ȡͼ��
    img1 = imread(path1);
    img2 = imread(path2);

    // ���ͼ���Ƿ��ȡ�ɹ�
    if (img1.empty() || img2.empty()) {
        cerr << "Could not open or find the image(s)." << endl;
        return;
    }

    // �����Ҫ����ͼ��ת��Ϊ�Ҷ�ͼ��
    if (bGray) {
        if (img1.channels() == 3) {
            cvtColor(img1, img1, COLOR_BGR2GRAY);
            cvtColor(img2, img2, COLOR_BGR2GRAY);
        }
    }

    // ��ͼ��ת��Ϊdouble����
    //img1.convertTo(img1, CV_64FC3, 1 / 255.0);
    //img2.convertTo(img2, CV_64FC3, 1 / 255.0);

    // ��ȡͼ���ļ���������·������չ����
    size_t lastSlash = path1.find_last_of("/\\");
    size_t lastDot = path1.find_last_of(".");
    if (lastDot == string::npos) {
        lastDot = path1.length();
    }
    name = path1.substr(lastSlash + 1, lastDot - lastSlash - 1);
}