#include "MultiwayMerge.h"
#include "Testing/MergeTests.h"
using namespace std;

/* TODO: Refer to MultiwayMerge.h for more information about what this function does,
 * then delete this comment.
 */
Vector<DataPoint> merge(Vector<DataPoint> left, Vector<DataPoint> right){
    Vector<DataPoint> ret;
    //get largest size
    DataPoint p_l;
    DataPoint p_r;
    while (left.size() > 0 && right.size() > 0) {
        p_l = left.pop_front();
        p_r = right.pop_front();

        if(p_l.weight >= p_r.weight){
            ret.push_back(p_r);
            //push back in left
            left.push_front(p_l);
        }
        else{
            ret.push_back(p_l);
            //push back in right
            right.push_front(p_r);
        }
    }
    //left reamins with bigger elements
    if(left.size() > 0){
        ret += left;
    }

    if(right.size() > 0){
        ret += right;
    }

    return ret;
}

Vector<DataPoint> mergeSequences(const Vector<Vector<DataPoint>>& sequences) {
    if(sequences.size() == 1){
            return sequences[0];
        }
    else if(sequences.size() < 1){
        return {};
    }
    else{
        Vector<Vector<DataPoint>> left, right;
        Vector<DataPoint> left_m, right_m;
        //split sequences 2 groups
        left = sequences.subList(0, sequences.size()/2);
        right = sequences.subList(sequences.size()/2, sequences.size()-(sequences.size()/2));
        //Recursively merge each group
        left_m = mergeSequences(left);
        right_m = mergeSequences(right);
        //Merge final sequences
        return merge(left_m, right_m);
    }
}

/* * * * * * Tests Below This Point * * * * * */

ADD_TEST("Works in the example from the handout.") {
    Vector<Vector<int>> sequences = {
        { 3, 8, 10, 11, 13, 16 },
        { 0, 1, 12 },
        { 4, 9, 17, 19 },
        { 7 },
        { 5, 6, 14, 18 },
        { 2, 15 }
    };

    /* The example in the handout uses integers, but we need DataPoints. Tag each integer
     * with an empty string.
     */
    Vector<Vector<DataPoint>> dataPoints;
    for (auto sequence: sequences) {
        Vector<DataPoint> result;
        for (int elem: sequence) {
            result += { "", elem };
        }
        dataPoints += result;
    }

    /* Merge them! */
    auto merged = mergeSequences(dataPoints);

    EXPECT(merged.size() == 20);
    for (int i = 0; i < 20; i++) {
        EXPECT(merged[i].weight == i);
    }
}
