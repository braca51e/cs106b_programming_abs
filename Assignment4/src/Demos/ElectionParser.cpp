#include "strlib.h"
#include <string>
#include <fstream>
#include "vector.h"
#include "filelib.h"
#include "ElectionParser.h"
using namespace std;

/* Everything in here is private to the file. */
namespace {
    /* How many lines to read if the data are simplified first. */
    const int kSimplifiedLines = 10;


    /**
     * Given a line from a file of the form
     *    State Name, Postal Code, Electoral Votes, Votes Cast, Population
     * returns a State object constructed from the relevant information.
     *
     * @param line The line of the file to parse.
     * @return A State object constructed from it.
     * @throws ErrorException If the line is malformed.
     */
    State parseState(const string& line) {
        /* Enumerated type corresponding to the different components of
         * a line from the input file.
         */
        enum LineComponents {
            StateName,
            PostalCode,
            ElectoralVotes,
            PopularVotes,
            NumComponents
        };

        auto pieces = stringSplit(line, ",");
        if (pieces.size() != LineComponents::NumComponents) {
            error("Wrong number of entries on this line.");
        }

        return {
            pieces[LineComponents::StateName],
            stringToInteger(pieces[LineComponents::ElectoralVotes]),
            stringToInteger(pieces[LineComponents::PopularVotes])
        };
    }

    /**
     * Parses the first line of an elections file, confirming that it matches the
     * expected year.
     *
     * @param line    The line from the file.
     * @param year    The year in question.
     * @throws ErrorException If the line isn't valid.
     */
    void parseFirstLine(const string& line, int year) {
        /* Type representing the pieces of the header that we'd need. */
        enum HeaderLineComponents {
            Year,
            MinPopularVote,
            MinSimplePopularVote,
            NumComponents
        };

        /* Split the line into its components. It should have the same number as
         * a regular line.
         */
        auto pieces = stringSplit(line, ",");
        if (pieces.size() != HeaderLineComponents::NumComponents) {
            error("Wrong number of entries on this line.");
        }

        /* Make sure this is for the right year, just as a sanity check. */
        if (stringToInteger(pieces[HeaderLineComponents::Year]) != year) {
            error("Election data file doesn't match the expected year.");
        }
    }
}

/**
 * Given an election year, returns a Vector<State> containing all the
 * election data from that year.
 *
 * @param year The year in question. Valid years are 1828 - 2016.
 * @return A list of all the states from the election in that year.
 * @throws ErrorException If an error occurs or the year is invalid.
 */
ElectionData electionDataFrom(int year) {
    ifstream input("Elections/Data/" + integerToString(year) + ".csv");
    if (!input) {
        error("Can't open a data file for that year.");
    }

    ElectionData result;
    result.year = year;

    /* Confirm that the year matches and extract the reference solution. */
    string firstLine;
    getline(input, firstLine);
    parseFirstLine(firstLine, year);

    /* Parse each line of the file. */
    for (string line; getline(input, line); ) {
        result.allStates += parseState(line);
    }

    /* If when all things are said and done we couldn't read
     * anything, we've encountered an error.
     */
    if (result.allStates.isEmpty()) error("File is missing or empty?");

    return result;
}
