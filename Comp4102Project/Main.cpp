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
#include "background.h"
#include "arguments.h"

const int threshold_max = 20;
const int min_size_max = 200;
const int sigma_max = 15;
cv::Mat img, background;
int num_ccs;
float sigma, threshold;
int min_size;

universe* u;

std::vector<int> xVector, yVector;
std::unordered_set<int> savedSegments;

/*
	Updates slider for segmentation arguments and display new segments
*/
void segment_and_display() {

	sigma = sigma_val / 10.0f;
	threshold = threshold_val * 50;
	min_size = min_size_val * 100;
	cv::Mat output = segment_image(input_image, sigma, threshold, min_size, &num_ccs, u);
	cv::imshow("sliders", output);
	cv::imwrite(destination + "/o_segments.png", output);
}

/*
	get sigma value from slider and update window
*/
void sigma_and_display(int slider_val, void* data) {
	sigma_val = slider_val;
	segment_and_display();
}

/*
	gets threshold value from slider and update window
*/
void threshold_and_display(int slider_val, void* data) {
	threshold_val = slider_val;
	segment_and_display();
}

/*
	gets minimum size value from slider and update window
*/
void min_size_and_display(int slider_val, void* data) {
	min_size_val = slider_val;
	segment_and_display();
}

/*
	gets selected segment and saves it
*/
void select_location(int event, int x, int y, int flags, void* param) {
	if (event == cv::EVENT_LBUTTONDOWN)
	{
		xVector.push_back(x);
		yVector.push_back(y);
		savedSegments.insert(u->find(y * input_image.cols + x)); //save the selected segment
	}
}

/*
	Runs the application, first showing segmentation, then the pixelization, then depixelization, and finally adds background
*/
int main(int argc, char* argv[])
{
	if (ParseArguments(argc, argv)) {
		return 0;
	}
	cv::namedWindow("sliders", cv::WINDOW_NORMAL);
	cv::resizeWindow("sliders", 500, 500);

	cv::createTrackbar("Sigma", "sliders", &sigma_val, sigma_max, sigma_and_display);
	cv::createTrackbar("Threshold","sliders", &threshold_val, threshold_max, threshold_and_display);
	cv::createTrackbar("Min Size", "sliders", &min_size_val, min_size_max, min_size_and_display);
	cv::setMouseCallback("sliders", select_location, 0);

	segment_and_display();

	cv::waitKey(0);
	cv::destroyWindow("sliders");
	cv::namedWindow("Segmentation", cv::WINDOW_NORMAL);
	cv::resizeWindow("Segmentation", 500, 500);

	cv::Vec4b transparent(0,0,0,0);
	int width = input_image.cols;
	int height = input_image.rows;
	cv::Mat segmented_output = cv::Mat::zeros(cv::Size(width, height), CV_8UC4);
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int comp = u->find(y * width + x);
			cv::Vec3b img_pixel = input_image.at<cv::Vec3b>(y, x);
			segmented_output.at<cv::Vec4b>(y, x) = savedSegments.find(comp) != savedSegments.end() ? 
				cv::Vec4b(img_pixel[0], img_pixel[1], img_pixel[2], 255) : transparent;
		}
	}
	
	cv::namedWindow("Segmentation", cv::WINDOW_NORMAL);
	cv::resizeWindow("Segmentation", 500, 500);
	cv::imshow("Segmentation", segmented_output);
	cv::imwrite(destination + "/o_foreground.png", segmented_output);
	cv::waitKey(0);
	cv::destroyWindow("Segmentation");

	
	cv::namedWindow("Pixelate", cv::WINDOW_NORMAL);
	cv::resizeWindow("Pixelate", 500, 500);
	cv::Mat pixelate_output = pixelate(segmented_output, down_scale, pixel_sigma_val, color_type, draw_contours);
	cv::imshow("Pixelate", pixelate_output);
	cv::imwrite(destination+"/o_pixelated.png", pixelate_output);
	cv::waitKey(0);
	cv::destroyWindow("Pixelate");
	
	cv::namedWindow("DePixelization", cv::WINDOW_NORMAL);
	cv::resizeWindow("DePixelization", 500, 500);
	SimilarityGraph similarity_graph(pixelate_output, SimilarityGraph::SameColor);
	BorderGraph border_graph = similarity_graph.ExtractDualGraph();
	border_graph.SplitJunctions();
	cv::Mat cartoon_output;
	SetBorders(cartoon_output, pixelate_output.size(), border_graph.GetChains(), down_scale);
	std::unordered_map<PointC, cv::Vec4b> colors;
	ApproximateColorMap(pixelate_output, colors, down_scale);
	FloodColors(cartoon_output, colors);
	cv::imshow("DePixelization", cartoon_output);
	cv::imwrite(destination + "/o_cartoon.png", cartoon_output);
	cv::waitKey(0);
	cv::destroyWindow("DePixelization");
	if (!background_image.empty()) {
		cv::namedWindow("Final Image", cv::WINDOW_NORMAL);
		cv::resizeWindow("Final Image", 500, 500);
		cv::Mat final = addBackground(cartoon_output, background_image);
		cv::imshow("Final Image", final);
		cv::imwrite(destination+"/o_final.png", final);
		cv::waitKey(0);
		cv::destroyWindow("Final Image");
	}
	return 0;
}