#include "allfunction.h"

int main()
{
    Mat img1, img2;
    string name;
    double lambda;
    clock_t start, end,teststart,testend;
    start = clock();
    string path1 = "C:/Users/bit/Desktop/Context-Enhance-via-Fusion-master/Context_Enhance_via_Fusion/image/Camp_Vis.jpg";
    string path2 = "C:/Users/bit/Desktop/Context-Enhance-via-Fusion-master/Context_Enhance_via_Fusion/image/Camp_IR.jpg";
    // µ÷ÓÃº¯Êý
    
    //PickName(path1, path2, img1, img2, name, true);
    //img1 = imread(path1, IMREAD_GRAYSCALE);
    img1 = imread(path1);
    img2 = imread(path2, IMREAD_GRAYSCALE);

    //cout << "Image name: " << name << endl;
    //cvtColor(img1, img1, COLOR_BGR2GRAY);
    //img1.convertTo(img1, CV_64FC3, 1 / 255.0);
    teststart = clock();
    Mat img1E = Ehn_GF_new(img1); 
    //imshow("imgE", img1E);

    img1 = img1E;
    cvtColor(img1, img1, cv::COLOR_BGR2GRAY);
    testend = clock();
    cout << testend - teststart << endl;   
    //cout << "img1.type£º" << img1.type() << endl;
    double minVal, maxVal;
    minMaxLoc(img2, &minVal, &maxVal);
    Mat img2_normalized;
    img2.convertTo(img2_normalized, CV_64F);
    img2 = (img2_normalized - minVal) / (maxVal - minVal) * 255.0;
    img1.convertTo(img1, CV_64F);
    
    //img2_normalized.convertTo(img2_normalized, CV_8U);
    //imshow("normalized image", img2_normalized);
    cout << "img1 type" << img1.type() << endl;
    cout << "img2 type" << img2.type() << endl;
    double Rs = Relative_PS(img2, img1);
    //cout << img1 << endl;
    if (Rs < 0.8)
    {
        lambda = 100.0;
    }

    else if (Rs > 1.6)
    {
        lambda = 2000.0;
    }

    else
    {
        lambda = 2500.0 * Rs - 1900.0;
    }
    
    int nLevel = 4;
    double sigma = 2.0, k = 2.0;
    int r0 = 2;
    double eps0 = 0.1;
    double l = 2.0;

    vector<Mat> M1(nLevel + 1), M1L(nLevel + 1), M1D(nLevel + 1), M1E(nLevel + 1);
    vector<Mat> M2(nLevel + 1), M2L(nLevel + 1), M2D(nLevel + 1), M2E(nLevel + 1);

    M1[0] = img1 / 255.0;
    //cout << M1[0] << endl;
    M1L[0] = M1[0];
    M2[0] = img2 / 255.0;
    M2L[0] = M2[0];

    double sigma0 = sigma;
    int r = r0;
    double eps = eps0;

    for (int ii = 1; ii < nLevel + 1; ii++)
    {
        int s = max(1, r / 2);
        M1[ii] = GuidedFilter4(M1[ii - 1], M1[ii - 1], -1, r, 100.0*100.0,s);
        //cout << M1[ii] << endl;
        M1L[ii] = GuidedFilter4(M1L[ii - 1], M1L[ii - 1], -1, r, eps * eps,s);

        M1D[ii] = M1[ii - 1] - M1L[ii];
        M1E[ii] = M1L[ii] - M1[ii];

        sigma0 *= k;
        r *= k;
        eps /= l;
    }

    sigma0 = sigma;
    r = r0;
    eps = eps0;

    for (int ii = 1; ii < nLevel + 1; ii++) {
        int s = max(1, r / 2);
        M2[ii] = GuidedFilter4(M2[ii - 1], M2[ii - 1],-1, r, 100.0*100.0,s);
        M2L[ii] = GuidedFilter4(M2L[ii - 1], M2L[ii - 1],-1, r, eps * eps,s);

        M2D[ii] = M2[ii - 1] - M2L[ii];
        M2E[ii] = M2L[ii] - M2[ii];

        sigma0 *= k;
        r *= k;
        eps /= l;
    }

    //---------- Fusion --------------
    Mat MB = Mat::zeros(M2[nLevel].size(), CV_64F);
    for (int j = nLevel; j >= 2; --j) {
        Mat D2 = abs(M2E[j]);
        Mat D1 = abs(M1E[j]);
        Mat R = max(D2 - D1, 0.0);
        double Rmax;
        minMaxLoc(R, nullptr, &Rmax);
        Mat P = R / Rmax;
        P.convertTo(P, CV_64F);
        Mat Cj = atanMatrix(lambda * P) / atan(lambda);

        double sigma_b = 2 * sigma0;
        if (j == nLevel) {
            
            int w = floor(3 * sigma_b);
            int kernelSize = 2 * w + 1;
            Mat Cb = atanMatrix(lambda * P) / atan(lambda);
            
            Mat h;
            GaussianBlur(Cb, h, Size(kernelSize, kernelSize), sigma_b, sigma_b, BORDER_REFLECT);
            Cb = h; 
            //cout << " Cb:" << Cb.type();
            Mat M2n = M2[nLevel];
            Mat M1n = M1[nLevel];
            //M2n.convertTo(M2n, CV_64F);
            //M1n.convertTo(M1n, CV_64F);
            //cout << "M2n:type" << M2n.type()<<endl;
            //cout << "M1n.type:" << M1n.type() << endl;
            MB = Cb.mul(M2n) + (1 - Cb).mul(M1n);
            //cout << MB << endl;
        }

        double sigma_c = 1;
        int wc = floor(3 * sigma_c);
        int kernelSize2 = 2 * wc + 1;
        Mat b;
        GaussianBlur(Cj, b, Size(kernelSize2, kernelSize2), sigma_c, sigma_c, BORDER_REFLECT);
        Cj = b;
        //cout <<"Cj.typt:" << Cj.type() << endl;
        //cout << "M2E,type:" << M2E[j].type() << endl;
        //cout << "M1E,type:" << M1E[j].type() << endl;
        //M2E[j].convertTo(M2E[j], CV_64F);
        //M1E[j].convertTo(M1E[j], CV_64F);
        Mat md = Cj.mul(M2E[j]) + (1.0 - Cj).mul(M1E[j]);
        MB += md;
        //cout << "M2D,type:" << M2D[j].type() << endl;
        //cout << "M1D,type:" << M1D[j].type() << endl;
        //M2D[j].convertTo(M2D[j], CV_64F);
        //M1D[j].convertTo(M1D[j], CV_64F);
        md = Cj.mul(M2D[j]) + (1.0 - Cj).mul(M1D[j]);
        MB += md;
        //cout << MB << endl;
    }

    // Processing the smallest scale (j = 2)
    double sigma_t = 1;
    int wt = floor(3 * sigma_t);
    int kernelSize3 = 2 * wt + 1;
    Mat c;
    Mat C11;
    Mat absM1E2 = abs(M1E[2]);
    Mat absM2E2 = abs(M2E[2]);
    compare(absM1E2, absM2E2, C11, CMP_LT);
    //cout << "C11.type:" << C11.type() << endl;
    C11 = C11 / 255.0;
    C11.convertTo(C11, CV_64F);
    //cout << C11 << endl;
    GaussianBlur(C11, c, Size(kernelSize3, kernelSize3), sigma_t, sigma_t, BORDER_REFLECT);
    C11 = c;
    //cout << "C11.type:" << C11.type() << endl;
    Mat md = C11.mul(M2E[1]) + (1.0 - C11).mul(M1E[1]);
    //cout << "md.type:" << md.type() << endl;
    MB += md;
    Mat C10 = (abs(M1D[1]) < abs(M2D[1])) / 255.0;
    C10.convertTo(C10, CV_64F);
    //cout << C10 << endl;
    //cout << "C10.type:" << C10.type();
    md = C10.mul(M2D[1]) + (1.0 - C10).mul(M1D[1]);
    //cout << md << endl;
    //imshow("md", md);
    MB += md;
    //cout << MB << endl;
    //cout << "MB.type:" << MB.type()<<endl;
    
    Mat FI = MB * 275;
    FI.convertTo(FI, CV_32S);
    cv::min(FI, 255.0, FI);
    cv::max(FI, 0.0, FI);
    FI.convertTo(FI, CV_8U);
    //FI = min(FI, 255);
    //FI = max(FI, 0);
    end = clock();
    cout << "running time:" << double(end - start)/CLOCKS_PER_SEC<<"ms" <<endl;
    imshow("Result", FI);
    waitKey(0);
    return 0;
}