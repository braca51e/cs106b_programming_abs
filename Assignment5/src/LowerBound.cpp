#include "LowerBound.h"
#include "Testing/LowerBoundTests.h"
using namespace std;

/* TODO: Refer to LowerBound.h for more information about this function, then
 * delete this comment.
 */
int lowerBoundOf(const Vector<DataPoint>& points, int key) {
    /* TODO: Delete the next few lines and implement this. */
    //if out of bounds
    cout << "points: " << points << endl;
    cout << "key: " << key << endl;
    if(points.size() > key){
        return points.size();
    }
    //perform binary search
    //indexing begins at 0
    int middle = (points.size()/2) - 1;
    DataPoint m_point = points[middle];
    if(m_point.weight == key){
        return middle;
    }
    else if(m_point.weight > key){
        return lowerBoundOf(points.subList(0, middle), key);
    }
    else{
        return lowerBoundOf(points.subList(middle, points.size()), key);
    }
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
