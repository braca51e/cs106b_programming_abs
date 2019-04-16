#include "WinningThePresidency.h"
#include "Testing/WinningThePresidencyTests.h"
using namespace std;

/**
 * Given a list of states, a target number of electoral votes, and a start index into that list
 * of states, returns the minimum number of popular votes you'd need to get that many electoral
 * votes, along with which states you'd use to make that target.
 *
 * If it's not possible to hit the target number of electoral votes, you should return a MinInfo
 * struct that has the number of popular votes set to the constant kNotPossible. The set of
 * associated states can be whatever you'd like it to be in that case.
 *
 * When you're implementing your preliminary version of this function, you MUST NOT add any extra
 * parameters to this function, and you MUST NOT make this a wrapper around another helper
 * function. You should only edit the signature of this function when you're adding in
 * memoization.
 *
 * @param electoralVotes The target number of electoral votes to hit.
 * @param startIndex The starting index into the Vector of which states you're allowed to use.
 * @param states All the states participating in the election.
 * @return Information about the fewest number of popular votes needed to hit the target number
 *         of electoral votes using only states from index startIndex and beyond.
 */
MinInfo minPopularVoteToGetAtLeast(int electoralVotes,
                                   int startIndex,
                                   const Vector<State>& states) {
    /* TODO: Delete the next few lines and implement this function. */
    (void) electoralVotes;
    (void) startIndex;
    (void) states;
    return { kNotPossible, {} };
}

/* TODO: Refer to WinningThePresidency.h for more information about this function.
 * Then, delete this comment.
 */
MinInfo minPopularVoteToWin(const Vector<State>& states) {
    /* TODO: Delete the next few lines and implement this function. */
    (void) states;
    return { kNotPossible, {} };
}






/* * * * * * Test Cases Below This Point * * * * * */

/* These tests are designed to test the helper function minPopularVoteToGetAtLeast. When you
 * add in memoization, you will need to change these tests to edit how you call the function.
 */
ADD_TEST("Can't conjure votes out of thin air.") {
    EXPECT(minPopularVoteToGetAtLeast(1, 0, {}).popularVotesNeeded == kNotPossible);
}




/* These tests are designed to test your top-level minPopularVoteToWin function. You'll
 * need to add in more test cases here, but you shouldn't need to change any of these tests
 * at any point during your design.
 */
ADD_TEST("If there's only one state, you have to pick it.") {
    Vector<State> states = {
        { "Denial", 1, 1 }
    };

    auto soln = minPopularVoteToWin(states);
    EXPECT(soln.popularVotesNeeded == 1);
    EXPECT(soln.statesUsed == states);
}
