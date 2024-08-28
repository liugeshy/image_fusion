#include "allfunction.h"

using namespace cv;

int main() {
    // 假设你已经加载了图像并且 Cb 是一个单通道图像
    string pic_path = "image";
    for(int i=0;i<=255;i++)
    { 
        double lambda = 100;
        int nLevel = 4;
    
        double sigma_b = 4;
        int w = floor(3 * sigma_b);
        int kernelSize = 2 * w + 1;
        Mat Cb = atanMatrix(lambda * P) / atan(lambda);
        Mat h;
        GaussianBlur(Cb, h, Size(kernelSize, kernelSize), sigma_b, sigma_b, BORDER_REFLECT);
        Cb = h;
        Mat M2n = M2[nLevel];
        Mat M1n = M1[nLevel];
        MB = Cb.mul(M2n) + (1.0 - Cb).mul(M1n);
        waitKey(0);
    }
    return 0;
}