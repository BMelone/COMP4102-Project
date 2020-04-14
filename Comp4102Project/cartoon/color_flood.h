#pragma once
/** @author Eimhin McDonald */

#include <opencv2/opencv.hpp>
#include <unordered_map>
#include <vector>
#include "point_c.h"

/** @brief fills empty pixels of an image

Fills the empty pixels of an image using a map of colors from pixel coordinates
The src image should contain image boundarys which are used to define flooding in dst image
@param src the input inmage
@param dst the output image
@param colors a map of pixel coordinates to image colors to fill
*/
void FloodColors(const cv::Mat& src, cv::Mat& dst, const std::unordered_map<PointC, cv::Vec4b>& colors);

/** @brief fills empty pixels of an image

Fills the empty pixels of an image using a map of colors from pixel coordinates
The dst image should contain image boundarys which are used to define flooding
@param dst the output image that is overwritten
@param colors a map of pixel coordinates to image colors to fill
*/
void FloodColors(cv::Mat& dst, const std::unordered_map<PointC, cv::Vec4b>& colors);

/** @brief creates a color map based on an image

Approximates the color map by brute forcing a dense mapping of input pixel colors to the 2d map space
@param src the input image
@param dst the output color map
@param scale scaling factor of output color map
*/
void ApproximateColorMap(const cv::Mat& src, std::unordered_map<PointC, cv::Vec4b>& dst, int scale = 4);

/** @brief draws paths onto image

@param dst the output image
@param size the new size of the output image
@param borders list of paths stored as a list of points
@param scale scaling factor of output image
*/
void SetBorders(cv::Mat& dst, cv::Size size, const std::vector<std::vector<Point2fC>>& borders, int scale = 4);