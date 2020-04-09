#include <opencv2/opencv.hpp>
#include <iostream>
#include "segment-image.h"

int main()
{
	cv::Mat img = cv::imread("lenna.png");

	float sigma = atof("0.5");
	float k = atof("500");
	int min_size = atoi("20");
	int num_ccs;

	printf("processing\n");

	cv::Mat output = segment_image(img, sigma, k, min_size, &num_ccs);

	printf("got %d components\n", num_ccs);

	cv::namedWindow("image", cv::WINDOW_NORMAL);
	cv::imshow("image", output);
	cv::imwrite("output.png", output);
	cv::waitKey(0);
	return 0;
}