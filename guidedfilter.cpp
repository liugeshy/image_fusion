#include "allfunction.h"



Mat GuidedFilter4(Mat I, Mat P, int type, int radius, double eps, int s)
{
	Mat I_sub, P_sub;

	resize(I, I_sub, Size(), 1.0 / s, 1.0 / s, INTER_NEAREST);
	resize(P, P_sub, Size(), 1.0 / s, 1.0 / s, INTER_NEAREST);
	int r_sub = radius / s;
	Size winSize(2 * r_sub + 1, 2 * r_sub + 1);
	//求I*I, I*P
	Mat  I2, IP;
	multiply(I_sub, I_sub, I2);
	multiply(I_sub, P_sub, IP);

	//求均值
	Mat meanI, meanP, meanI2, meanIP;
	boxFilter(I_sub, meanI, type, winSize);
	boxFilter(P_sub, meanP, type, winSize);
	boxFilter(I2, meanI2, type, winSize);
	boxFilter(IP, meanIP, type, winSize);
	//求方差/协方差
	Mat varI, covIP;
	varI = meanI2 - meanI.mul(meanI);
	covIP = meanIP - meanI.mul(meanP);
	//求系数a, b
	Mat a, b;
	varI += eps;
	divide(covIP, varI, a);
	b = meanP - a.mul(meanI);
	//a、b窗口内求平均
	Mat meanA, meanB;
	boxFilter(a, meanA, type, winSize);
	boxFilter(b, meanB, type, winSize);

	resize(meanA, meanA, I.size(), 0, 0, INTER_LINEAR);
	resize(meanB, meanB, I.size(), 0, 0, INTER_LINEAR);
	//输出
	Mat output;
	output = meanA.mul(I) + meanB;
	return output;
}


int main()
{
	Mat src = imread("C:/Users/bit/Desktop/texture_2.jpg");
	cvtColor(src, src, COLOR_BGR2GRAY);
	imshow("src", src);
	clock_t start = clock();
	Mat dst = GuidedFilter4(src, src, -1, 4, 0.0000001, 1);
	clock_t end = clock();
	cout << "running time:" << static_cast<double>(end - start) / CLOCKS_PER_SEC << endl;
	imshow("dst",dst);
	waitKey(0);
	return 0;
}