#ifndef RidingCircuit_Included
#define RidingCircuit_Included

#include "point.h"
#include "vector.h"

/**
 * Given a collection of cities, returns a list of the order in which to visit those cities
 * in a circuit (visiting each point once and returning back to the starting point in the
 * circuit) that minimizes the total distance travelled.
 *
 * For the purposes of this problem, you should measure distance between points using the
 * Manhattan distance, in which the distance between two points is computed as
 * |p1.x - p2.x| + |p1.y - p2.y|.
 *
 * The return value from this function should be a list of all the cities in some order.
 * The interpretation of this list is that you start at the first city in the list, then
 * walk through the cities in the order they appear, and then finally return from the last
 * city in the list back to the first city in the list. For example, the list A, B, C, D
 * would mean "go from A to B, then from B to C, then from C to D, then from D back to A."
 *
 * It's possible that there are no points to visit. In that case, you should return an empty
 * list.
 *
 * If there are multiple options for the best route, you're free to pick any one of them that
 * you'd like.
 *
 * Some of the cities might be at the same position in space. If so, your solution should
 * include each copy in the output list.
 *
 * @param cities The cities that need to be visited, represented by their coordinates.
 * @return The fastest circuit that goes through all of them.
 */
Vector<Point> bestCircuitThrough(const Vector<Point>& cities);

/**
 * Given two points, computes their Manhattan distance.
 *
 * @param p1 The first point.
 * @param p2 The second point.
 * @return Their Manhattan distance.
 */
int distanceBetween(const Point& p1, const Point& p2);

#endif
