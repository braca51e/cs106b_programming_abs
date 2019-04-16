#include "LowerBound.h"
#include "Testing/LowerBoundTests.h"
using namespace std;

/* TODO: Refer to LowerBound.h for more information about this function, then
 * delete this comment.
 */
int lowerBoundOf(const Vector<DataPoint>& points, int key) {
    /* TODO: Delete the next few lines and implement this. */
    (void) points;
    (void) key;
    return 0;
}






/* * * * * * Test Cases Below This Point * * * * * */

ADD_TEST("Works in the examples from the handout.") {
    Vector<DataPoint> points = {
        { "Becquerel",  1903 },
        { "M. Curie",   1903 },
        { "P. Curie",   1903 },
        { "W. Bragg",   1915 },
        { "L. Bragg",   1915 },
        { "Einstein",   1921 },
        { "Fermi",      1938 },
        { "Strickland", 2018 }
    };

    EXPECT(lowerBoundOf(points, 1915) == 3);
    EXPECT(lowerBoundOf(points, 1900) == 0);
    EXPECT(lowerBoundOf(points, 1969) == 7);
    EXPECT(lowerBoundOf(points, 2137) == 8);
}
