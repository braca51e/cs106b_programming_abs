#ifndef FleschKincaid_Included
#define FleschKincaid_Included

#include <string>
#include <istream>

/**
 * Type: DocumentInfo
 *
 * Information about the number of words, sentences, and syllables
 * in a document.
 */
struct DocumentInfo {
    int numSentences;
    int numWords;
    int numSyllables;
};

/**
 * Reads the specified file, computes statistics about it, and returns
 * those statistics.
 *
 * @param source A stream containing the file contents.
 * @return A DocumentInfo struct containing information about that file.
 */
DocumentInfo statisticsFor(std::istream& source);

#endif
