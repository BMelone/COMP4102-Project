#pragma once

#include "point_c.h"

template <typename T>
struct Edge {
	Point_C<T> first;
	Point_C<T> second;

	bool operator==(const Edge& rhs) const {
		return (first == rhs.first && second == rhs.second) || (second == rhs.first && first == rhs.second);
	}
};

namespace std {
	template <typename T> struct hash<Edge<T>> {
		size_t operator()(const Edge<T> &lhs) const {
			return hash<Point_C<T>>()(lhs.first) ^ hash<Point_C<T>>()(lhs.second);
		}
	};
}