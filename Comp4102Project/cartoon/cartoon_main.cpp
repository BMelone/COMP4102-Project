#include <opencv2/opencv.hpp>
#include <iostream>
#include "similarity_graph.h"
int main() {
	cv::Mat input = cv::imread("testgrid.png");
	if (input.data == NULL)
		std::cout << "FAILED\n";
	SimilarityGraph graph(input, SimilarityGraph::SameColor);
	std::cout << std::oct << graph;
	cv::waitKey(0);
}