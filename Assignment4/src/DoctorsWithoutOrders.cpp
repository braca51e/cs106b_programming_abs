#include "DoctorsWithoutOrders.h"
#include "Testing/DoctorsWithoutOrdersTests.h"
using namespace std;

/* TODO: Refer to DoctorsWithoutOrders.h for more information about this function.
 * Then, delete this comment.
 */
bool canAllPatientsBeSeen(const Vector<Doctor>& doctors,
                          const Vector<Patient>& patients,
                          Map<string, Set<string>>& schedule) {
    /* TODO: Delete the next few lines and implement this function. */
    (void) doctors;
    (void) patients;
    (void) schedule;
    return false;
}





/* * * * * * Test Cases Below This Point * * * * * */

ADD_TEST("A patient requiring more hours than available prevents solutions.") {
    Map<string, Set<string>> schedule;

    Vector<Doctor> doctors = {
        { "Dr. Zhivago", 8 },
        { "Dr. Strange", 8 },
        { "Dr. Horrible", 8 },
    };
    Vector<Patient> patients = {
        { "You Poor Soul", 9 },  // Needs more time than any one doctor can provide
    };

    EXPECT(!canAllPatientsBeSeen(doctors, patients, schedule));
}
