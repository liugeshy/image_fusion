#include "defog1_pointer.h"

Mat GuidedFilter1(Mat I, Mat P, int type, int radius, double eps)
{
	Size winSize(2 * radius + 1, 2 * radius + 1);
	//求I*I, I*P
	Mat  I2, IP;
	multiply(I, I, I2);
	multiply(I, P, IP);

	//求均值
	Mat meanI, meanP, meanI2, meanIP;
	boxFilter(I, meanI, type, winSize);
	boxFilter(P, meanP, type, winSize);
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
	//输出
	Mat output;
	output = meanA.mul(I) + meanB;
	return output;
}


void  min_filter1(Mat &src, Mat &dst, int k_size,int* max)
{
	deque <int>  L;

	uchar* src_p = src.ptr<uchar>(0);//获取src首地址
	uchar* dst_p = dst.ptr<uchar>(0);//获取dst首地址

	for (int j = 0; j < src.rows; j++)
	{
		L.clear();//清空L列队
		src_p++;//从i=1开始
		for (int i = 1; i < src.cols; i++)
		{
			if (i >= k_size)
			{
				//minval[i - w] = a[L.size() > 0 ? L.front() : i - 1];
				*dst_p = *(src_p + (L.size() > 0 ? L.front() - i : -1));
				*max = *dst_p > *max ? *dst_p : *max;//记录暗通道最大值
				dst_p++;
			}
			if (*src_p > * (src_p - 1))
			{
				L.push_back(i - 1);
				if (i == k_size + L.front())
					L.pop_front();
			}
			else
			{
				while (L.size() > 0)
				{
					if (*src_p >= *(src_p + L.back() - i))
					{
						if (i == k_size + L.front())
							L.pop_front();
						break;
					}
					L.pop_back();
				}
			}
			src_p++;

		}
		//minval[a.cols - w] = a[L.size() > 0 ? L.front() : a.cols -1];
		*dst_p = *(src_p + (L.size() > 0 ? L.front() - src.cols : -1));
		*max = *dst_p > *max ? *dst_p : *max;//记录暗通道最大值
		*dst_p++;
	}

}

Mat defog1(Mat src)
{

	float kenlRatio = .01;
	int minAtomsLight = 240;
	float omga = 0.95;
	int k_size = 11;


	Mat dark(src.rows, src.cols, CV_8UC1);


	

	////////////////////////////////////////////////////////////////////////////////////////
	//提取暗通道
	uchar* src_p = src.ptr<uchar>(0);//获取src首地址
	uchar* dark_p = dark.ptr<uchar>(0);//获取dark首地址
	uchar src_B, src_G, src_R;
	for (int i = 0; i < src.rows; i++)
	{
		for (int j = 0; j < src.cols; j++)
		{
			src_B = *src_p;
			src_G = *(src_p + 1);
			src_R = *(src_p + 2);

			*dark_p = src_G < src_B ? src_G : src_B;
			*dark_p = *dark_p < src_R ? *dark_p : src_R;

			//max_dc = *dark_p > max_dc ? *dark_p : max_dc;//记录暗通道最大值

			src_p += 3;
			dark_p++;
		}

	}
	////////////////////////////////////////////////////////////////////////////////////////
	
	
	
	////////////////////////////////////////////////////////////////////////////////////////
	//最小值滤波
	int max_dc = 0;
	//复制扩边处理
	Mat dark_2;
	copyMakeBorder(dark, dark_2, (k_size - 1) / 2, (k_size - 1) / 2, (k_size - 1) / 2, (k_size - 1) / 2, BORDER_REPLICATE);

	//行过滤
	Mat dark_2_c(dark_2.rows, dark_2.cols - (k_size - 1), CV_8UC1);

	
	min_filter1(dark_2, dark_2_c, k_size,&max_dc);


	max_dc = 0;
	//列过滤
	Mat dark_2_r;
	dark_2_r = dark_2_c.t();//行过滤后的图像转置再进行列过滤
	Mat dark_filter_t(dark_2.cols - (k_size - 1), dark_2.rows - (k_size - 1), CV_8UC1);
	min_filter1(dark_2_r, dark_filter_t, k_size,&max_dc);

	Mat dark_filter(dark_2.rows - (k_size - 1), dark_2.cols - (k_size - 1), CV_8UC1);
	dark_filter = dark_filter_t.t();//滤波后的图像
	////////////////////////////////////////////////////////////////////////////////////////



	Mat t(src.rows, src.cols, CV_8UC1);


	t = 255 - omga *dark_filter;


	int A = minAtomsLight < max_dc ? minAtomsLight : max_dc;
	

	//double t1 = (double)getTickCount(); //测时间
	/////////////////////////////////////////////////////////////////////////////////////////
	double eps = 0.00001;

	Mat src_gray, t_filter;
	cvtColor(src, src_gray, COLOR_RGB2GRAY);
	src_gray.convertTo(src_gray, CV_32FC1, 1 / 255.0);
	t.convertTo(t_filter, CV_32FC1, 1 / 255.0);
	
	t_filter = GuidedFilter1(src_gray, t_filter, -1, 4*k_size, eps);

	t_filter.convertTo(t_filter, CV_8UC1, 255);

	/////////////////////////////////////////////////////////////////////////////////////////

	//t1 = (double)getTickCount() - t1;
	//double time = (t1 *1000.) / ((double)getTickFrequency());
	//cout << "filter time=" << time << " ms.   ";

	Mat J(src.rows, src.cols, CV_32FC3);
	src_p = src.ptr<uchar>(0);//获取src首地址
	uchar* t_p = t_filter.ptr<uchar>(0);//获取t首地址
	float* J_p = J.ptr<float>(0);//获取J首地址
	

	for (int i = 0; i < src.rows; i++)
	{
		for (int j = 0; j < src.cols; j++)
		{
			*t_p = *t_p > 1 ? *t_p : 1;
			*J_p	   = 255 * (*src_p - A) / ((float)*t_p) + A;
			*(J_p + 1) = 255 * (*(src_p + 1) - A) / ((float)*t_p) + A;
			*(J_p + 2) = 255 * (*(src_p + 2) - A) / ((float)*t_p) + A;
			src_p+=3;
			J_p+=3;
			t_p++;
		}

	}

	J.convertTo(J, CV_8UC3);

	imshow("dark_filter", dark_filter);
	imshow("t", t);
	imshow("t_filter", t_filter);


	return J;
}