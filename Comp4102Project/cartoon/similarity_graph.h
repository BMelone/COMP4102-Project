#pragma once

#include <cstdint>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <unordered_set>
#include <set>
#include "point_c.h"
#include "border_graph.h"

/** @brief Graph representing similarity of pixel neighbours

Defines a graph with edges between pixels that are considered similar based on a given constraint.
The graph is an image width by image height matrix with a bitset representing the edges of the 8 surrounding pixels.
*/
class SimilarityGraph {
public:
	typedef bool(*SimilarityConstraint)(cv::Vec4b, cv::Vec4b);

	/** @brief Checks if cv::Vec3b are the same

	Compares each of the bytes of two cv::Vec3b.
	Assumes the format of @see SimilarityConstraint
	@param lhs The left cv::Vec3b to compare
	@param rhs The righ cv::Vec3b to compare
	@return True if and only if the arguments are equal
	*/
	static bool SameColor(cv::Vec4b lhs, cv::Vec4b rhs);

	/** @brief Creates a similarity graph for an image

	@param image The input image
	@param constraint function that determines if two pixels are considered similar @see SimilarityConstraint
	*/
	SimilarityGraph(const cv::Mat& image, SimilarityConstraint constraint);
	
	~SimilarityGraph();
	//! prints underlying edge graph
	friend std::ostream& operator<<(std::ostream& os, const SimilarityGraph& graph);
	void showDual(const cv::Mat& image);
	BorderGraph ExtractDualGraphBorderChains(const cv::Mat& image);
private:
	//! contains edges between similar pixels
	cv::Mat edges;

	//! converts neighbour direction vector to bitmask
	uchar NeighbourAsByte(int y, int x);

	//! checks if neighbour direction vector is diagonal
	inline bool NeighbourIsDiagonal(int y, int x) { return (x && y); }

	bool PixelIsBorder(int y, int x);

	struct corner {
		Point2fC first, second;
		bool split, first_valid, second_valid;
	};

	corner FindPixelCorner(int y, int x, int i, int j);
};