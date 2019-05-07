#include "ShiftScheduling.h"
#include "Testing/ShiftSchedulingTests.h"
#include "vector.h"
using namespace std;

/* TODO: Refer to ShiftScheduling.h for more information about what this function should do.
 * Then, delete this comment.
 */

//Use to get all possible groups of shifts
Set<Shift> highestValueScheduleForHelp(Set<Shift>& shifts, int maxHours, Set<Shift>& chosen, Set<Shift>& best) {
    //base case
    //shift is empty or max houseris bigger that current shift
    //cout << "shifts size: " << shifts.size() << endl;
    if(shifts.isEmpty()){
        if(maxHours >= 0){
            //cout << "chosen: " << chosen << endl;
            int maxInShift = 0;
            int maxInBest = 0;
            for(Set<Shift>::iterator it=chosen.begin(); it!=chosen.end(); it++){
                maxInShift += valueOf(*it);
            }
            for(Set<Shift>::iterator it=best.begin(); it!=best.end(); it++){
                maxInBest += valueOf(*it);
            }
            if(maxInShift >= maxInBest)
                best = chosen;
        }
        return best;
    }
    //do backtracking
    //select
    //Create a sub-group of shifts
    Shift s_tmp = shifts.front();
    shifts.remove(s_tmp);
    //cout << "shifts size: " << shifts.size() << endl;

    //choose/explore
    //try without this shift
    highestValueScheduleForHelp(shifts, maxHours, chosen, best);

    //try with this shiift
    //TODO check for overlaps before trying and there is time for it
    bool overlaps = false;
    for(Set<Shift>::iterator it=chosen.begin(); it!=chosen.end(); it++){
        if(overlapsWith(*it, s_tmp) || (maxHours - lengthOf(s_tmp)) < 0){
            overlaps = true;
            break;
        }
    }
    if(!overlaps){
        chosen.add(s_tmp);
        highestValueScheduleForHelp(shifts, maxHours - lengthOf(s_tmp), chosen, best);
    }

    //unchoose
    shifts.add(s_tmp);
    chosen.remove(s_tmp);

    return chosen;

}

Set<Shift> highestValueScheduleFor(const Set<Shift>& shifts, int maxHours) {
    /* TODO: Delete the next few lines and implement this function. */
    //base case
    if(maxHours < 0)
        error("maxHouers should not be negative!");

    Set<Shift> chosen;
    Set<Shift> best;
    Set<Shift> local_shifts;
    for(Set<Shift>::iterator it=shifts.begin(); it != shifts.end(); it++){
        local_shifts.add(*it);
    }
    //cout << "local_shifts!!!!!: " << local_shifts << endl;
    //cout << "Initiali maxHours!!!!!!: " << maxHours << endl;
    highestValueScheduleForHelp(local_shifts, maxHours, chosen, best);
    //cout << "best shift: " << best << endl;
    return best;
}

/* * * * * * Test Cases * * * * * */

/* This nice utility function lets you call highestValueScheduleFor, passing in
 * a Vector of shifts rather than a Set. This makes it a bit easier to test things.
 * You shouldn't need this function outside of these test cases.
 */
Set<Shift> asSet(const Vector<Shift>& shifts) {
    Set<Shift> result;
    for (Shift s: shifts) {
        result += s;
    }
    return result;
}

ADD_TEST("Pick a shift you do have time for.") {
    Set<Shift> shifts = {
        { Day::MONDAY, 9, 17, 1000 },  // Monday, 9AM - 5PM, value is 1000
    };

    EXPECT(highestValueScheduleFor(shifts, 24).size() == 1);
    EXPECT(highestValueScheduleFor(shifts, 24) == shifts);
}

ADD_TEST("Don't pick a shift you don't have time for.") {
    Set<Shift> shifts = {
        { Day::MONDAY, 9, 17, 1000 },  // Monday, 9AM - 5PM, value is 1000
    };

    EXPECT(highestValueScheduleFor(shifts, 3).isEmpty());
}

ADD_TEST("Don't pick overlapping shifts.") {
    Vector<Shift> shifts = {
        { Day::MONDAY, 9, 17, 1000 },  // Monday,  9AM - 5PM, value is 1000
        { Day::MONDAY, 8, 18, 2000 },  // Monday, 10AM - 6PM, value is 2000
    };

    EXPECT(highestValueScheduleFor(asSet(shifts), 100).size() == 1);
    EXPECT(highestValueScheduleFor(asSet(shifts), 100).contains(shifts[1]));
}

ADD_TEST("Passes the example from the handout.") {
    Vector<Shift> shifts = {
        { Day::MONDAY,     8, 12, 27 },  // Mon  8AM - 12PM, value 27 *
        { Day::MONDAY,    12, 16, 28 },  // Mon 12PM -  4PM, value 28 *
        { Day::MONDAY,    16, 20, 25 },  // Mon  4PM -  8PM, value 25 *
        { Day::MONDAY,     8, 14, 39 },  // Mon  8AM -  2PM, value 39
        { Day::MONDAY,    14, 20, 31 },  // Mon  2PM -  8PM, value 31
        { Day::TUESDAY,    8, 12,  7 },  // Tue  8AM - 12PM, value  7
        { Day::TUESDAY,   12, 16,  7 },  // Tue 12PM -  4PM, value  7
        { Day::TUESDAY,   16, 20, 11 },  // Tue  4PM -  8PM, value 11
        { Day::TUESDAY,    8, 14, 10 },  // Tue  8AM -  2PM, value 10
        { Day::TUESDAY,   14, 20,  8 },  // Tue  2PM -  8PM, value  8
        { Day::WEDNESDAY,  8, 12, 10 },  // Wed  8AM - 12PM, value 10
        { Day::WEDNESDAY, 12, 16, 11 },  // Wed 12PM -  4PM, value 11
        { Day::WEDNESDAY, 16, 20, 13 },  // Wed  4PM -  8PM, value 13
        { Day::WEDNESDAY,  8, 14, 19 },  // Wed  8AM -  2PM, value 19
        { Day::WEDNESDAY, 14, 20, 25 },  // Wed  2PM -  8PM, value 25 *
    };

    /* Get back the solution.
     *
     * We're using the cool C++ auto keyword here. Declaring a variable as
     *
     *    auto var = value;
     *
     * will have C++ figure out what the right type for var is. It's a nice
     * way to avoid having to explicitly write out type information.
     */
    auto computedSolution = highestValueScheduleFor(asSet(shifts), 20);

    /* Form the correct answer. It's the starred entries. */
    Set<Shift> actualSolution = {
        shifts[0], shifts[1], shifts[2], shifts[14]
    };

    EXPECT(computedSolution == actualSolution);
}

/* TODO: Add your own tests here. You know the drill - look for edge cases, think about
 * very small and very large cases, etc.
 */
