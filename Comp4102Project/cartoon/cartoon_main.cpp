#include <opencv2/opencv.hpp>
#include <iostream>
#include <ctime>
#include <cmath>
#include "similarity_graph.h"
#include "point_c.h"
#include "color_flood.h"

int main() {
	cv::Mat input_image = cv::imread("8means.png", cv::IMREAD_UNCHANGED);
	if (input_image.data == NULL)
		std::cout << "FAILED\n";
	cv::Mat input;
	cv::cvtColor(input_image, input, cv::COLOR_RGB2RGBA);
	cv::Mat* input_channels = new cv::Mat[4];
	cv::split(input_image, input_channels);
	input_channels[3] = cv::Mat::ones(input_image.size(), CV_8U);
	cv::merge(input_channels, 4, input);
	SimilarityGraph graph(input, SimilarityGraph::SameColor);
	BorderGraph lines = graph.ExtractDualGraphBorderChains(input);
	lines.SplitJunctions();
	std::vector<std::vector<Point2fC>> chains = lines.GetChains();
	int scale = 4;
	cv::Mat borders;
	SetBorders(borders, input.size(), chains, scale);
	std::unordered_map<PointC, cv::Vec4b> colors;
	AppoximateColorMap(input, colors, scale);
	FloodColors(borders, colors);
	cv::namedWindow("borders", cv::WINDOW_NORMAL);
	//cv::resizeWindow("borders", cv::Size(600, 600));
	cv::imshow("borders", borders);
	cv::imwrite("girl_garbage.png", borders);
	cv::waitKey(0);
}