/* File: FleschKincaid.cpp
 *
 * TODO: Edit these comments to describe anything interesting or noteworthy in your implementation.
 */
#include "FleschKincaid.h"
#include "Testing/FleschKincaidTests.h"
#include <sstream> // For the stringstream type used in testing
using namespace std;

DocumentInfo statisticsFor(istream& source) {
    /* TODO: The next few lines just exist to make sure you don't get compiler warning messages
     * when this function isn't implemented. Delete these lines, then implement this function.
     */
    (void) source;
    return { 0, 0, 0 };
}






/* * * * * * Test Cases * * * * * */

/* This is a helper function you can use to directly run your statisticsFor function on a string,
 * rather than on the contents of a file. It uses the C++ stringstream type, which makes a string
 * look like a stream the same way that a file can look like a stream.
 */
DocumentInfo statisticsFor(string text) {
    istringstream stream(text);
    return statisticsFor(stream);
}

ADD_TEST("Counts words and sentences from the handout quote correctly.") {
    /* This is the example from the handout. */
    DocumentInfo info = statisticsFor("Mr. Ford's first day as President was August 9, 1974.");

    EXPECT(info.numWords == 9);
    EXPECT(info.numSentences == 2);
}

ADD_TEST("Counts sentences in simple cases.") {
    /* We just count sentence markers without context, so these appear to be three sentences. */
    EXPECT(statisticsFor(".!?").numSentences == 3);
    EXPECT(statisticsFor("Hmmmm...").numSentences == 3);

    EXPECT(statisticsFor("Thank you! Next (next)").numSentences == 1);
}

ADD_TEST("Counts words in simple cases.") {
    EXPECT(statisticsFor("Welcome to CS106B!").numWords == 3);
    EXPECT(statisticsFor("(We hope you have fun!)").numWords == 5);

    /* Tokens here will be <Tokenization> <:> <it> <'> <s> <hard> <!>, which
     * contains four tokens starting with a letter.
     */
    EXPECT(statisticsFor("Tokenization: it's hard!").numWords == 4);
}

ADD_TEST("Counts syllables in simple cases.") {
    // I -> 1, hEArt -> 1, qUOkkAs -> 2
    EXPECT(statisticsFor("i heart quokkas").numSyllables == 4);

    // hEllO -> 2, wOrld -> 1
    EXPECT(statisticsFor("Hello, world!").numSyllables == 3);
}

/* TODO: You will need to add your own tests into this suite of test cases. Think about the sorts
 * of inputs we tested here, and, importantly, what sorts of inputs we *didn't* test here. Some
 * general rules of testing:
 *
 *    1. Try extreme cases. What are some very large cases to check? What are some very small cases?
 *
 *    2. Be diverse. There are a lot of possible inputs out there. Make sure you have tests that account
 *       for cases that aren't just variations of one another.
 *
 *    3. Be sneaky. Don't just try standard inputs. Try weird ones that you wouldn't expect anyone to
 *       actually enter, but which are still perfectly legal.
 *
 * Happy testing!
 */
