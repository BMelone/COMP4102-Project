#include "similarity_graph.h"

SimilarityGraph::SimilarityGraph(const cv::Mat &image, SimilarityGraph::SimilarityConstraint constraint) {
	edges = cv::Mat::zeros(image.rows, image.cols, CV_8U);
	//edges pointing out of the image are removed
	for (int y = 0; y < image.rows; y++) {
		*edges.ptr<uchar>(y, 0) |= 0b00101001;
		*edges.ptr<uchar>(y, image.cols - 1) |= 0b10010100;
	}
	for (int x = 0; x < image.cols; x++) {
		*edges.ptr<uchar>(0, x) |= 0b00000111;
		*edges.ptr<uchar>(image.rows - 1, x) |= 0b11100000;
	}
	for (int y = 0; y < image.rows; y++) {
		for (int x = 0; x < image.cols; x++) {
			for (int i = -1; i <= 1; i++) {
				if ((y == 0 && i == -1) || (y == image.rows - 1 && i == 1)) continue;
				for (int j = -1; j <= 1; j++) {
					if ((x == 0 && j == -1) || (x == image.cols - 1 && j == 1)) continue;
					if (i == 0 && j == 0) continue;
					if (constraint(image.at<cv::Vec4b>(y, x), image.at<cv::Vec4b>(y + i, x + j))) {
						if (NeighbourIsDiagonal(i, j) && constraint(image.at<cv::Vec4b>(y + i, x), image.at<cv::Vec4b>(y, x + j))) {
							if (constraint(image.at<cv::Vec4b>(y, x), image.at<cv::Vec4b>(y + i, x))) {
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
									// temporary solution is to remove first seen
									*edges.ptr<uchar>(y, x) |= NeighbourAsByte(i, j);
								}
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

SimilarityGraph::~SimilarityGraph() {
	edges.release();
}

std::ostream& operator<<(std::ostream& os, const SimilarityGraph& graph) {
	os << graph.edges;
	return os;
}

bool SimilarityGraph::SameColor(cv::Vec4b lhs, cv::Vec4b rhs) {
	return lhs[0] == rhs[0] && lhs[1] == rhs[1] && lhs[2] == rhs[2] && lhs[3] == rhs[3];
}

uchar SimilarityGraph::NeighbourAsByte(int y, int x) {
	switch (y) {
	case -1:
		switch (x) {
		case -1: return 0b00000001;
		case 0:  return 0b00000010;
		case 1:  return 0b00000100;
		default: return 0;
		}
	case 0:
		switch (x) {
		case -1: return 0b00001000;
		case 1:  return 0b00010000;
		default: return 0;
		}
		break;
	case 1:
		switch (x) {
		case -1: return 0b00100000;
		case 0:  return 0b01000000;
		case 1:  return 0b10000000;
		default: return 0;
		}
	default: return 0;
	}
}

inline bool SimilarityGraph::PixelIsBorder(int y, int x) {
	//for now count diagonals as border
	return edges.at<uchar>(y, x);
}

SimilarityGraph::corner SimilarityGraph::FindPixelCorner(int y, int x, int i, int j) {
	uchar pixel = edges.at<uchar>(y, x);
	float cx = x + 0.5f, cy = y + 0.5f;
	corner result;
	result.first_valid = false;
	result.second_valid = false;
	result.split = false;
	cv::Point2f invalid(-1, -1);
	if (pixel & NeighbourAsByte(i, 0) | pixel & NeighbourAsByte(0, j)) {
		if ((y + i >= 0 && y + i < edges.rows) || (x + j >= 0 && x + j < edges.cols)) {
			if (!(pixel & NeighbourAsByte(i, j))) {
				cv::Point2f first = invalid, second = invalid;
				if ((pixel & NeighbourAsByte(i, 0))) {
					result.first_valid = true;
					result.first = Point2fC(cx + 0.25f * j, cy + 0.75f * i);
				}
				if ((pixel & NeighbourAsByte(0, j))) {
					result.second_valid = true;
					result.second = Point2fC(cx + 0.75f * j, cy + 0.25f * i);
				}
				result.split = true;
				return result;
			}
		}
		if (y + i >= 0 && y + i < edges.rows) {
			if ((pixel & NeighbourAsByte(i, 0)) && !(edges.at<uchar>(y + i, x) & NeighbourAsByte(-i, j))) {
				result.first = Point2fC(cx + 0.25f * j, cy + 0.25f * i);
				result.first_valid = true;
				return result;
			}
		}
		if (x + j >= 0 && x + j < edges.cols) {
			if ((pixel & NeighbourAsByte(0, j)) && !(edges.at<uchar>(y, x + j) & NeighbourAsByte(i, -j))) {
				result.first = Point2fC(cx + 0.25f * j, cy + 0.25f * i);
				result.first_valid = true;
				return result;
			}
		}
		result.first = Point2fC(cx + 0.5f * j, cy + 0.5f * i);
		result.first_valid = true;
		return result;
	}
	return result;
}

BorderGraph SimilarityGraph::ExtractDualGraph() {
	BorderGraph borders;
	for (int y = 0; y < edges.rows; y++) {
		for (int x = 0; x < edges.cols; x++) {
			if (!PixelIsBorder(y, x)) {
				continue;
			}
			//get corners
			corner corners[2][2];
			corners[0][0] = FindPixelCorner(y, x, -1, -1);
			corners[0][1] = FindPixelCorner(y, x, -1, 1);
			corners[1][0] = FindPixelCorner(y, x, 1, -1);
			corners[1][1] = FindPixelCorner(y, x, 1, 1);
			for (int ci = 0, i = -1; ci <= 1; ci++, i += 2) {
				for (int cj = 0, j = -1; cj <= 1; cj++, j += 2) {
					if (!corners[ci][cj].split) { // basic case
						if (corners[ci][cj].first_valid) {
							//link to adjacents
							if (edges.at<uchar>(y, x) & NeighbourAsByte(i, 0)) {
								if (corners[ci][cj ^ 1].split) {
									borders.AddEdge(corners[ci][cj].first, corners[ci][cj ^ 1].first);
								}
								else {
									if (corners[ci][cj ^ 1].first_valid) {
										borders.AddEdge(corners[ci][cj].first, corners[ci][cj ^ 1].first);
									}
								}
							}
							if (edges.at<uchar>(y, x) & NeighbourAsByte(0, j)) {
								if (corners[ci^1][cj].split) {
									borders.AddEdge(corners[ci][cj].first, corners[ci ^ 1][cj].second);
								}
								else {
									if (corners[ci^1][cj].first_valid) {
										borders.AddEdge(corners[ci][cj].first, corners[ci ^ 1][cj].first);
									}
								}
							}
						}
					}
				}
			}
		}
	}
	return borders;
}



void SimilarityGraph::showDual(const cv::Mat &image) {
	cv::Mat out = cv::Mat::zeros(edges.rows * 4, edges.cols * 4, CV_8UC4);
	for (int y = 0; y < edges.rows; y++) {
		for (int x = 0; x < edges.cols; x++) {
			uchar edge = edges.at<uchar>(y, x);
			cv::Vec4b color = image.at<cv::Vec4b>(y, x);
			if (edge & NeighbourAsByte(-1, -1) | edge & NeighbourAsByte(-1, 0) | edge & NeighbourAsByte(0, -1)) { //UL
				bool pushed = false;
				if (y > 0) {
					if ((edge & NeighbourAsByte(-1, 0)) && !(edges.at<uchar>(y - 1, x) & NeighbourAsByte(1, -1))) {
						pushed = true;
					}
				}
				else if (x > 0) {
					if ((edge & NeighbourAsByte(0, -1)) && !(edges.at<uchar>(y, x-1) & NeighbourAsByte(-1, 1))) {
						pushed = true;
					}
				}
				if (pushed) {
					cv::drawMarker(out, cv::Point(x * 4 + 1, y * 4 + 1), cv::Scalar(color[0], color[1], color[2]), 0, 1);
				}else {
					cv::drawMarker(out, cv::Point(x * 4, y * 4), cv::Scalar(color[0] + 50, color[1], color[2]), 0, 1);
				}
			}
			if (edge & NeighbourAsByte(1, -1) | edge & NeighbourAsByte(1, 0) | edge & NeighbourAsByte(0, -1)) { //LL
				bool pushed = false;
				if (y < edges.rows - 1) {
					if ((edge & NeighbourAsByte(1, 0)) && !(edges.at<uchar>(y + 1, x) & NeighbourAsByte(-1, -1))) {
						pushed = true;
					}
				}
				else if (x > 0) {
					if ((edge & NeighbourAsByte(0, -1)) && !(edges.at<uchar>(y, x - 1) & NeighbourAsByte(1, 1))) {
						pushed = true;
					}
				}
				if (pushed) {
					cv::drawMarker(out, cv::Point(x * 4 +1, (y + 1) * 4 -1), cv::Scalar(color[0], color[1], color[2]), 0, 1);
				}
				else {
					cv::drawMarker(out, cv::Point(x * 4, (y + 1) * 4), cv::Scalar(color[0] + 50, color[1], color[2]), 0, 1);
				}
			}
			if (edge & NeighbourAsByte(-1, 1) | edge & NeighbourAsByte(-1, 0) | edge & NeighbourAsByte(0, 1)) { //UR
				bool pushed = false;
				if (y > 0) {
					if ((edge & NeighbourAsByte(-1, 0)) && !(edges.at<uchar>(y - 1, x) & NeighbourAsByte(1, 1))) {
						pushed = true;
					}
				}
				else if (x < edges.cols - 1) {
					if ((edge & NeighbourAsByte(0, 1)) && !(edges.at<uchar>(y, x + 1) & NeighbourAsByte(-1, -1))) {
						pushed = true;
					}
				}
				if (pushed) {
					cv::drawMarker(out, cv::Point((x + 1) * 4 -1, y * 4 + 1), cv::Scalar(color[0], color[1], color[2]), 0, 1);
				}
				else {
					cv::drawMarker(out, cv::Point((x + 1) * 4, y * 4), cv::Scalar(color[0] + 50, color[1], color[2]), 0, 1);
				}
			}
			if (edge & NeighbourAsByte(1, 1) | edge & NeighbourAsByte(1, 0) | edge & NeighbourAsByte(0, 1)) { //LR
				bool pushed = false;
				if (y < edges.rows - 1) {
					if ((edge & NeighbourAsByte(1, 0)) && !(edges.at<uchar>(y + 1, x) & NeighbourAsByte(-1, 1))) {
						pushed = true;
					}
				}
				else if (x < edges.cols - 1) {
					if ((edge & NeighbourAsByte(0, 1)) && !(edges.at<uchar>(y, x + 1) & NeighbourAsByte(1, -1))) {
						pushed = true;
					}
				}
				if (pushed) {
					cv::drawMarker(out, cv::Point((x + 1) * 4 - 1, (y + 1) * 4 - 1), cv::Scalar(color[0], color[1], color[2]), 0, 1);
				}
				else {
					cv::drawMarker(out, cv::Point((x + 1) * 4, (y + 1) * 4), cv::Scalar(color[0] + 50, color[1], color[2]), 0, 1);
				}
			}
		}
	}
	cv::namedWindow("image", cv::WINDOW_NORMAL);
	cv::imshow("image", out);
}