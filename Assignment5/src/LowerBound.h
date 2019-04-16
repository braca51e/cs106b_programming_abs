#ifndef LowerBound_Included
#define LowerBound_Included

#include "DataPoint.h"
#include "vector.h"

/**
 * Given a list of DataPoints sorted in ascending order by weight, along with a weight
 * called the key, returns the index of the first spot in that list whose weight is
 * greater than or equal to the key.
 *
 * If the key is strictly greater than all of the weights in the list, this function
 * should return the length of the input list, which corresponds to the position right
 * after the very last element.
 *
 * If the key is less or equal to than all of the weights in the list, this function
 * should return 0, even if the list is empty.
 *
 * There may be many different keys in the list with the same weight. Make sure to
 * take this into account!
 *
 * This function should run in time O(log n), where n is the number of elements in the
 * list.
 *
 * @param points The list of data points, sorted by weight.
 * @param key    The key point.
 * @return The index of the first element in the list of points whose weight is greater than
 *         or equal to the key weight, or points.size() if the key is greater than all of the
 *         weights in the list.
 */
int lowerBoundOf(const Vector<DataPoint>& points, int key);


#endif
