#include <iostream>
#include <opencv2/opencv.hpp>
#include <cmath>
#include <unistd.h>

#include "pixelate.h"

int main() {
    cv::Mat img = cv::imread("../Comp4102Project/lenna.png");
    cv::Mat pixelated = pixelate(img, 4);
    cv::Mat pixelated_rsize;
    cv::Size imSize(img.cols, img.rows);
    cv::resize(pixelated, pixelated_rsize, imSize, cv::INTER_LINEAR_EXACT);
    cv::imshow("original", img);
    cv::imshow("pixelated", pixelated_rsize);
    cv::waitKey();
}

