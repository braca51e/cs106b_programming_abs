#include "DisasterPlanning.h"
#include "Testing/DisasterPlanningTests.h"
using namespace std;

/* TODO: Refer to DisasterPlanning.h for more information about this function.
 * Then, delete this comment.
 */
Vector<string> getCities(Vector<string> citiesCovered){
    Vector<string> tmp;

    for (auto it = citiesCovered.begin(); it != citiesCovered.end(); ++it) {
        if(!tmp.contains(*it)){
            tmp.add(*it);
        }
    }

    return tmp;
}

bool allCitiesCovered(const Vector<string> cities, Vector<string>& citiesCovered){
    bool ret = false;
    Vector<string> tmp_unique = getCities(citiesCovered);

    ret = tmp_unique.size() == cities.size();

    if(ret)
        cout << "found!!!!!!!" << endl;

    return ret;
}

bool canBeMadeDisasterReadyHelp(Map<string, Set<string>>& roadNetwork,
                                int numCities,
                                Set<string>& supplyLocations,
                                const Vector<string> cities,
                                Vector<string>& citiesCovered) {
    /* TODO: Delete the next few lines and implement this function. */
    if(numCities < 0){
        error("Cities is less than zero!");
    }
    if(numCities == 0){
        bool ret = allCitiesCovered(cities, citiesCovered);
        if(ret){
            cout << "Cities remaining: " << numCities << endl;
            cout << "supplyLocations: " << supplyLocations << endl;
        }
        return ret;
    }
    if(allCitiesCovered(cities, citiesCovered)){
        cout << "Cities remaining: " << numCities << endl;
        cout << "supplyLocations: " << supplyLocations << endl;
        return true;
    }
    else {

        bool ret = false;
        Map<string, Set<string>> tmp_roadNetwork = roadNetwork;
        string tmp_selected = tmp_roadNetwork.front();
        Set<string> tmp_neighbors = tmp_roadNetwork.get(tmp_selected);
        Vector<string> tmp_citiesCovered_1 = citiesCovered;
        Set<string> tmp_supplyLocations_1 = supplyLocations;
        Vector<string> tmp_citiesCovered_2 = citiesCovered;
        Set<string> tmp_supplyLocations_2 = supplyLocations;
        //choose city
        while(citiesCovered.contains(tmp_selected)){
            //Necessary to keep moving forward
            tmp_roadNetwork.remove(tmp_selected);
            tmp_selected = tmp_roadNetwork.front();
            tmp_neighbors = tmp_roadNetwork.get(tmp_selected);
        }
        //Supply this city
        tmp_supplyLocations_1.add(tmp_selected);
        tmp_citiesCovered_1.add(tmp_selected);
        for(auto it = tmp_neighbors.begin(); it != tmp_neighbors.end(); ++it){
            if(!tmp_citiesCovered_1.contains(*it)){
                tmp_citiesCovered_1.add(*it);
            }
        }
        //Find if this works
        ret = canBeMadeDisasterReadyHelp(roadNetwork, numCities-1, tmp_supplyLocations_1, cities, tmp_citiesCovered_1);
        //if not pick one neighbor
        if(!ret){
            //Supply one neighbor until done
            for(auto it = tmp_neighbors.begin(); it != tmp_neighbors.end(); ++it){
                //Supply this neighbor
                tmp_supplyLocations_2.add(*it);
                tmp_citiesCovered_2.add(*it);
                Set<string> tmp_nei_neighbors = roadNetwork.get(*it);
                //Add if not covered
                for(auto it2 = tmp_nei_neighbors.begin(); it2 != tmp_nei_neighbors.end(); ++it2){
                    if(!tmp_citiesCovered_2.contains(*it2)){
                        tmp_citiesCovered_2.add(*it2);
                    }
                }
                //Add coverage of neighbor neighrbors
                ret = canBeMadeDisasterReadyHelp(roadNetwork, numCities-1, tmp_supplyLocations_2, cities, tmp_citiesCovered_2);
                if(!ret){
                    //Remove coverage and supply
                    tmp_citiesCovered_2 = citiesCovered;
                    tmp_supplyLocations_2 = supplyLocations;
                }
                else {
                    break;
                }
            }

        }

        return ret;
        }
}
bool canBeMadeDisasterReady(const Map<string, Set<string>>& roadNetwork,
                            int numCities,
                            Set<string>& supplyLocations) {
    /* TODO: Delete the next few lines and implement this function. */
    cout << "Initial Cities: " << numCities << endl;
    Map<string, Set<string>> roadNetwork_tmp = roadNetwork;
    //Map<string, Set<string>> chosen_net;
    bool ret = false;
    Vector<string> coveredCities;
    Vector<string> cities = roadNetwork.keys();

    ret = canBeMadeDisasterReadyHelp(roadNetwork_tmp, numCities, supplyLocations, cities, coveredCities);
    return ret;
}

/* * * * * * * Test Cases Below This Point * * * * * */

namespace { // Private helper functions
    /* Given a road network that may or may not be symmetric, adds in reverse links to make it
     * symmetric. This makes it a lot easier to define road networks!
     */
    Map<string, Set<string>> makeSymmetric(const Map<string, Set<string>>& source) {
        Map<string, Set<string>> result = source;

        for (const string& from: source) {
            for (const string& to: source[from]) {
                result[from] += to;
                result[to] += from;
            }
        }

        return result;
    }
}

ADD_TEST("Ethane example from the handout passes.") {
    auto map = makeSymmetric({
        { "C2", {"H2", "H4", "H6"} },
        { "C1", {"H1", "H3", "H5", "C2"} },
    });

    Set<string> locations;
    EXPECT(canBeMadeDisasterReady(map, 2, locations));
    EXPECT(locations.size() == 2);
    EXPECT(locations.contains("C1"));
    EXPECT(locations.contains("C2"));
}
