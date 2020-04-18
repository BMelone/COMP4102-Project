#include <iostream>
#include <opencv2/opencv.hpp>
#include "background.h"

cv::Mat addBackground(cv::Mat& fore, cv::Mat& back) {
	int height = fore.rows > back.rows ? fore.rows: back.rows;
	int width = fore.cols > back.cols ? fore.cols : back.cols;
	cv::Size foreSize = cv::Size(fore.cols, fore.rows);
	cv::Mat out = cv::Mat::zeros(foreSize, fore.type());
	cv::Mat s_back;
	cv::resize(back, s_back, foreSize);
	
	for (int i = 0; i < fore.cols; i++) {
		for (int j = 0; j < fore.rows; j++) {
			cv::Vec4b foreColor = fore.at<cv::Vec4b>(j, i);
			cv::Vec3b backColor = s_back.at<cv::Vec3b>(j, i);
			cv::Vec4b AlphaBackColor = cv::Vec4b(backColor[0], backColor[1], backColor[2], 255);
			
			if (foreColor[3]==0) {
				out.at<cv::Vec4b>(j, i) = AlphaBackColor;
			}
			else if (foreColor[3] > 125) {
				out.at<cv::Vec4b>(j, i) = foreColor;
			}
			else {
				out.at<cv::Vec4b>(j, i) = AlphaBackColor;
			}
		}
	}

	return out;
}