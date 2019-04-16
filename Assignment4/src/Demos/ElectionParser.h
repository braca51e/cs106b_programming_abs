#ifndef ElectionParser_Included
#define ElectionParser_Included

#include "vector.h"
#include "WinningThePresidency.h"

/**
 * Type representing raw data about an election, as pulled from disk. This includes
 * all the states used, the year in question, and a cached version of the optimal
 * number of votes needed.
 */
struct ElectionData {
    Vector<State> allStates;  // All the states that voted
    int year;                 // What year it is
};

/**
 * Given an election year, returns a Vector<State> containing all the
 * election data from that year.
 *
 * @param year The year in question. Valid years are multiples of four between 1828 - 2016.
 * @return A list of all the states from the election in that year.
 * @throws ErrorException If an error occurs or the year is invalid.
 */
ElectionData electionDataFrom(int year);

#endif
