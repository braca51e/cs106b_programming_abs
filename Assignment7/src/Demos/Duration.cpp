#include "Duration.h"
#include "error.h"
#include <vector>
#include <utility>
#include <sstream>
using namespace std;

Duration::Duration(int seconds) : seconds(seconds) {

}

int Duration::length() const {
    return seconds;
}

/* Time units. Keys are suffixes, values are multiples of the base time unit.
 *
 * This list must be stored in descending length order.
 */
static const vector<pair<char, int>> kTimeUnits = {
    { 'y', 60 * 60 * 24 * 365 }, // Years
    { 'w', 60 * 60 * 24 * 7   }, // Weeks
    { 'd', 60 * 60 * 24       }, // Days
    { 'h', 60 * 60            }, // Hours
    { 'm', 60                 }, // Minutes
    { 's', 1                  }  // Seconds
};

Duration operator + (const Duration& lhs, const Duration& rhs) {
    return Duration(lhs.length() + rhs.length());
}
Duration operator - (const Duration& lhs, const Duration& rhs) {
    return Duration(lhs.length() - rhs.length());
}
bool operator == (const Duration& lhs, const Duration& rhs) {
    return lhs.length() == rhs.length();
}
bool operator != (const Duration& lhs, const Duration& rhs) {
    return !(lhs == rhs);
}
bool operator <  (const Duration& lhs, const Duration& rhs) {
    return lhs.length() < rhs.length();
}
bool operator >  (const Duration& lhs, const Duration& rhs) {
    return rhs < lhs;
}
bool operator <= (const Duration& lhs, const Duration& rhs) {
    return !(lhs > rhs);
}
bool operator >= (const Duration& lhs, const Duration& rhs) {
    return !(lhs < rhs);
}
Duration& Duration::operator += (const Duration& rhs) {
    seconds += rhs.length();
    return *this;
}
Duration& Duration::operator -= (const Duration& rhs) {
    seconds -= rhs.length();
    return *this;
}

/* Reads a duration from a stream. Although the normal convention to follow when a stream
 * read fails is to set some bits in the stream type, we've opted to throw an exception
 * to more clearly signal that something very seriously wrong has happened.
 */
istream& operator>> (istream& input, Duration& time) {
    istream::sentry sentry(input);
    if (sentry) {
        size_t unitIndex = 0;

        int total = 0;
        for (int amount; input >> amount; ) {
            /* The next character should be a unit of some sort. */
            char unit;
            if (!input.get(unit)) error("Missing unit on duration " + to_string(amount));
            unit = tolower(unit);

            /* Scan forward until we find this unit in the unit table. */
            while (true) {
                if (unitIndex >= kTimeUnits.size()) error("Unknown or misplaced time unit: " + to_string(unit));

                if (kTimeUnits[unitIndex].first == unit) break;
                unitIndex++;
            }

            /* Scale this amount by that unit. */
            total += amount * kTimeUnits[unitIndex].second;
            unitIndex++;
        }

        /* If we couldn't even make a single pass through the loop, we know something bad happened. */
        if (unitIndex == 0) error("Malformed duration.");

        time = Duration(total);
    }

    return input;
}

ostream& operator<< (ostream& out, const Duration& time) {
    ostringstream result;

    long seconds = time.length();

    /* Edge case: We may have a total of 0s, in which case we need to explicitly output 0. */
    if (seconds == 0) {
        return out << "0s";
    }

    /* Edge case: handle negative durations. */
    if (seconds < 0) {
        result << '-';
        seconds *= -1;
    }

    /* Decompose the time into constituent units using a greedy algorithm. */
    for (const auto& timeUnit: kTimeUnits) {
        if (seconds >= timeUnit.second) {
            result << (seconds / timeUnit.second) << timeUnit.first;
            seconds %= timeUnit.second;
        }
    }

    return out << result.str();
}
