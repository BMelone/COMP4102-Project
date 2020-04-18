#pragma once
//
// Created by ryan on 4/11/20.
//
#include <iostream>
#include <opencv2/opencv.hpp>
#include <cmath>
#include <unordered_set>
enum QuantizeColor { NBIT, PALETTE, KMEANS };
cv::Mat restrict_color_nbit(cv::Mat& im1);
cv::Mat restrict_color_palette(cv::Mat& im1, std::vector<cv::Vec4b> palette);
cv::Mat restrict_color_kMeans(cv::Mat& im1, int ksplits);
cv::Mat pixelate(cv::Mat& img, int factor, float sigma, QuantizeColor opt = KMEANS, bool contours = false);