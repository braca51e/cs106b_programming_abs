#ifndef WinningThePresidency_Included
#define WinningThePresidency_Included

#include "vector.h"
#include <string>
#include <ostream>

/**
 * Constant representing that there is no way to win the election with the given number of votes.
 */
static const int kNotPossible = 1000000000;

/**
 * Type representing a state in an election.
 */
struct State {
    std::string name;   // The name of the state
    int electoralVotes; // How many electors it has
    int popularVotes;   // The number of people in that state who voted
};

/**
 * Type representing information about how to win an election with the fewest number
 * of popular votes.
 */
struct MinInfo {
    int popularVotesNeeded;   // How many popular votes you'd need.
    Vector<State> statesUsed; // Which states you'd carry in the course of doing so.
};

/**
 * Given a list of all the states in an election, including their popular and electoral vote
 * totals, returns information about how few popular votes you'd need to win in order to win
 * the presidency.
 *
 * @param states All the states in the election (plus DC, if appropriate)
 * @return Information about how few votes you'd need to win the election.
 */
MinInfo minPopularVoteToWin(const Vector<State>& states);





/* * * * * Printing Routines * * * * */

/* This functions allow you to print out our structs using cout. You don't need to use these
 * functions, but they might be helpful during testing. They're implemented in Operators.cpp
 * in case you're curious how they work.
 */
std::ostream& operator<< (std::ostream& out, const State& state);
std::ostream& operator<< (std::ostream& out, const MinInfo& info);

/* * * * * Comparison Routines * * * * */

/* These comparison routines allow you to toss States into Sets and HashSets, if you so choose.
 * It's not required that you do this, though.
 */
bool operator<  (const State& lhs, const State& rhs);
bool operator== (const State& lhs, const State& rhs);
int  hashCode   (const State& state);

#endif
