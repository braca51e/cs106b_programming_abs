#include "HumanPyramids.h"
#include "Testing/HumanPyramidTests.h"
using namespace std;

struct element{
    int row;
    int col;
};

//int weightOnBackOfMemo(int row, int col, map<element, int>& table);

/* TODO: Refer to HumanPyramids.h for more information about what this function should do.
 * Then, delete this comment.
 */
bool inBounds(int row, int col){
    if(row >= col && row >= 0 && col >= 0)
        return true;
    else {
        return false;
    }
}

/*int weightOnBackOf(int row, int col) {
    map<element, int> table;
    return weightOnBackOfMemo(row, col, table);
}*/
//No memoization solution
int weightOnBackOf(int row, int col) {

    if(!inBounds(row,col)){
        error("Out of bounds!");
    }
    //base case
    if(row == 0 && col == 0)
        return 0;
    else{
        //first check for side lines
        if(col == 0)
            return 80+(weightOnBackOf(row-1, col))/2;
        else if(row == col)
            return 80+(weightOnBackOf(row-1, col-1))/2;
        //solve middle points
        else
            return 80+(weightOnBackOf(row-1, col-1))/2 + 80+(weightOnBackOf(row-1, col))/2;
    }
}

/*int weightOnBackOfMemo(int row, int col, map<element, int>& table){
    if(!inBounds(row,col)){
        error("Out of bounds!");
    }
    //base case
    if(row == 0 && col == 0 )
        return 0;
    else if(table.find({row, col}) != table.end())
        return table[{row,col}];
    else{
        //first check for side lines
        int weight = 0;
        if(col == 0)
            weight = 80+(weightOnBackOf(row-1, col))/2;
        else if(row == col)
            weight = 80+(weightOnBackOf(row-1, col-1))/2;
        //solve middle points
        else
            weight = 80+(weightOnBackOf(row-1, col-1))/2 + 80+(weightOnBackOf(row-1, col))/2;

        table.insert(pair<element, int>({row,col}, weight));
        return weight;
    }
}*/

/* * * * * * Test Cases * * * * * */

ADD_TEST("Check Person E from the handout.") {
    /* Person E is located at row 2, column 1. */
    EXPECT(weightOnBackOf(2, 1) == 240);
}

/* TODO: Add your own tests here. You know the drill - look for edge cases, think about
 * very small and very large cases, etc.
 */
