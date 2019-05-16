#include "DoctorsWithoutOrders.h"
#include "Testing/DoctorsWithoutOrdersTests.h"
using namespace std;

/* TODO: Refer to DoctorsWithoutOrders.h for more information about this function.
 * Then, delete this comment.
 */
//First make all possible combinations for patients
bool canAllPatientsBeSeenHelp( Vector<Doctor>& doctors,
                               Vector<Patient>& patients,
                               Vector<Patient>& pats_chosen,
                               Map<string, Set<string>>& schedule) {
    /* TODO: Delete the next few lines and implement this function. */
    //base cases
    //there is no doctors available
    if(patients.isEmpty()){
        return doctors.size() > 0 ? true : false;
    }
    else{

        //explore all doctor combinations
        bool ret = false;
        for(auto i = 0; i < patients.size(); ++i){
            //choose
            Vector<Doctor> tmp_doctors;
            Patient tmp_patient = patients[i];
            Map<string, Set<string>> tmp_schedule = schedule;
            Set<string> tmp_set;
            Doctor tmp_doc;
            patients.remove(i);
            pats_chosen.push_front(tmp_patient);
            //Make sure this patient is seen
            bool seen = false;
            for(auto i = 0 ; i < doctors.size(); ++i)
            {
                tmp_doc = doctors[i];
                if(doctors[i].hoursFree >= tmp_patient.hoursNeeded && !seen){
                    tmp_doc.hoursFree -= tmp_patient.hoursNeeded;
                    tmp_set = tmp_schedule.get(tmp_doc.name);
                    tmp_set.add(tmp_patient.name);
                    tmp_schedule.add(tmp_doc.name, tmp_set);
                    tmp_doctors.add(tmp_doc);
                    seen = true;
                }
                //Already seen but doctor still available
                else if(seen) {
                    tmp_doctors.add(tmp_doc);
                }
            }
            //backtrack
            ret = canAllPatientsBeSeenHelp(tmp_doctors,
                                     patients,
                                     pats_chosen,
                                     tmp_schedule);
            //unchoose
            pats_chosen.pop_front();
            patients.insert(i, tmp_patient);
            //cout << "schedule: " << schedule << endl;
            if(ret){
                schedule = tmp_schedule;
                break;
              }
        }
        return ret;
    }
}

bool canAllPatientsBeSeen(const Vector<Doctor>& doctors,
                          const Vector<Patient>& patients,
                          Map<string, Set<string>>& schedule) {
    /* TODO: Try all possible doctor combinations! */
    Vector<Doctor> lcl_doctors;
    Vector<Patient> pats_chosen;
    Vector<Patient> lcl_patients;
    Map<string, Set<string>> chosen;
    for(auto i=0; i < doctors.size(); ++i){
        lcl_doctors.push_back(doctors[i]);
        if(!schedule.keys().contains(doctors[i].name)){
            Set<string> tmp_set;
            schedule.add(doctors[i].name, tmp_set);
        }
    }
    for(auto i=0; i < patients.size(); ++i){
        lcl_patients.push_back(patients[i]);
    }

    return canAllPatientsBeSeenHelp(lcl_doctors, lcl_patients, pats_chosen, schedule);
}

/* * * * * * Test Cases Below This Point * * * * * */

ADD_TEST("Dev test.") {
    Map<string, Set<string>> schedule;

    Vector<Doctor> doctors = {
        { "Dr. Thomas", 2 },
        { "Dr. Taussig", 3 },
    };
    Vector<Patient> patients = {
        { "Lacks 1", 3 },

    };

    EXPECT(canAllPatientsBeSeen(doctors, patients, schedule));
}

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

ADD_TEST("Example from handout.") {
    Map<string, Set<string>> schedule;

    Vector<Doctor> doctors = {
        { "Dr. Thomas", 10 },
        { "Dr. Taussig", 8 },
        { "Dr. Sacks", 8 },
        { "Dr. Ofri", 8 },
    };
    Vector<Patient> patients = {
        { "Lacks", 2 },
        { "Gage", 3 },
        { "Molaison", 4 },
        { "Writebol", 3 },
        { "St. Martin", 1 },
        { "Washkansky", 6 },
        { "Sandoval", 8 },
        { "Giese", 6 },
    };

    EXPECT(canAllPatientsBeSeen(doctors, patients, schedule));
}
ADD_TEST("Example from handout.") {
    Map<string, Set<string>> schedule;

    Vector<Doctor> doctors = {
        { "Dr. Sacks", 8 },
        { "Dr. Thomas", 10 },
        { "Dr. Ofri", 8 },
        { "Dr. Taussig", 8 },
    };
    Vector<Patient> patients = {
        { "Gage", 3 },
        { "Molaison", 4 },
        { "Writebol", 3 },
        { "Lacks", 2 },
        { "Washkansky", 6 },
        { "St. Martin", 1 },
        { "Sandoval", 8 },
        { "Giese", 6 },
    };

    EXPECT(canAllPatientsBeSeen(doctors, patients, schedule));
}

ADD_TEST("Dev test.") {
    Map<string, Set<string>> schedule;

    Vector<Doctor> doctors = {
        { "Dr. Sacks", 8 },
        { "Dr. Taussig", 6 },
    };
    Vector<Patient> patients = {
        { "Lacks", 6 },
        { "Giese", 8 },
    };

    EXPECT(canAllPatientsBeSeen(doctors, patients, schedule));
}
