#pragma once 

#include <unordered_set>
#include <unordered_map>
#include <vector>
#include "point_c.h"
#include "edge.h"

class BorderGraph {
public:
	BorderGraph();
	void AddEdge(Point2fC first, Point2fC second);
	void AddPoint(Point2fC point);
	void SplitJunctions();
	std::vector<std::vector<Point2fC>> GetChains();
private:
	std::unordered_map<Point2fC, std::vector<Point2fC>> links;
	std::unordered_set<Edge<float>> single_length;
	void ClearChainSegment(const std::vector<Point2fC>& chain);
};

