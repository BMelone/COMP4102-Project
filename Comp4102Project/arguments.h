#pragma once
#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include "pixelate.h"

cv::Mat input_image, background_image;
std::string destination = ".";
int sigma_val = 5;
int threshold_val = 5;
int min_size_val = 30;
QuantizeColor color_type = KMEANS;
int down_scale = 4;
bool draw_contours = false;

static void PrintHelp()
{
	std::cout << "Help:\nThe first argument must be the input image. The following additional arguments may be added:\n"
		<< "-h\t\tShow this help message\n"
		<< "-d\tThe destination directory of the output. Defaults to current directory. e.g. '-d C:/folder'\n"
		<< "-b\tA background image to appear behind cartoon segments. Defaults to no background. e.g. '-b C:/folder/image.png'\n"
		<< "-s\tAn integer of the down-scaling factor. Defaults to " << down_scale << ". e.g. '-s 8'\n"
		<< "-c\tThe color reduction palette, either 'KMEANS', '6BIT', or 'BASIC'. Defaults to KMEANS. e.g. '-c BASIC'\n"
		<< "-g\tThe integer default sigma value for gaussian blur. Defaults to " << threshold_val << ". e.g. '-g 1'\n"
		<< "-t\tThe float default threshold value for segmentation. Defaults to " << sigma_val << ". e.g. '-c 5.0'\n"
		<< "-m\tThe integer default minimum size of the segments. Defaults to " << min_size_val << ". e.g. '-m 50'\n"
		<< "-l\tAdds contours to image before down-scaling, 'yes' or 'no'. Defaults to no. e.g. '-l yes'\n";
}

bool ParseArguments(int argc, char* argv[]) {

	if (argc < 2) {
		PrintHelp();
		return true;
	}
	if (argv[1] == "-h") {
		PrintHelp();
		return true;
	}
	input_image = cv::imread(argv[1]);
	if (input_image.data == NULL) {
		std::cout << "The input file did not exist.\n";
		PrintHelp();
		return true;
	}
	std::string input = argv[1];
	if (argc > 2 && (argc & 1)) {
		PrintHelp();
		return true;
	}
	for (int i = 2; i < argc; i+=2) {
		std::string param = argv[i];
		std::string arg = argv[i+1];
		if (param == "-d") {
			destination = arg;
		}
		else if (param == "-b") {
			background_image = cv::imread(argv[1]);
			if (background_image.data == NULL) {
				std::cout << "The background file did not exist.\n";
				PrintHelp();
				return true;
			}
		}
		else if (param == "-s") {
			try {
				down_scale = stoi(arg);
			}
			catch (...) {
				std::cout << "The scale was not an integer.\n";
				PrintHelp();
				return true;
			}
		}
		else if (param == "-c") {
			if (arg == "KMEANS") {
				color_type = KMEANS;
			}
			else if (arg == "6BIT") {
				color_type = NBIT;
			}
			else if (arg == "BASIC") {
				color_type = PALETTE;
			}
			else {
				std::cout << "The color palette was not valid.\n";
				PrintHelp();
				return true;
			}
		}
		else if (param == "-g") {
			try {
				sigma_val = (int)(stof(arg)*10);
			}
			catch (...) {
				std::cout << "The sigma was not a float.\n";
				PrintHelp();
				return true;
			}
		}
		else if (param == "-t") {
			try {
				threshold_val = stoi(arg);
			}
			catch (...) {
				std::cout << "The threshold was not an integer.\n";
				PrintHelp();
				return true;
			}
		}
		else if (param == "-m") {
			try {
				min_size_val = stoi(arg);
			}
			catch (...) {
				std::cout << "The minimum size was not an integer.\n";
				PrintHelp();
				return true;
			}
		}
		else if (param == "-l") {
			try {
				if (arg == "yes") {
					draw_contours = true;
				}
				else if (arg != "no") {
					std::cout << "The draw counters argument should be 'yes' or 'no'.\n";
					PrintHelp();
					return true;
				}
			}
			catch (...) {
				std::cout << "The minimum size was not an integer.\n";
				PrintHelp();
				return true;
			}
		}
		else {
			PrintHelp();
			return true;
		}
	}
	return false;
}