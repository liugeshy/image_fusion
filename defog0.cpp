//#include "pch.h"
#include "defog0.h"


void  min_filter0(Mat &src, Mat &dst, int k_size)
{
	deque <int>  L;

	for (int j = 0; j < src.rows; j++)
	{
		L.clear();//���L�ж�
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
	//��ȡ��ͨ��
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

			max_dc = dark_value > max_dc ? dark_value : max_dc;//��¼��ͨ�����ֵ

	
		}

	}
	////////////////////////////////////////////////////////////////////////////////////////
	
	//////////////////////////////////////////////////////////////////////////////////////////
	////��Сֵ�˲�

	////�������ߴ���
	//Mat dark_2;
	//copyMakeBorder(dark, dark_2, (k_size - 1) / 2, (k_size - 1) / 2, (k_size - 1) / 2, (k_size - 1) / 2, BORDER_REPLICATE);

	////�й���
	//Mat dark_2_c(dark_2.rows, dark_2.cols - (k_size - 1), CV_8UC1);

	////double t = (double)getTickCount(); //��ʱ��
	//min_filter0(dark_2, dark_2_c, k_size);

	///*t = (double)getTickCount() - t;
	//double time = (t *1000.) / ((double)getTickFrequency());
	//cout << "filter time=" << time << " ms. " << endl;*/

	////�й���
	//Mat dark_2_r;
	//dark_2_r = dark_2_c.t();//�й��˺��ͼ��ת���ٽ����й���
	//Mat dark_filter_t(dark_2.cols - (k_size - 1), dark_2.rows - (k_size - 1), CV_8UC1);
	//min_filter0(dark_2_r, dark_filter_t, k_size);

	//Mat dark_filter(dark_2.rows - (k_size - 1), dark_2.cols - (k_size - 1), CV_8UC1);
	//dark_filter = dark_filter_t.t();//�˲����ͼ��
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