#include "DoctorsWithoutOrders.h"
#include "Testing/DoctorsWithoutOrdersTests.h"
using namespace std;

/* TODO: Refer to DoctorsWithoutOrders.h for more information about this function.
 * Then, delete this comment.
 */
//First make all possible combinations for patients
Vector<Patient> subGroupOfPatients(Vector<Patient>& patients, Vector<Patient>& chosen, Set<Vector<Patient>>& subsets){
    //base case
    if(patients.isEmpty()){
        cout << "chosen: " << chosen << endl;
        subsets.add(chosen);
        return chosen;
    }
    //Include this element
    Vector<Patient> tmp_patients = patients;
    Patient tmp_pat = tmp_patients.front();
    tmp_patients.pop_front();
    chosen.push_back(tmp_pat);
    subGroupOfPatients(tmp_patients, chosen, subsets);
    //Exclude this element
    chosen.pop_back();
    subGroupOfPatients(tmp_patients, chosen, subsets);

    return chosen;
}
bool canAllPatientsBeSeenHelp( Vector<Doctor>& doctors,
                           Vector<Patient>& patients,
                           Map<string, Set<string>>& schedule) {
    /* TODO: Delete the next few lines and implement this function. */
    //base cases
    //there is no doctors available
    if(doctors.size() == 0 && patients.size() > 0){
        return false;
    }
    else if(doctors.size() >= 0 && patients.size() == 0){
        return true;
    }
    else{
        //exlpore
        //choose  a doctor
        Vector<Doctor> tmp_doctors_y = doctors;
        Vector<Doctor> tmp_doctors_n = doctors;
        Vector<Patient> tmp_patients_y = patients;
        Vector<Patient> tmp_patients_n = patients;
        Doctor tmp_doc;
        //chose
        tmp_doc = tmp_doctors_y.front();

        //YES try
        //try to assign doctors to patients
        for(auto j=0; j < tmp_patients_y.size(); ++j){
            Patient tmp_pat = tmp_patients_y.front();
            if(tmp_doc.hoursFree >= tmp_pat.hoursNeeded){
                tmp_doc.hoursFree -= tmp_pat.hoursNeeded;
                //unchoose patients
                tmp_patients_y.pop_front();
            }
        }
        //recover doctor
        tmp_doctors_y.pop_front();
        bool ret = false;
        ret = canAllPatientsBeSeenHelp(tmp_doctors_y, tmp_patients_y, schedule);
        cout << "ret yes: " << ret << endl;
        //NO
        ret = canAllPatientsBeSeenHelp(tmp_doctors_y, tmp_patients_y, schedule);
        tmp_doc = tmp_doctors_n.front();
        tmp_doctors_n.pop_front();
        if((!tmp_doctors_n.isEmpty()) && tmp_patients_y.size() != patients.size()){
            tmp_doctors_n.push_back(tmp_doc);
        }
        if(!ret){
            ret = canAllPatientsBeSeenHelp(tmp_doctors_n, tmp_patients_n, schedule);
        }
        cout << "ret no: " << ret << endl;
        return ret;
    }
}

bool canAllPatientsBeSeen(const Vector<Doctor>& doctors,
                          const Vector<Patient>& patients,
                          Map<string, Set<string>>& schedule) {
    /* TODO: Delete the next few lines and implement this function. */
    //(void) doctors;
    //(void) patients;
    //(void) schedule;
    Vector<Doctor> lcl_doctors;
    Vector<Patient> lcl_patients;
    Map<string, Set<string>> chosen;
    for(auto i=0; i < doctors.size(); ++i){
        lcl_doctors.push_back(doctors[i]);
    }
    for(auto i=0; i < patients.size(); ++i){
        lcl_patients.push_back(patients[i]);
    }
    bool ret;
    ret = canAllPatientsBeSeenHelp(lcl_doctors, lcl_patients, schedule);
    cout << "allpatientsSeen: " << ret << endl;
    //get patient subsets
    //Vector<Patient> chosen_sub_p;
    //Set<Vector<Patient>> subsets_p;
    //subGroupOfPatients(lcl_patients, chosen_sub_p, subsets_p);
    //for (auto it=subsets_p.begin(); it != subsets_p.end(); ++it) {
    //    cout << "sub_patients: " << *it << endl;
    //}

    return ret;
}

/* * * * * * Test Cases Below This Point * * * * * */

/*ADD_TEST("Dev test.") {
    Map<string, Set<string>> schedule;

    Vector<Doctor> doctors = {
        { "Dr. Thomas", 9 },
        { "Dr. Taussig", 3 },
        { "Dr. Sacks", 8 },
    };
    Vector<Patient> patients = {
        { "Lacks 1", 3 },
        { "Lacks 2", 10 },
    };

    EXPECT(canAllPatientsBeSeen(doctors, patients, schedule));
}*/

/*ADD_TEST("A patient requiring more hours than available prevents solutions.") {
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
}*/

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

/*ADD_TEST("Dev test.") {
    Map<string, Set<string>> schedule;

    Vector<Doctor> doctors = {
        { "Dr. Sacks", 8 },
    };
    Vector<Patient> patients = {
        { "Lacks 2", 10 },
    };

    EXPECT(canAllPatientsBeSeen(doctors, patients, schedule));
}*/
