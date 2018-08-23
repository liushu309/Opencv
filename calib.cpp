#include <opencv2/opencv.hpp>

int main(){
    cv::Mat mat_test = cv::imread("1.jpg", 1);
    cv::imshow("test", mat_test);
    cv::waitKey(0);
    return 0;
}
