//#include "pch.h"
#include "defog0.h"


void  min_filter0(Mat &src, Mat &dst, int k_size)
{
	deque <int>  L;

	for (int j = 0; j < src.rows; j++)
	{
		L.clear();//清空L列队
		for (int i = 1; i < src.cols; i++)
		{
			if (i >= k_size)
			{
				//minval[i - w] = a[L.size() > 0 ? L.front() : i - 1];
				dst.at<uchar>(j, i - k_size) = src.at<uchar>(j, L.size() > 0 ? L.front() : i - 1);
	
			}
			if (src.at<uchar>(j, i) > src.at<uchar>(j, i - 1))
			{
				L.push_back(i - 1);
				if (i == k_size + L.front())
					L.pop_front();
			}
			else
			{
				while (L.size() > 0)
				{
					if (src.at<uchar>(j, i) >= src.at<uchar>(j, L.back()))
					{
						if (i == k_size + L.front())
							L.pop_front();
						break;
					}
					L.pop_back();
				}
			}

		
		}
		//minval[a.cols - w] = a[L.size() > 0 ? L.front() : a.cols -1];
		dst.at<uchar>(j, src.cols - k_size) = src.at<uchar>(j, L.size() > 0 ? L.front() : src.cols - 1);

	}

}

Mat defog0(Mat src)
{

	float kenlRatio = .01;
	int minAtomsLight = 240;
	float omga = 0.95;
	int k_size = 25;


	Mat dark(src.rows, src.cols, CV_8UC1);


	int max_dc = 0;

	////////////////////////////////////////////////////////////////////////////////////////
	//提取暗通道
	uchar src_B, src_G, src_R,dark_value;
	for (int i = 0; i < src.rows; i++)
	{
		for (int j = 0; j < src.cols; j++)
		{
			src_B = src.at<Vec3b>(i, j)[0];
			src_G = src.at<Vec3b>(i, j)[1];
			src_R = src.at<Vec3b>(i, j)[2];
		

			dark_value = src_G < src_B ? src_G : src_B;
			dark_value = dark_value < src_R ? dark_value : src_R;
			dark.at<uchar>(i, j)= dark_value;

			max_dc = dark_value > max_dc ? dark_value : max_dc;//记录暗通道最大值

	
		}

	}
	////////////////////////////////////////////////////////////////////////////////////////
	
	//////////////////////////////////////////////////////////////////////////////////////////
	////最小值滤波

	////复制扩边处理
	//Mat dark_2;
	//copyMakeBorder(dark, dark_2, (k_size - 1) / 2, (k_size - 1) / 2, (k_size - 1) / 2, (k_size - 1) / 2, BORDER_REPLICATE);

	////行过滤
	//Mat dark_2_c(dark_2.rows, dark_2.cols - (k_size - 1), CV_8UC1);

	////double t = (double)getTickCount(); //测时间
	//min_filter0(dark_2, dark_2_c, k_size);

	///*t = (double)getTickCount() - t;
	//double time = (t *1000.) / ((double)getTickFrequency());
	//cout << "filter time=" << time << " ms. " << endl;*/

	////列过滤
	//Mat dark_2_r;
	//dark_2_r = dark_2_c.t();//行过滤后的图像转置再进行列过滤
	//Mat dark_filter_t(dark_2.cols - (k_size - 1), dark_2.rows - (k_size - 1), CV_8UC1);
	//min_filter0(dark_2_r, dark_filter_t, k_size);

	//Mat dark_filter(dark_2.rows - (k_size - 1), dark_2.cols - (k_size - 1), CV_8UC1);
	//dark_filter = dark_filter_t.t();//滤波后的图像
	//////////////////////////////////////////////////////////////////////////////////////////





	Mat t_d(src.rows, src.cols, CV_32FC1, 1 / 255.0);
	Mat dc_f;
	//dark_filter.convertTo(dc_f, CV_32FC1);
	dark.convertTo(dc_f, CV_32FC1);
	t_d = 1 - omga * dc_f / 255;


	int A = minAtomsLight < max_dc ? minAtomsLight : max_dc;

	//cout <<"A:"<< A << endl;

	Mat J(src.rows, src.cols, CV_32FC3);


	for (int i = 0; i < src.rows; i++)
	{
		for (int j = 0; j < src.cols; j++)
		{
			for (int k = 0; k < 3; k++)
			{
				float value = ((float)src.at<Vec3b>(i, j)[k] - A) / t_d.at<float>(i, j) + A;

				J.at<Vec3f>(i, j)[k] = value;
			}
		}

	}

	J.convertTo(J, CV_8UC3);




	imshow("dark", dark);

	imshow("t", t_d);

	return J;
}