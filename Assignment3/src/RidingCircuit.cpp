#include "RidingCircuit.h"
#include "Testing/RidingCircuitTests.h"
#include <cmath> // For abs
using namespace std;

/* Use this function to compute distances between points. */
int distanceBetween(const Point& p1, const Point& p2) {
    return abs(p1.getX() - p2.getX()) + abs(p1.getY() - p2.getY());
}

/* TODO: Refer to RidingCircuit.h for more information about what this function should do.
 * Then, delete this comment.
 */

//Make all possible combinations of cities
Vector<Point> bestCircuitThroughHelp(Vector<Point>& cities, Vector<Point>& chosen, Vector<Point>& best){
    //base case
    Vector<Point> ret;
    if(cities.isEmpty()){
        best = chosen;
        return best;
       }
    //explore
    //chose
    for (auto i=0; i < cities.size(); ++i) {

        Point tmp_point = cities[i];
        cities.remove(i);
        chosen.push_back(tmp_point);
        //Check if current distance is bigger than best so far
        auto new_distance = 0;
        for(auto i=0; i < chosen.size()-1; ++i){
            new_distance += distanceBetween(chosen[i], chosen[i+1]);
        }
        //Add return
        new_distance += distanceBetween(chosen[chosen.size()-1], chosen[0]);
        auto best_distance = 0;
        for(auto i=0; i < best.size()-1; ++i){
            best_distance += distanceBetween(best[i], best[i+1]);
        }
        //Add return
        if(!best.isEmpty())
            best_distance += distanceBetween(best[best.size()-1], best[0]);
        if(new_distance <= best_distance || best.isEmpty()){
            //backtrack
            bestCircuitThroughHelp(cities, chosen, best);
        }

        //unchose
        chosen.pop_back();
        cities.insert(i, tmp_point);
    }
    return ret;
}

Vector<Point> bestCircuitThrough(const Vector<Point>& cities) {
    /* TODO: Delete the next few lines and implement this function. */
    //(void) cities;
    Vector<Point> tmp_cities;
    Vector<Point> chosen;
    Vector<Point> best;

    for(auto i=0; i < cities.size(); ++i){
        tmp_cities.push_back(cities[i]);
    }
    //Cities
    cout << "Cities: " << tmp_cities << endl;
    //check all possible cobinations
    bestCircuitThroughHelp(tmp_cities, chosen, best);
    cout << "best: " << best << endl;

    return best;
}

/* * * * * * Test Cases * * * * * */

/* This utility function returns whether two circuits are identically equal to
 * one another. There can be many different ways of expressing the same circuit.
 * For example, we can start anywhere on the circuit without changing its value,
 * and we can reverse a circuit and get the same circuit back.
 *
 * You can use this function in cases where there is a unique best circuit. Pass
 * in the expected circuit and what was actually generated, and we'll see whether
 * these are the same circuit.
 *
 * This is mostly for testing purposes. You're not expected to use this in your
 * implementation of the bestCircuitThrough function.
 *
 * Be careful - this function doesn't return whether two solutions have the same
 * length. It only checks if they're literally the same solution, just expressed
 * differently.
 */
bool areEquivalent(const Vector<Point>& one, const Vector<Point>& two) {
    /* If two circuits don't have the same size, they can't be equivalent. */
    if (one.size() != two.size()) return false;

    /* If either are empty, they're equivalent. */
    if (one.isEmpty()) return true;

    /* If both have length one, they're equivalent precisely if they have
     * the same element.
     */
    if (one.size() == 1) return one[0] == two[0];

    /* Find where the first point on the first circuit is on the second
     * circuit.
     */
    int start = two.indexOf(one[0]);
    if (start == -1) return false; // Oops, not there.

    /* Look at the next point in the first circuit. That should be adjacent
     * to the equivalent point in the second circuit. The question is where.
     */
    int step;
    if (two[(start + 1) % two.size()] == one[1]) { // Forward
        step = 1;
    } else if (two[(start + (two.size() - 1)) % two.size()] == one[1]) { // Backward
        step = two.size() - 1; // Equivalent to -1
    } else { // Oops, neither one matches.
        return false;
    }

    /* Walk the circuits, confirming everything matches. */
    for (int i = 0; i < one.size(); i++) {
        if (one[i] != two[(start + i * step) % two.size()]) return false;
    }

    /* Hey, they're equal! */
    return true;
}

ADD_TEST("Route includes each city exactly once.") {
    Vector<Point> cities = {
        { 0, 0 },
        { 1, 1 }
    };

    /* See ShiftScheduling.cpp for more about the auto keyword. */
    auto route = bestCircuitThrough(cities);
    EXPECT(route.size() == 2);

    /* This route should be equivalent to one that cycles through the two cities
     * in whatever order we'd like.
     */
    EXPECT(areEquivalent(route, { cities[0], cities[1] }));
}

ADD_TEST("Finds optimal route in a square.") {
    Vector<Point> cities = {
        { 0, 0 },
        { 1, 1 },
        { 0, 1 },
        { 1, 0 }
    };

    auto route = bestCircuitThrough(cities);

    /* The best route should cycle through the points as they go around the
     * square. Notice that we aren't saying that the path must LITERALLY be
     * these cities in this order - the areEquivalent function checks if two
     * paths are the same, possibly including the option that they start in
     * different places or go in a different direction - but rather that the
     * circuit described here matches the circuit returned by the function.
     */
    EXPECT(areEquivalent(route, { {1, 0}, {1, 1}, {0, 1}, {0, 0} }));
}

ADD_TEST("Finds optimal route on a line.") {
    /* These cities are stored in the same relative order as you'd expect
     * to find them in a solution.
     */
    Vector<Point> cities = {
        { 0, 0 },
        { 2, 0 },
        { 4, 0 },
        { 6, 0 },
        { 8, 0 },
        { 9, 0 }
    };

    auto route = bestCircuitThrough(cities);

    /* This check is okay because (1) the cities are stored in an order that happens to
     * be optimal and (2) because areEquivalent allows for the possibility that the circuits
     * are the same after we rotate or mirror them, two operations that don't actually
     * change which circuit is being described.
     */
    EXPECT(areEquivalent(route, cities));
}

ADD_TEST("Handles duplicate points.") {
    /* I guess these cities are all stacked on top of one another. Maybe there's
     * a Z axis we aren't taking into account? :-)
     */
    Vector<Point> cities = {
        { 0, 0 },
        { 0, 0 },
        { 0, 0 },
        { 0, 0 },
        { 0, 0 },
        { 0, 0 }
    };

    auto route = bestCircuitThrough(cities);

    /* Should visit multiple copies of the same city. */
    EXPECT(route.size() == cities.size());

    /* All points in the route should be the same. */
    for (Point pt: route) {
        EXPECT(pt == cities[0]);
    }
}

/* TODO: Add your own tests here. You know the drill - look for edge cases, think about
 * very small and very large cases, etc.
 */
