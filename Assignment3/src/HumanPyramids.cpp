#include "HumanPyramids.h"
#include "Testing/HumanPyramidTests.h"
using namespace std;

/* TODO: Refer to HumanPyramids.h for more information about what this function should do.
 * Then, delete this comment.
 */
int weightOnBackOf(int row, int col) {
    /* TODO: Delete the next few lines and implement this function. */
    (void) row;
    (void) col;
    return 0;
}






/* * * * * * Test Cases * * * * * */

ADD_TEST("Check Person E from the handout.") {
    /* Person E is located at row 2, column 1. */
    EXPECT(weightOnBackOf(2, 1) == 240);
}

/* TODO: Add your own tests here. You know the drill - look for edge cases, think about
 * very small and very large cases, etc.
 */
