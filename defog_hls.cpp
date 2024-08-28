#include "defog_hls.h"


Mat defog_hls(Mat src)
{

	float kenlRatio = .01;
	int minAtomsLight = 240;
	float omga = 0.95;
	float grammar = 0.5;

	//暗通道滤波半径为图像最长边的1%
	int k_radius;
	int k_w = src.cols*kenlRatio;
	int k_h = src.rows*kenlRatio;
	k_radius = k_w > k_h ? k_w : k_h;
	int k_size = 2 * k_radius + 1;

	Mat dark(src.rows, src.cols, CV_8UC1);


	int max_dc = 0;

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

			max_dc = *dark_p > max_dc ? *dark_p : max_dc;//记录暗通道最大值

			src_p += 3;
			dark_p++;
		}

	}
	////////////////////////////////////////////////////////////////////////////////////////



	Mat t(src.rows, src.cols, CV_8UC1);

	t = 255 - omga * dark;
	 
	int A = minAtomsLight < max_dc ? minAtomsLight : max_dc;


	//cout <<"A:"<< A << endl;

	Mat J(src.rows, src.cols, CV_8UC3);
	src_p = src.ptr<uchar>(0);//获取src首地址
	uchar* t_p = t.ptr<uchar>(0);//获取t首地址
	uchar* J_p = J.ptr<uchar>(0);//获取J首地址

	for (int i = 0; i < src.rows; i++)
	{
		for (int j = 0; j < src.cols; j++)
		{
			*t_p = *t_p > 1 ? *t_p : 1;
			*J_p = saturate_cast <uchar> (255 * (*src_p - A) / *t_p + A);
			*(J_p + 1) = saturate_cast <uchar> (255 * (*(src_p + 1) - A) / *t_p + A);
			*(J_p + 2) = saturate_cast <uchar> (255 * (*(src_p + 2) - A) / *t_p + A);
			src_p += 3;
			J_p += 3;
			t_p++;
		}

	}

	Mat J_gramma(src.rows, src.cols, CV_8UC3);
	gramma(J, J_gramma, grammar);

	
	//imshow("dark", dark);
	//imshow("t", t);


	return J_gramma;
}