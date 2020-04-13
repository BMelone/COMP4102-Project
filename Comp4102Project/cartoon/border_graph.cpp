#include "border_graph.h"
#include <unordered_set>
#include <cmath>
#include <algorithm>
BorderGraph::BorderGraph() {}

void BorderGraph::ClearChainSegment(const std::vector<Point2fC>& chain) {
	for (int i = 1; i < chain.size(); i++) {
		single_length.erase({ chain.at(i), chain.at(i - 1) });
	}
}

std::vector<std::vector<Point2fC>> BorderGraph::GetChains() {
	std::vector<std::vector<Point2fC>> chains;
	std::unordered_set<Point2fC> seen;
	for (std::pair<Point2fC, std::vector<Point2fC>> point : links) {
		if (seen.find(point.first) == seen.end()) {
			if (point.second.size() == 0) continue;
			Point2fC current = point.first, previous = point.first;
			std::vector<Point2fC> chain;
			while (true) {
				if (current != previous && std::find(links.at(current).begin(), links.at(current).end(), previous) == links.at(current).end()) {
					// handle non-cycle
					chain.clear();
					chain.push_back(current);
					seen.insert(current);
					std::swap(current, previous);
					while (std::find(links.at(current).begin(), links.at(current).end(), previous) != links.at(current).end()) {
						chain.push_back(current);
						seen.insert(current);
						if (links.at(current).at(0) == previous) {
							previous = current;
							current = links.at(current).at(1);
						}
						else {
							previous = current;
							current = links.at(current).at(0);
						}
					}
					chain.push_back(current);
					chains.push_back(chain);
					ClearChainSegment(chain);
					break;
				}
				chain.push_back(current);
				if (current == point.first && seen.find(point.first) != seen.end()) { 
					chains.push_back(chain);
					ClearChainSegment(chain);
					break;
				}
				seen.insert(current);
				if (links.at(current).at(0) == previous) {
					previous = current;
					current = links.at(current).at(1);
				}
				else {
					previous = current;
					current = links.at(current).at(0);
				}
			}
		}
	}
	for (auto edge = single_length.begin(); edge != single_length.end(); edge++) {
		std::vector<Point2fC> chain;
		chain.push_back(edge->first);
		chain.push_back(edge->second);
		chains.push_back(chain);
	}
	return chains;
}

void BorderGraph::SplitJunctions() {
	for (auto point = links.begin(); point != links.end(); point++) {
		if (point->second.size() == 4) {
			point->second.clear();
		}
		else if (point->second.size() == 3) {
			Point2fC a = point->second.at(0) - point->first;
			Point2fC b = point->second.at(1) - point->first;
			Point2fC c = point->second.at(2) - point->first;
			float cos_ab = a.dot(b) / (sqrt(a.dot(a)) * sqrt(b.dot(b)));
			float cos_bc = b.dot(c) / (sqrt(c.dot(c)) * sqrt(b.dot(b)));
			float cos_ac = a.dot(c) / (sqrt(a.dot(a)) * sqrt(c.dot(c)));
			if (cos_ab < cos_bc && cos_ab < cos_ac) {
				point->second.erase(point->second.begin() + 2);
			}
			else if (cos_bc < cos_ac && cos_bc < cos_ab) {
				point->second.erase(point->second.begin());
			}
			else {
				point->second.erase(point->second.begin() + 1);
			}
		}
	}
}

void BorderGraph::AddPoint(Point2fC point) {
	if (links.find(point) == links.end()) {
		std::vector<Point2fC> connections;
		links.insert({ point, connections });
	}
}

void BorderGraph::AddEdge(Point2fC first, Point2fC second) {
	AddPoint(first);
	AddPoint(second);
	single_length.insert({ first,second });
	if (std::find(links.at(first).begin(), links.at(first).end(), second) == links.at(first).end())
		links.at(first).push_back(second);
	if (std::find(links.at(second).begin(), links.at(second).end(), first) == links.at(second).end())
		links.at(second).push_back(first);
}