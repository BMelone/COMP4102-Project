#pragma once

#include <opencv2/opencv.hpp>
#include <unordered_map>
#include <vector>
#include "point_c.h"

void FloodColors(const cv::Mat& src, cv::Mat& dst, const std::unordered_map<PointC, cv::Vec4b>& colors);
void FloodColors(cv::Mat& dst, const std::unordered_map<PointC, cv::Vec4b>& colors);
void ApproximateColorMap(const cv::Mat& src, std::unordered_map<PointC, cv::Vec4b>& dst, int scale = 4);
void SetBorders(cv::Mat& dst, cv::Size size, const std::vector<std::vector<Point2fC>>& borders, int scale = 4);