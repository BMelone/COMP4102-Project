#pragma once
/** @author Eimhin McDonald */

#include "point_c.h"

/** @brief edge between two points in 2d

The directionality of the edge does not matter
*/
template <typename T>
struct Edge {
	Point_C<T> first;
	Point_C<T> second;

	bool operator==(const Edge& rhs) const {
		return (first == rhs.first && second == rhs.second) || (second == rhs.first && first == rhs.second);
	}
};

// add hashing
namespace std {
	template <typename T> struct hash<Edge<T>> {
		size_t operator()(const Edge<T> &lhs) const {
			return hash<Point_C<T>>()(lhs.first) ^ hash<Point_C<T>>()(lhs.second);
		}
	};
}