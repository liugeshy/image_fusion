#include "allfunction.h"

// Fast guided filter function placeholder (you can implement it or use an existing library)
// Here, we use the standard guided filter provided by OpenCV as an example.


cv::Mat Ehn_GF(const cv::Mat& img) {
    cv::Mat img1;
    double mi, ma;
    cv::minMaxLoc(img, &mi, &ma);

    // Normalize the image to [0, 255]
    img.convertTo(img1, CV_64F, 255.0 / (ma - mi), -255.0 * mi / (ma - mi));

    // Apply log transform
    cv::Mat log_img;
    cv::log(img1 + 1, log_img);

    // Define filter parameters
    int m = img1.rows;
    int n = img1.cols;
    int r = static_cast<int>(0.04 * std::max(m, n));
    double eps = 0.1;

    // Perform guided filtering
    cv::Mat base = 255.0 * GuidedFilter4(img1 / 255.0, img1 / 255.0, -1, r, eps * eps, max(1,r/4));

    // Apply log transform to the base layer
    cv::Mat log_base;
    cv::log(base + 1, log_base);

    // Detail layer
    cv::Mat log_detail = log_img - log_base;

    // Contrast adjustment
    double target_contrast = std::log(4);
    double min_log_base, max_log_base;
    cv::minMaxLoc(log_base, &min_log_base, &max_log_base);

    double compressfactor = target_contrast / (max_log_base - min_log_base);
    double log_absolute_scale = (1 - compressfactor) * max_log_base;

    // Output image in log domain
    cv::Mat log_output = compressfactor * log_base + log_detail + log_absolute_scale;

    // Convert back to linear domain
    cv::Mat R;
    cv::exp(log_output, R);
    R = cv::min(R, 255.0);

    // Convert to 8-bit image
    R.convertTo(R, CV_8U);

    return R;
}
/*
int main() {
    // Load an image
    cv::Mat img = cv::imread("C:/Users/bit/Desktop/texture_2.jpg", cv::IMREAD_GRAYSCALE);
    if (img.empty()) {
        std::cerr << "Could not open the image file." << std::endl;
        return -1;
    }

    // Apply the enhancement function
    cv::Mat enhanced_img = Ehn_GF(img);

    // Display the results
    cv::imshow("Original Image", img);
    cv::imshow("Enhanced Image", enhanced_img);

    // Save the result
    cv::imwrite("C:/Users/bit/Desktop/enhanced_image.jpg", enhanced_img);

    cv::waitKey(0);
    return 0;
}
*/