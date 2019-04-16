#ifndef Duration_Included
#define Duration_Included

#include <ostream>
#include <istream>

/**
 * A type representing a duration of time, measured in whole seconds.
 *
 * We use the Duration type to store durations rather than other types because
 * it provides nice conversions between pure seconds and, say, hours, days,
 * weeks, and years.
 */
class Duration {
public:
    /* Constructs a new Duration object from the specified number of seconds. */
    Duration(int seconds = 0);

    /* Returns the amount of time represented by this Duration object, in seconds. */
    int length() const;

    /* Add or subtract durations. */
    Duration& operator += (const Duration& other);
    Duration& operator -= (const Duration& other);

private:
    int seconds;
};

/* Stream operators, used for printing or reading Durations. */
std::ostream& operator<< (std::ostream& out, const Duration& duration);
std::istream& operator>> (std::istream& in,  Duration& duration);

/* Comparison and arithmetical operators on durations. Use these operators to add, subtract,
 * or compare times.
 */
Duration operator + (const Duration& lhs, const Duration& rhs);
Duration operator - (const Duration& lhs, const Duration& rhs);

bool operator == (const Duration& lhs, const Duration& rhs);
bool operator != (const Duration& lhs, const Duration& rhs);
bool operator <  (const Duration& lhs, const Duration& rhs);
bool operator >  (const Duration& lhs, const Duration& rhs);
bool operator <= (const Duration& lhs, const Duration& rhs);
bool operator >= (const Duration& lhs, const Duration& rhs);

#endif
