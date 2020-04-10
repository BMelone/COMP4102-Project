#include <opencv2/opencv.hpp>
#include <iostream>
#include "segment-image.h"

int threshold_val, min_size_val, sigma_val;
const int threshold_max = 50;
const int min_size_max = 500;
const int sigma_max = 15;
cv::Mat img;
int num_ccs;
float sigma, threshold;
int min_size;

std::vector<int> xVector, yVector;


void segment_and_display() {
	sigma = sigma_val / 10;
	threshold = threshold_val * 100;
	min_size = min_size_val * 100;
	cv::Mat output = segment_image(img, sigma, threshold, min_size, &num_ccs);
	printf("got %d components with k=%f and min_size=%d\n", num_ccs, threshold, min_size);
	cv::imshow("image", output);
	cv::imwrite("output.png", output);
}

void sigma_and_display(int slider_val, void* data) {
	sigma_val = slider_val;
	segment_and_display();
}

void threshold_and_display(int slider_val, void* data) {
	threshold_val = slider_val;
	segment_and_display();
}

void min_size_and_display(int slider_val, void* data) {
	min_size_val = slider_val;
	segment_and_display();
}

void select_location(int event, int x, int y, int flags, void* param) {
	if (event == cv::EVENT_LBUTTONDOWN)
	{
		std::cout << "x= " << x << " y= " << y << std::endl;
		xVector.push_back(x);
		yVector.push_back(y);
	}
}

int main()
{
	img = cv::imread("spiderman.png");

	threshold_val = 3;
	min_size_val = 200;
	sigma_val = 5;
	cv::namedWindow("image", cv::WINDOW_NORMAL);

	cv::createTrackbar("Sigma", "image", &sigma_val, sigma_max, sigma_and_display);
	cv::createTrackbar("Threshold", "image", &threshold_val, threshold_max, threshold_and_display);
	cv::createTrackbar("Min Size", "image", &min_size_val, min_size_max, min_size_and_display);
	cv::setMouseCallback("image", select_location, 0);

	segment_and_display();

	cv::waitKey(0);
	return 0;
}