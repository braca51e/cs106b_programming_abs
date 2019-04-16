#ifndef NPSGUI_Included
#define NPSGUI_Included

#include "DataPoint.h"
#include "ProblemHandler.h"
#include "TemporaryComponent.h"
#include "gobjects.h"
#include "ginteractors.h"
#include <unordered_map>
#include <memory>

/* Problem handler that lets the user see trends in attendance at US National Parks,
 * Monuments, Historical Sites, etc over time.
 */
class NPSGUI: public ProblemHandler {
public:
    NPSGUI(GWindow& window);

    void changeOccurredIn(GObservable* source) override;

protected:
    void repaint(GWindow& window) override;

private:
    /* Slider bar controlling what year we're displaying. */
    Temporary<GSlider> mYearSlider;

    /* Map from parks to their graphical representations. */
    std::unordered_map<std::string, GOval *> mGraphics;

    /* Map from parks to names. */
    std::unordered_map<std::string, std::string> mParkNames;

    /* Timeline of events to display. Names represent park/attendance
     * pairs. Values represent years.
     */
    Vector<DataPoint> mTimeline;

    /* Current year. */
    int mYear = -1;

    /* String to render for the "top parks" box. */
    std::string mTopParks;

    /* Last bounding rectangle for the "top parks" box. */
    GRectangle mLastTopParksBounds = { 0, 0, 0, 0 };

    /* Updates the display based on the current slider year. */
    void recalculateDisplay();
};

REGISTER_HANDLER(NPSGUI, "National Parks", 6);

#endif
