#include "TopK.h"
#include "Testing/TopKTests.h"
#include "HeapPQueue.h"
using namespace std;

/* TODO: Refer to TopK.h for more information about what this function does, then
 * delete this comment.
 */
Vector<DataPoint> topK(istream& stream, int k) {
    /* TODO: Delete the next few lines and implement this. */
    (void) stream;
    (void) k;
    return {};
}





/* * * * * * Test Cases Below This Point * * * * * */

/* Helper function that, given a list of data points, produces a stream from them. */
stringstream asStream(const Vector<DataPoint>& dataPoints) {
    stringstream result;
    for (const auto& pt: dataPoints) {
        result << pt;
    }
    return result;
}

ADD_TEST("Works in a simple case.") {
    /* Build a stream with some simple elements in it. */
    auto stream = asStream({
        { "A", 1 }, { "B", 2 }, { "C", 3 }, { "D", 4 }
    });

    /* What we should see. */
    Vector<DataPoint> expected = {
        { "D", 4 }, { "C", 3 }, { "B", 2 }
    };

    EXPECT(topK(stream, 3) == expected);
}
