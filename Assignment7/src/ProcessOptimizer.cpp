#include "ProcessOptimizer.h"
#include "Testing/OptimizerTests.h"
using namespace std;

/* TODO: Delete this comment and implement this function. */
HashSet<Task *> optimizationCandidatesFor(const HashSet<Task *>& process) {
    /* TODO: Delete this line and the two lines after it, then implement this function. */
    (void) process;
    return {};
}

/***** Test Cases Below This Point *****/

ADD_TEST("Nothing can be optimized in the empty process.") {
    EXPECT(optimizationCandidatesFor({}).isEmpty());
}
