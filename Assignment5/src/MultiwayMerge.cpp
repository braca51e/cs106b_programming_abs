#include "MultiwayMerge.h"
#include "Testing/MergeTests.h"
using namespace std;

/* TODO: Refer to MultiwayMerge.h for more information about what this function does,
 * then delete this comment.
 */
Vector<DataPoint> mergeSequences(const Vector<Vector<DataPoint>>& sequences) {
    /* TODO: Delete the next few lines and implement this. */
    (void) sequences;
    return {};
}

/* * * * * * Tests Below This Point * * * * * */

ADD_TEST("Works in the example from the handout.") {
    Vector<Vector<int>> sequences = {
        { 3, 8, 10, 11, 13, 16 },
        { 0, 1, 12 },
        { 4, 9, 17, 19 },
        { 7 },
        { 5, 6, 14, 18 },
        { 2, 15 }
    };

    /* The example in the handout uses integers, but we need DataPoints. Tag each integer
     * with an empty string.
     */
    Vector<Vector<DataPoint>> dataPoints;
    for (auto sequence: sequences) {
        Vector<DataPoint> result;
        for (int elem: sequence) {
            result += { "", elem };
        }
        dataPoints += result;
    }

    /* Merge them! */
    auto merged = mergeSequences(dataPoints);

    EXPECT(merged.size() == 20);
    for (int i = 0; i < 20; i++) {
        EXPECT(merged[i].weight == i);
    }
}
