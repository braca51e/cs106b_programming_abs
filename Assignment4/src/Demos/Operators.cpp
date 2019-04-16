#include "DoctorsWithoutOrders.h"
#include "DisasterPlanning.h"
#include "WinningThePresidency.h"
#include "GUIUtils.h"
#include "hashcode.h"
#include <sstream>
#include <climits>
using namespace std;

/* Prints a human-readable version of a Doctor struct. */
ostream& operator<< (ostream& out, const Doctor& doc) {
    ostringstream result;
    result << "[ Doctor " << doc.name << " ("
           << pluralize(doc.hoursFree, "hour") << " free) ]";
    return out << result.str();
}

/* Prints a human-readable version of a Patient struct. */
ostream& operator<< (ostream& out, const Patient& patient) {
    ostringstream result;
    result << "[ Patient " << patient.name << " ("
           << pluralize(patient.hoursNeeded, "hour") << " needed) ]";
    return out << result.str();
}

/* Prints a human-readable version of a state. */
ostream& operator<< (ostream& out, const State& state) {
    ostringstream result;
    result << "[ " << state.name << ": "
           << pluralize(state.popularVotes, "popular vote") << ", "
           << pluralize(state.electoralVotes, "electoral vote")
           << " ]";
    return out << result.str();
}

/* Prints a human-readable version of a MinInfo structure. */
ostream& operator<< (ostream& out, const MinInfo& info) {
    ostringstream result;

    if (info.popularVotesNeeded == kNotPossible) {
        result << "[ impossible ]";
    } else {
        result << "[ " << pluralize(info.popularVotesNeeded, " vote") << " needed, carrying " << info.statesUsed << " ]";
    }

    return out << result.str();
}

/* * * * * Comparison Functions * * * * */

bool operator<  (const Doctor& lhs, const Doctor& rhs) {
    if (lhs.name != rhs.name) return lhs.name < rhs.name;
    return lhs.hoursFree < rhs.hoursFree;
}
bool operator== (const Doctor& lhs, const Doctor& rhs) {
    return lhs.name == rhs.name &&
           lhs.hoursFree == rhs.hoursFree;
}
int hashCode(const Doctor& d) {
    return hashCode(d.name, d.hoursFree);
}

bool operator<  (const Patient& lhs, const Patient& rhs) {
    if (lhs.name != rhs.name) return lhs.name < rhs.name;
    return lhs.hoursNeeded < rhs.hoursNeeded;
}
bool operator== (const Patient& lhs, const Patient& rhs) {
    return lhs.name == rhs.name &&
           lhs.hoursNeeded == rhs.hoursNeeded;
}
int hashCode(const Patient& p) {
    return hashCode(p.name, p.hoursNeeded);
}

bool operator<  (const State& lhs, const State& rhs) {
    if (lhs.name != rhs.name) return lhs.name < rhs.name;
    if (lhs.electoralVotes != rhs.electoralVotes) return lhs.electoralVotes < rhs.electoralVotes;
    return lhs.popularVotes < rhs.popularVotes;
}
bool operator== (const State& lhs, const State& rhs) {
    return lhs.name == rhs.name &&
           lhs.electoralVotes == rhs.electoralVotes &&
           lhs.popularVotes == rhs.popularVotes;

}
int hashCode(const State& state) {
    return hashCode(state.name, state.electoralVotes, state.popularVotes);
}
