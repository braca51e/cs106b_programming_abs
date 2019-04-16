#ifndef SierpinskiBungeeJumpGUI_Included
#define SierpinskiBungeeJumpGUI_Included

#include "ProblemHandler.h"
#include "gtimer.h"

/* Problem handler to do a deep dive into the Sierpinski triangle. */
class SierpinskiBungeeJumpGUI: public ProblemHandler {
public:
    SierpinskiBungeeJumpGUI(GWindow& window);

    void timerFired() override;

    static std::string name();

protected:
    void repaint(GWindow& window) override;

private:
    GTimer mTimer;
    int    mFrame = 0; // Which animation frame we're on

    GPoint mPoints[3];       // Control points of the triangle
    GPoint mCentroid;        // Centroid of rotation
};

REGISTER_HANDLER(SierpinskiBungeeJumpGUI, "Sierpinski Bungee Jump", 2);


#endif
