#include <opencv2/opencv.hpp>
#include <iostream>
#include <unordered_set>

#include "segment-image.h"
#include "disjoint-set.h"
#include "pixelate.h"

int threshold_val, min_size_val, sigma_val;
const int threshold_max = 20;
const int min_size_max = 200;
const int sigma_max = 15;
cv::Mat img;
int num_ccs;
float sigma, threshold;
int min_size;

universe* u;

std::vector<int> xVector, yVector;
std::unordered_set<int> savedSegments;


void segment_and_display() {
	sigma = sigma_val / 10;
	threshold = threshold_val * 50;
	min_size = min_size_val * 100;
	cv::Mat output = segment_image(img, sigma, threshold, min_size, &num_ccs, u);
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
		savedSegments.insert(u->find(y * img.cols + x)); //save the selected segment
	}
}
/*
int main()
{
	img = cv::imread("smokin.png");

	threshold_val = 6;
	min_size_val = 5;
	sigma_val = 5;
	cv::namedWindow("image", cv::WINDOW_NORMAL);
	cv::resizeWindow("image", 500, 500);

	cv::createTrackbar("Sigma", "image", &sigma_val, sigma_max, sigma_and_display);
	cv::createTrackbar("Threshold", "image", &threshold_val, threshold_max, threshold_and_display);
	cv::createTrackbar("Min Size", "image", &min_size_val, min_size_max, min_size_and_display);
	cv::setMouseCallback("image", select_location, 0);

	segment_and_display();

	cv::waitKey(0);

	cv::Vec3b black;
	black[0] = 0;
	black[1] = 0;
	black[2] = 0;

	int width = img.cols;
	int height = img.rows;
	cv::Mat output = cv::Mat::zeros(cv::Size(width, height), img.type());
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int comp = u->find(y * width + x);
			output.at<cv::Vec3b>(y, x) = savedSegments.find(comp) != savedSegments.end() ? 
				img.at<cv::Vec3b>(y, x) : black;
		}
	}

	cv::imshow("image", output);
	cv::imwrite("output2.png", output);
	cv::waitKey(0);

	cv::Mat pixelated = pixelate(output, 4,KMEANS);
	cv::imshow("image", pixelated);
	cv::imwrite("pixelted.png", pixelated);
	cv::waitKey(0);

	return 0;
}*/