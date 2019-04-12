#ifndef OnlyConnect_Included
#define OnlyConnect_Included

#include <string>
#include "strlib.h"
#include <cctype>
/**
 * Given a string, converts that string into an Only Connect puzzle by deleting all
 * non-consonants and making all characters upper-case.
 *
 * @param phrase The phrase to transform.
 * @return That string, rendered as an Only Connect puzzle.
 */
std::string onlyConnectize(std::string phrase);
std::string onlyConnectizeHelp(std::string& phrase, std::vector<std::string>& selected);

#endif
