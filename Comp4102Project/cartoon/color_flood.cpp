#include "color_flood.h"
#include <queue>
#include <iostream>

void FloodFill(cv::Mat& dst, PointC position, cv::Vec4b color) {
	if (color[3] == 0) return;
	std::queue<PointC> points;
	points.push(position);
	while (!points.empty()) {
		if (points.front().y < 0 || points.front().y >= dst.rows || points.front().x >= dst.cols || points.front().x < 0 || dst.at<cv::Vec4b>(points.front().y, points.front().x)[3] != 0) {
			if (points.front() == position && dst.at<cv::Vec4b>(position.y, position.x) != color) {
				std::cout << "ERROR: color issue\n";
			}
			if (!(points.front().y < 0) && !(points.front().y >= dst.rows) && !(points.front().x >= dst.cols) && !(points.front().x < 0) && dst.at<cv::Vec4b>(points.front().y, points.front().x)[3] == 128) {
				*dst.ptr<cv::Vec4b>(points.front().y, points.front().x) = color;
			}
			points.pop();
			continue;
		}
		*dst.ptr<cv::Vec4b>(points.front().y, points.front().x) = color;
		points.push(PointC(points.front().x, points.front().y + 1));
		points.push(PointC(points.front().x, points.front().y - 1));
		points.push(PointC(points.front().x + 1, points.front().y));
		points.push(PointC(points.front().x - 1, points.front().y));
		points.pop();
	}
}

void FloodColors(const cv::Mat& src, cv::Mat& dst, const std::unordered_map<PointC, cv::Vec4b>& colors) {
	src.copyTo(dst);
	FloodColors(dst, colors);
}

void FloodColors(cv::Mat& dst, const std::unordered_map<PointC, cv::Vec4b>& colors) {
	for (auto color = colors.begin(); color != colors.end(); color++) {
		FloodFill(dst, color->first, color->second);
	}
	//fix unreached diagonals
	for (int y = 0; y < dst.rows; y++) {
		for (int x = 0; x < dst.cols; x++) {
			if (dst.at<cv::Vec4b>(y, x)[3] == 128) { // still 0 holes
				if (x > 0)
					*dst.ptr<cv::Vec4b>(y, x) = dst.at<cv::Vec4b>(y, x - 1);
				else if (y > 0)
					*dst.ptr<cv::Vec4b>(y, x) = dst.at<cv::Vec4b>(y - 1, x);
			}
		}
	}
}

void SetBorders(cv::Mat& dst, cv::Size size, const std::vector<std::vector<Point2fC>>& borders, int scale) {
	dst = cv::Mat::zeros(size.height*scale + 1, size.width*scale + 1, CV_8UC4);
	cv::Scalar black(0, 0, 0, 128);
	for (std::vector<Point2fC> chain : borders) {
		for (int i = 1; i < chain.size(); i++) {
			cv::line(dst, chain.at(i-1)*scale, chain.at(i)*scale, black);
		}
	}
}

void ApproximateColorMap(const cv::Mat& src, std::unordered_map<PointC, cv::Vec4b>& dst, int scale) {
	for (int i = 0; i <= src.rows; i++) {
		for (int j = 0; j <= src.cols; j++) {
			if (i != src.rows && j != src.cols) {
				dst.insert({ PointC(j * scale + scale / 2, i * scale + scale / 2), src.at<cv::Vec4b>(i, j) });
			}
		}
	}
}
