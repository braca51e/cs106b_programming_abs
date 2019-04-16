#include "DisasterPlanning.h"
#include "Testing/DisasterPlanningTests.h"
using namespace std;

/* TODO: Refer to DisasterPlanning.h for more information about this function.
 * Then, delete this comment.
 */
bool canBeMadeDisasterReady(const Map<string, Set<string>>& roadNetwork,
                            int numCities,
                            Set<string>& supplyLocations) {
    /* TODO: Delete the next few lines and implement this function. */
    (void) roadNetwork;
    (void) numCities;
    (void) supplyLocations;
    return false;
}




/* * * * * * * Test Cases Below This Point * * * * * */

namespace { // Private helper functions
    /* Given a road network that may or may not be symmetric, adds in reverse links to make it
     * symmetric. This makes it a lot easier to define road networks!
     */
    Map<string, Set<string>> makeSymmetric(const Map<string, Set<string>>& source) {
        Map<string, Set<string>> result = source;

        for (const string& from: source) {
            for (const string& to: source[from]) {
                result[from] += to;
                result[to] += from;
            }
        }

        return result;
    }
}

ADD_TEST("Ethane example from the handout passes.") {
    auto map = makeSymmetric({
        { "C1", {"H1", "H3", "H5", "C2"} },
        { "C2", {"H2", "H4", "H6"} },
    });

    Set<string> locations;
    EXPECT(canBeMadeDisasterReady(map, 2, locations));
    EXPECT(locations.size() == 2);
    EXPECT(locations.contains("C1"));
    EXPECT(locations.contains("C2"));
}
