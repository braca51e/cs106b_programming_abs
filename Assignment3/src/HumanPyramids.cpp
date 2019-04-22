#include "HumanPyramids.h"
#include "Testing/HumanPyramidTests.h"
using namespace std;

int weightOnBackOfMemo(int row, int col, map<Human, int>& table);

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

int weightOnBackOf(int row, int col) {
    map<Human, int> table;
    return weightOnBackOfMemo(row, col, table);
}
//No memoization solution
/*int weightOnBackOf(int row, int col) {

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
}*/

int weightOnBackOfMemo(int row, int col, map<Human, int>& table){

    if(!inBounds(row,col)){
        error("Out of bounds!");
    }
    //base case
    Human humanTmp(row, col);
    if(row == 0 && col == 0 )
        return 0;
    else if(table.find(humanTmp) != table.end()){
        cout << "found: " << humanTmp.getRow() << " " << humanTmp.getCol() << endl;
        cout << "found in table: " << table[humanTmp] << endl;
        return table[humanTmp];
    }
    else{
        //first check for side lines
        int weight = 0;
        if(col == 0)
            return 80+(weightOnBackOfMemo(row-1, col, table))/2;
        else if(row == col)
            return 80+(weightOnBackOfMemo(row-1, col-1, table))/2;
        //solve middle points
        else{
            cout << "Inserting: " << humanTmp.getRow() << " " << humanTmp.getCol() << " " << weight << endl;
            cout << "table: " << table.size() << endl;
            //table[humanTmp] = 80+(weightOnBackOfMemo(row-1, col-1, table))/2 + 80+(weightOnBackOfMemo(row-1, col, table))/2;;
            return 80+(weightOnBackOfMemo(row-1, col-1, table))/2 + 80+(weightOnBackOfMemo(row-1, col, table))/2;
        }
            //cout << "inserted in table: " << table[humanTmp] << endl;
        return weight;
    }
}

/* * * * * * Test Cases * * * * * */

ADD_TEST("Check Person E from the handout.") {
    /* Person E is located at row 2, column 1. */
    EXPECT(weightOnBackOf(2, 1) == 240);
}

/* TODO: Add your own tests here. You know the drill - look for edge cases, think about
 * very small and very large cases, etc.
 */
