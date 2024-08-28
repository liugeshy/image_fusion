#include "defog2_mybox.h"



void box_filter(Mat& img, Mat& result, int k_size)
{
	//init part
	result.create(img.rows-k_size+1, img.cols-k_size + 1,CV_32FC1);

	int width = img.cols, height = img.rows;
	int boxwidth = width - k_size + 1, boxheight = height - k_size + 1;
	float *sum = (float*)malloc(boxwidth *boxheight * sizeof(float));
	float *buff = (float*)malloc(width * sizeof(float));
	memset(sum, 0, boxwidth *boxheight * sizeof(float));
	memset(buff, 0, width * sizeof(float));


	////////////////////////////�����ۼӾ���sum//////////////////////////////////////////
	float Xsum = 0;
	//��[0��k_size]���ۼ�д��buff
	float* img_p = img.ptr<float>(0);//��ȡimg�׵�ַ
	for (int y = 0; y < k_size; y++)
	{
		for (int x = 0; x < width; x++)
		{
			buff[x] += *img_p;
			img_p++;
		}
	}
	img_p = img.ptr<float>(0);//��ȡimg�׵�ַ
	for (int y = 0; y < boxheight - 1; y++)
	{

		Xsum = 0;
		for (int j = 0; j < k_size; j++)
		{
			Xsum += buff[j];//��buff[0��k_size]�ۼ�
		}

		sum[y*boxwidth + 0] = Xsum;
		for (int x = 1; x < boxwidth; x++)
		{
			Xsum = Xsum - buff[x - 1] + buff[k_size - 1 + x];// ����������غ�
			sum[y*boxwidth + x] = Xsum;
		}

		for (int x = 0; x < width; x++)
		{
			buff[x] = buff[x] - *img_p + *(img_p + k_size * width);//�����������ƶ�
			img_p++;
		}
	}
	Xsum = 0;
	for (int j = 0; j < k_size; j++)
	{
		Xsum += buff[j];//��buff[0��k_size]�ۼ�
	}

	sum[(height - k_size)*boxwidth + 0] = Xsum;
	for (int x = 1; x < boxwidth; x++)
	{
		Xsum = Xsum - buff[x - 1] + buff[k_size - 1 + x];// ����������غ�
		sum[(height - k_size)*boxwidth + x] = Xsum;
	}
	//////////////////////////////////////////////////////////////////////



	/////////////////�������õ�ÿ����ĺͣ���������result/////////////////////
	int num = k_size * k_size;
	float* result_p = result.ptr<float>(0);//��ȡresult�׵�ַ
	for (int y = 0; y < boxheight; y++)
	{
		for (int x = 0; x < boxwidth; x++)
		{
			*result_p = sum[y *boxwidth + x] / num;
			result_p++;
		}
	}
}


Mat GuidedFilter2(Mat I, Mat P, int type, int radius, double eps)
{

	int k_size = 2 * radius + 1;
	//��I*I, I*P
	Mat  I2, IP;
	multiply(I, I, I2);
	multiply(I, P, IP);

	//���ֵ
	Mat meanI, meanP, meanI2, meanIP;
	Mat I_k, P_k,  I2_k,  IP_k;
	copyMakeBorder(I, I_k, radius, radius, radius, radius, BORDER_REFLECT_101);
	copyMakeBorder(P, P_k, radius, radius, radius, radius, BORDER_REFLECT_101);
	copyMakeBorder(I2, I2_k, radius, radius, radius, radius, BORDER_REFLECT_101);
	copyMakeBorder(IP, IP_k, radius, radius, radius, radius, BORDER_REFLECT_101);
	box_filter(I_k, meanI,  k_size);
	box_filter(P_k, meanP,  k_size);
	box_filter(I2_k, meanI2, k_size);
	box_filter(IP_k, meanIP,  k_size);
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
	Mat a_k, b_k;
	copyMakeBorder(a, a_k, radius, radius, radius, radius, BORDER_REFLECT_101);
	copyMakeBorder(b, b_k, radius, radius, radius, radius, BORDER_REFLECT_101);
	box_filter(a_k, meanA,  k_size);
	box_filter(b_k, meanB,  k_size);
	//boxFilter(b, meanB, type, winSize);
	//���
	Mat output;
	output = meanA.mul(I) + meanB;
	return output;
}


void  min_filter2(Mat &src, Mat &dst, int k_size,int* max)
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

Mat defog2(Mat src)
{

	float kenlRatio = .01;
	int minAtomsLight = 240;
	float omga = 0.95;
	int k_size = 11;


	Mat dark(src.rows, src.cols, CV_8UC1);


	////////////////////////////////////////////////////////////////////////////////////////
	//��ȡ��ͨ��
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
	
	
	
	////////////////////////////////////////////////////////////////////////////////////////
	//��Сֵ�˲�
	int max_dc = 0;
	//�������ߴ���
	Mat dark_2;
	copyMakeBorder(dark, dark_2, (k_size - 1) / 2, (k_size - 1) / 2, (k_size - 1) / 2, (k_size - 1) / 2, BORDER_REPLICATE);

	//�й���
	Mat dark_2_c(dark_2.rows, dark_2.cols - (k_size - 1), CV_8UC1);

	
	min_filter2(dark_2, dark_2_c, k_size,&max_dc);


	max_dc = 0;
	//�й���
	Mat dark_2_r;
	dark_2_r = dark_2_c.t();//�й��˺��ͼ��ת���ٽ����й���
	Mat dark_filter_t(dark_2.cols - (k_size - 1), dark_2.rows - (k_size - 1), CV_8UC1);
	min_filter2(dark_2_r, dark_filter_t, k_size,&max_dc);

	Mat dark_filter(dark_2.rows - (k_size - 1), dark_2.cols - (k_size - 1), CV_8UC1);
	dark_filter = dark_filter_t.t();//�˲����ͼ��
	////////////////////////////////////////////////////////////////////////////////////////



	Mat t(src.rows, src.cols, CV_8UC1);


	t = 255 - omga *dark_filter;


	int A = minAtomsLight < max_dc ? minAtomsLight : max_dc;
	

	//double t1 = (double)getTickCount(); //��ʱ��
	/////////////////////////////////////////////////////////////////////////////////////////
	double eps = 0.00001;

	Mat src_gray, t_filter;
	cvtColor(src, src_gray, COLOR_RGB2GRAY);
	src_gray.convertTo(src_gray, CV_32FC1, 1 / 255.0);
	t.convertTo(t_filter, CV_32FC1, 1 / 255.0);
	
	t_filter = GuidedFilter2(src_gray, t_filter, -1, 4*k_size, eps);

	t_filter.convertTo(t_filter, CV_8UC1, 255);

	/////////////////////////////////////////////////////////////////////////////////////////

	//t1 = (double)getTickCount() - t1;
	//double time = (t1 *1000.) / ((double)getTickFrequency());
	//cout << "filter time=" << time << " ms.   ";

	Mat J(src.rows, src.cols, CV_32FC3);

	src_p = src.ptr<uchar>(0);//��ȡsrc�׵�ַ
	uchar* t_p = t_filter.ptr<uchar>(0);//��ȡt�׵�ַ
	float* J_p = J.ptr<float>(0);//��ȡJ�׵�ַ
	

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



	//imshow("src", src);
	//imshow("dark_filter", dark_filter);
	//imshow("t", t);
	//imshow("t_filter", t_filter);
	//imshow("J", J);

	return J;
}