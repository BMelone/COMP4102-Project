#pragma once
/** @author Eimhin McDonald */

#include <unordered_set>
#include <unordered_map>
#include <vector>
#include "point_c.h"
#include "edge.h"

/** @brief Graph representing borders of an image

A graph that represents the borders of an image in 2d space
The graph may be reduced to contain only nodes with out-degree less than or equal 2
*/
class BorderGraph {
public:
	/** @brief Creates a new empty BorderGraph */
	BorderGraph();

	/** @brief adds an edge between points

	Adds a edge between points, duplicate edges are ignored, the edge is bidirectional
	@param first a point in the edge
	@param second a point in the edge
	*/
	void AddEdge(Point2fC first, Point2fC second);

	/** @brief adds a point to the graph

	Adds a point node to the graph, deplicate points are ignored
	@param point the point to add
	*/
	void AddPoint(Point2fC point);

	/** @brief splits the graph into a series of lists

	Splits the graph nodes that have out-degree greater than 2
	out-degree 3 nodes are allowed to maintain one chain, resulting in a node of out-degree two
	*/
	void SplitJunctions();

	/** @brief gets all of the split chains

	The graph must be split before the chains can be retrieved
	The chains are returned such that cycles are split into chains containing the same start and end node,
	and that non-cycles are split into chains such that the begining and end of the chain were split junctions
	@return the vector of chain vectors of points
	*/
	std::vector<std::vector<Point2fC>> GetChains();
private:
	std::unordered_map<Point2fC, std::vector<Point2fC>> links;
	std::unordered_set<Edge<float>> single_length; // maintains all edges that were not determined to be a single length

	//! Clears a chain segment from the single_length set because it is longer than size 1
	void ClearChainSegment(const std::vector<Point2fC>& chain);
};

