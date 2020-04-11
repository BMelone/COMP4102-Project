#include "similarity_graph.h"

SimilarityGraph::SimilarityGraph(const cv::Mat &image, SimilarityGraph::SimilarityConstraint constraint) {
	edges = cv::Mat::zeros(image.rows, image.cols, CV_8U);
	//edges pointing out of the image are kept
	for (int y = 0; y < image.rows; y++) {
		for (int x = 0; x < image.cols; x++) {
			for (int i = -1; i <= 1; i++) {
				if ((y == 0 && i == -1) || (y == image.rows - 1 && i == 1)) continue;
				for (int j = -1; j <= 1; j++) {
					if ((x == 0 && j == -1) || (x == image.cols - 1 && j == 1)) continue;
					if (i == 0 && j == 0) continue;
					if (constraint(image.at<cv::Vec3b>(y, x), image.at<cv::Vec3b>(y + i, x + j))) {
						if (NeighbourIsDiagonal(i, j) && constraint(image.at<cv::Vec3b>(y + i, x), image.at<cv::Vec3b>(y, x + j))) {
							if (constraint(image.at<cv::Vec3b>(y, x), image.at<cv::Vec3b>(y + i, x))) {
								//delete both blue x
								*edges.ptr<uchar>(y, x) |= NeighbourAsByte(i, j);
							}
							else {// red x
								if (edges.at<uchar>(y + i, x + j) & NeighbourAsByte(-i, -j)) {// already lost
									*edges.ptr<uchar>(y, x) |= NeighbourAsByte(i, j);	
								}
								else if (edges.at<uchar>(y + i, x) & NeighbourAsByte(-i, j) || edges.at<uchar>(y, x + j) & NeighbourAsByte(i, -j)){
									continue;//already won
								}
								else {//must resolve issue
									std::cout << "y: " << y << " x: " << x << " i: " << i << " j: " << j << " issue must be resolved\n";
									// temporary solution is to remove first seen
									*edges.ptr<uchar>(y, x) |= NeighbourAsByte(i, j);
								}
							}							
						}
						else {
							*edges.ptr<uchar>(y, x) |= NeighbourAsByte(i, j);
						}
					}
				}
			}
		}
	}
}

SimilarityGraph::~SimilarityGraph() {
	edges.release();
}

std::ostream& operator<<(std::ostream& os, const SimilarityGraph& graph) {
	os << graph.edges;
	return os;
}

bool SimilarityGraph::SameColor(cv::Vec3b lhs, cv::Vec3b rhs) {
	return lhs[0] == rhs[0] && lhs[1] == rhs[1] && lhs[2] == rhs[2];
}

uchar SimilarityGraph::NeighbourAsByte(int y, int x) {
	switch (y) {
	case -1:
		switch (x) {
		case -1: return 1;
		case 0: return 2;
		case 1: return 4;
		default: return 0;
		}
	case 0:
		switch (x) {
		case -1: return 8;
		case 1: return 16;
		default: return 0;
		}
	case 1:
		switch (x) {
		case -1: return 32;
		case 0: return 64;
		case 1: return 128;
		default: return 0;
		}
	default: return 0;
	}
}