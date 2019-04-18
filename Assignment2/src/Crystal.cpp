/* File: Crystal.cpp
 *
 * TODO: Delete this comment and replace it with something more insightful about how your
 * program works.
 */
#include "Crystal.h"
#include "Testing/CrystalTests.h"
using namespace std;

void crystallizeAt(Crystal& crystal, int x, int y) {
    /* TODO: Delete the next few lines and implement this function. */
    //(void) crystal;
    //(void) x;
    //(void) y;
    //Add used point
    crystal.used.add(Point(x, y));

    //Added inetresting points
    //up
    if(!crystal.used.contains(Point(x, y-1)))
        crystal.sites.add(Point(x, y-1));
    //down
    if(!crystal.used.contains(Point(x, y+1)))
        crystal.sites.add(Point(x, y+1));
    //left
    if(!crystal.used.contains(Point(x-1, y)))
        crystal.sites.add(Point(x-1, y));
    //right
    if(!crystal.used.contains(Point(x+1, y)))
        crystal.sites.add(Point(x+1, y));
}

void step(Crystal& crystal) {
    /* TODO: Delete the next line and implement this function. */
    //(void) crystal;
    //Do all crystals from previous step
    int numberCris = crystal.sites.size();
    while(numberCris){

        Point tmp = crystal.sites.dequeue();

        tmp.getX();
        //check adjacent to only ONE
        int adjacentTo = 0;
        //up
        if(crystal.used.contains({tmp.getX(), tmp.getY()-1}))
            adjacentTo += 1;
        //down
        if(crystal.used.contains({tmp.getX(), tmp.getY()+1}))
            adjacentTo += 1;
        //left
        if(crystal.used.contains({tmp.getX()-1, tmp.getY()}))
            adjacentTo += 1;
        if(crystal.used.contains({tmp.getX()+1, tmp.getY()}))
            adjacentTo += 1;

        if(adjacentTo == 1)
            crystallizeAt(crystal, tmp.getX(), tmp.getY());

        numberCris -= 1;
    }
}



/* * * * * * Tests Below This Point * * * * * */

ADD_TEST("Crystalizing at (0, 0) works correctly.") {
    Crystal crystal;
    crystallizeAt(crystal, 0, 0);

    EXPECT(crystal.used.size() == 1);
    EXPECT(crystal.used.contains({0, 0}));
    EXPECT(crystal.sites.size() == 4);
}

/* TODO: Delete this comment and add more of your own tests here! */
