#include "defog3_fastguide.h"


Mat GuidedFilter3(Mat I, Mat P, int type, int radius, double eps,int s)
{
	Mat I_sub, P_sub;

	resize(I, I_sub, Size(), 1.0 / s, 1.0 / s, INTER_NEAREST);
	resize(P, P_sub, Size(), 1.0 / s, 1.0 / s, INTER_NEAREST);
	int r_sub = radius / s;
	Size winSize(2 * r_sub + 1, 2 * r_sub + 1);
	//��I*I, I*P
	Mat  I2, IP;
	multiply(I_sub, I_sub, I2);
	multiply(I_sub, P_sub, IP);

	//���ֵ
	Mat meanI, meanP, meanI2, meanIP;
	boxFilter(I_sub, meanI, type, winSize);
	boxFilter(P_sub, meanP, type, winSize);
	boxFilter(I2, meanI2, type, winSize);
	boxFilter(IP, meanIP, type, winSize);
	//�󷽲�/Э����
	Mat varI, covIP;
	varI = meanI2 - meanI.mul(meanI);
	covIP = meanIP - meanI.mul(meanP);
	//��ϵ��a, b
	Mat a, b;
	varI += eps;
	divide(covIP, varI, a);
	b = meanP - a.mul(meanI);
	//a��b��������ƽ��
	Mat meanA, meanB;
	boxFilter(a, meanA, type, winSize);
	boxFilter(b, meanB, type, winSize);

	resize(meanA, meanA, I.size(), 0, 0, INTER_LINEAR);
	resize(meanB, meanB, I.size(), 0, 0, INTER_LINEAR);
	//���
	Mat output;
	output = meanA.mul(I) + meanB;
	return output;
}


void  min_filter3(Mat &src, Mat &dst, int k_size,int* max)
{
	deque <int>  L;

	uchar* src_p = src.ptr<uchar>(0);//��ȡsrc�׵�ַ
	uchar* dst_p = dst.ptr<uchar>(0);//��ȡdst�׵�ַ

	for (int j = 0; j < src.rows; j++)
	{
		L.clear();//���L�ж�
		src_p++;//��i=1��ʼ
		for (int i = 1; i < src.cols; i++)
		{
			if (i >= k_size)
			{
				//minval[i - w] = a[L.size() > 0 ? L.front() : i - 1];
				*dst_p = *(src_p + (L.size() > 0 ? L.front() - i : -1));
				*max = *dst_p > *max ? *dst_p : *max;//��¼��ͨ�����ֵ
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
		*max = *dst_p > *max ? *dst_p : *max;//��¼��ͨ�����ֵ
		*dst_p++;
	}

}

Mat defog3(Mat src)
{

	float kenlRatio = .01;
	int minAtomsLight = 240;
	float omga =0.95;
	int k_size = 15;


	Mat dark(src.rows, src.cols, CV_8UC1);


	/////////////////////////////////��ȡ��ͨ��///////////////////////////////////////////////////////
	uchar* src_p = src.ptr<uchar>(0);//��ȡsrc�׵�ַ
	uchar* dark_p = dark.ptr<uchar>(0);//��ȡdark�׵�ַ
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

			//max_dc = *dark_p > max_dc ? *dark_p : max_dc;//��¼��ͨ�����ֵ

			src_p += 3;
			dark_p++;
		}

	}
	////////////////////////////////////////////////////////////////////////////////////////
	
	
	
	///////////////////////////////��Сֵ�˲�/////////////////////////////////////////////////////////

	int max_dc = 0;
	//�������ߴ���
	Mat dark_2;
	copyMakeBorder(dark, dark_2, (k_size - 1) / 2, (k_size - 1) / 2, (k_size - 1) / 2, (k_size - 1) / 2, BORDER_REPLICATE);

	//�й���
	Mat dark_2_c(dark_2.rows, dark_2.cols - (k_size - 1), CV_8UC1);

	min_filter3(dark_2, dark_2_c, k_size,&max_dc);
	max_dc = 0;
	//�й���
	Mat dark_2_r;
	Mat dark_filter_t(dark_2.cols - (k_size - 1), dark_2.rows - (k_size - 1), CV_8UC1);
	dark_2_r = dark_2_c.t();//�й��˺��ͼ��ת���ٽ����й���
	min_filter3(dark_2_r, dark_filter_t, k_size,&max_dc);

	Mat dark_filter(dark_2.rows - (k_size - 1), dark_2.cols - (k_size - 1), CV_8UC1);
	dark_filter = dark_filter_t.t();//�˲����ͼ��
	////////////////////////////////////////////////////////////////////////////////////////


	Mat t(src.rows, src.cols, CV_8UC1);

	t = 255 - omga *dark_filter;

	int A = minAtomsLight < max_dc ? minAtomsLight : max_dc;


	/////////////////////////////////�����˲�//////////////////////////////////////////////////////////
	double eps = 0.00001;
	int r = 4 * k_size;
	int s = r;
	Mat src_gray, t_filter;
	cvtColor(src, src_gray, COLOR_RGB2GRAY);//ԭʼͼ�ĻҶ�ͼ��Ϊ����ͼ
	src_gray.convertTo(src_gray, CV_32FC1, 1 / 255.0);
	t.convertTo(t_filter, CV_32FC1, 1 / 255.0);
	t_filter = GuidedFilter3(src_gray, t_filter, -1, r, eps, s);
	t_filter.convertTo(t_filter, CV_8UC1, 255);
	////////////////////////////////////////////////////////////////////////////////////////////////



	Mat J(src.rows, src.cols, CV_8UC3);
	src_p = src.ptr<uchar>(0);//��ȡsrc�׵�ַ
	uchar* t_p = t_filter.ptr<uchar>(0);//��ȡt�׵�ַ
	uchar* J_p = J.ptr<uchar>(0);//��ȡJ�׵�ַ

	for (int i = 0; i < src.rows; i++)
	{
		for (int j = 0; j < src.cols; j++)
		{
			*t_p = *t_p > 1 ? *t_p : 1;
			*J_p	   = saturate_cast <uchar> (255 * (*src_p - A) / *t_p + A);
			*(J_p + 1) = saturate_cast <uchar> (255 * (*(src_p + 1) - A) / *t_p + A);
			*(J_p + 2) = saturate_cast <uchar> (255 * (*(src_p + 2) - A) / *t_p + A);
			src_p += 3;
			J_p += 3;
			t_p++;
		}

	}

/////////////////////////////�⺯����////////////////////////////////////////////////////////////
//	Mat src_f,t_f;
//	src.convertTo(src_f, CV_32FC3);
//	t.convertTo(t_f, CV_32FC1);
//	Mat src_RGB[3], J_RGB[3];
//	split(src_f, src_RGB);
//
//	J_RGB[0] = 255 * (src_RGB[0] - A) / t_f + A;
//	J_RGB[1] = 255 * (src_RGB[1] - A) / t_f + A;
//	J_RGB[2] = 255 * (src_RGB[2] - A) / t_f + A;
//
//	merge(J_RGB, 3, J);
//
//	J.convertTo(J, CV_8UC3);
//////////////////////////////////////////////////////////////////////////////////////////////
	

	/*imshow("dark_filter", dark_filter);
	imshow("t", t);
	imshow("t_filter", t_filter);*/
	

	return J;
}