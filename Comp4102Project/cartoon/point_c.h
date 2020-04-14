#pragma once
/** @author Eimhin McDonald */

#include <opencv2\opencv.hpp>

/** @brief extension of OpenCV Point, with comparability

The point are comparable such that the vertical coordinate of a point dominates the horizontal coordinate
*/
template<typename _Tp> class Point_C : public cv::Point_<_Tp>
{
public:
	Point_C() : cv::Point_<_Tp>() {}
	Point_C(_Tp _x, _Tp _y) : cv::Point_<_Tp>(_x, _y) {}
	Point_C(const cv::Point_< _Tp>& pt) : cv::Point_<_Tp>(pt) {}
	Point_C(const cv::Size_<_Tp>& sz) : cv::Point_<_Tp>(sz) {}
	Point_C(const cv::Vec<_Tp, 2>& v) : cv::Point_<_Tp>(v) {}
	friend bool operator<(const Point_C<_Tp>& a, const Point_C<_Tp>& b) {
		return (a.y < b.y) || (a.y == b.y && a.x < b.x);
	}
};

// define common typedefs, extensions of Openv Point
typedef Point_C<int> Point2iC;
typedef Point_C<int64> Point2lC;
typedef Point_C<float> Point2fC;
typedef Point_C<double> Point2dC;
typedef Point2iC PointC;

// add hashing
namespace std {
	template <typename _Tp> struct hash<Point_C<_Tp>> {
		size_t operator()(const Point_C<_Tp> &lhs) const {
			return hash<_Tp>()(lhs.x) ^ hash<_Tp>()(lhs.y);
		}
	};
}