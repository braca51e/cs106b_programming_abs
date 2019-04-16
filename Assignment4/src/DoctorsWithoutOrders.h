#ifndef DoctorsWithoutOrders_Included
#define DoctorsWithoutOrders_Included

#include <string>
#include <ostream>
#include "vector.h"
#include "set.h"
#include "map.h"


/**
 * A type representing a doctor and the number of hours they can work.
 */
struct Doctor {
    std::string name; // Their name, for testing purposes.
    int hoursFree;    // How many hours they have free to work.
};

/**
 * A type representing a patient and the number of hours they need to be seen for.
 */
struct Patient {
    std::string name; // Their name, for testing purposes.
    int hoursNeeded;  // How many hours they must be seen for.
};

/**
 * Given a list of doctors and a list of patients, determines whether all the patients can
 * be seen. If so, this function fills in the schedule outparameter with a map from doctors
 * to the set of patients that doctor would see.
 *
 * @param doctors  The list of the doctors available to work.
 * @param patients The list of the patients that need to be seen.
 * @param schedule An outparameter that will be filled in with the schedule, should one exist.
 * @return Whether or not a schedule was found.
 */
bool canAllPatientsBeSeen(const Vector<Doctor>& doctors,
                          const Vector<Patient>& patients,
                          Map<std::string, Set<std::string>>& schedule);


/* * * * * Helper Routines * * * * */

/* These functions allow you to print out our structs using cout. You don't need to use these
 * functions, but they might be helpful during testing.
 */
std::ostream& operator<< (std::ostream& out, const Doctor& doc);
std::ostream& operator<< (std::ostream& out, const Patient& patient);

/* These functions allow you to store Doctors and Patients in Maps, Sets, etc. You may not
 * need to use these functions, but they're included just in case you do.
 */
bool operator<  (const Doctor& lhs, const Doctor& rhs);
bool operator== (const Doctor& lhs, const Doctor& rhs);
int  hashCode(const Doctor& d);

bool operator<  (const Patient& lhs, const Patient& rhs);
bool operator== (const Patient& lhs, const Patient& rhs);
int  hashCode(const Patient& p);

#endif
