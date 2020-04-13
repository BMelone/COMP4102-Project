#include <opencv2/opencv.hpp>
#include <iostream>
#include <unordered_set>
#include <vector>

#include "segment-image.h"
#include "disjoint-set.h"
#include "pixelate.h"
#include "cartoon/similarity_graph.h"
#include "cartoon/border_graph.h"
#include "cartoon/point_c.h"
#include "cartoon/color_flood.h"

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
	cv::imshow("sliders", output);
	cv::imwrite("o_segments.png", output);
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

int main()
{
	img = cv::imread("smokin.png");
	if (img.data == NULL) {
		std::cout << "The input file did not exist\n";
		cv::waitKey(0);
		return 0;
	}

	threshold_val = 6;
	min_size_val = 5;
	sigma_val = 5;
	cv::namedWindow("sliders", cv::WINDOW_NORMAL);
	cv::resizeWindow("sliders", 500, 500);

	cv::createTrackbar("Sigma", "sliders", &sigma_val, sigma_max, sigma_and_display);
	cv::createTrackbar("Threshold","sliders", &threshold_val, threshold_max, threshold_and_display);
	cv::createTrackbar("Min Size", "sliders", &min_size_val, min_size_max, min_size_and_display);
	cv::setMouseCallback("sliders", select_location, 0);

	segment_and_display();

	cv::waitKey(0);
	cv::destroyWindow("sliders");
	cv::namedWindow("image", cv::WINDOW_NORMAL);
	cv::resizeWindow("image", 500, 500);

	cv::Vec4b transparent(0,0,0,0);
	int width = img.cols;
	int height = img.rows;
	cv::Mat segmented_output = cv::Mat::zeros(cv::Size(width, height), CV_8UC4);
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int comp = u->find(y * width + x);
			cv::Vec3b img_pixel = img.at<cv::Vec3b>(y, x);
			segmented_output.at<cv::Vec4b>(y, x) = savedSegments.find(comp) != savedSegments.end() ? 
				cv::Vec4b(img_pixel[0], img_pixel[1], img_pixel[2], 255) : transparent;
		}
	}

	cv::imshow("image", segmented_output);
	cv::imwrite("o_foreground.png", segmented_output);
	cv::waitKey(0);

	cv::Mat pixelate_output = pixelate(segmented_output, 4, KMEANS);
	cv::imshow("image", pixelate_output);
	cv::imwrite("o_pixelted.png", pixelate_output);
	cv::waitKey(0);

	SimilarityGraph similarity_graph(pixelate_output, SimilarityGraph::SameColor);
	BorderGraph border_graph = similarity_graph.ExtractDualGraph();
	border_graph.SplitJunctions();
	cv::Mat cartoon_output;
	SetBorders(cartoon_output, pixelate_output.size(), border_graph.GetChains());
	std::unordered_map<PointC, cv::Vec4b> colors;
	ApproximateColorMap(pixelate_output, colors);
	FloodColors(cartoon_output, colors);
	cv::imshow("image", cartoon_output);
	cv::imwrite("o_cartoon.png", pixelate_output);
	cv::waitKey(0);

	return 0;
}