#include "RisingTides.h"
#include "Testing/RisingTidesTests.h"
using namespace std;

/* TODO: Delete this comment and implement this function. */
Grid<bool> floodedRegionsIn(const Terrain& terrain, double height) {
    /* TODO: Delete this line and the next three lines, and implement this function. */
    (void) terrain;
    (void) height;
    return {};
}


/***** Test Cases Below This Point *****/
ADD_TEST("Nothing gets wet if there are no water sources.") {
    Terrain plain;
    plain.heights.resize(3, 3);

    /* There are no water sources, so nothing should be underwater. */
    auto water = floodedRegionsIn(plain, 1.0);
    Grid<bool> expected = {
        { false, false, false },
        { false, false, false },
        { false, false, false }
    };

    EXPECT(water == expected);
}
