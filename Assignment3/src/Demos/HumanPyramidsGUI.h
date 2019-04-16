#ifndef HumanPyramidsGUI_Included
#define HumanPyramidsGUI_Included

#include "ProblemHandler.h"
#include "TemporaryComponent.h"
#include "ginteractors.h"

/* Problem handler that displays a variety of human pyramids and reports the weight on
 * each person.
 */
class HumanPyramidsGUI: public ProblemHandler {
public:
    HumanPyramidsGUI(GWindow& window);

    void actionPerformed(GObservable* source) override;
    void mouseMoved(double x, double y) override;

    static std::string name();

protected:
    void repaint(GWindow& window) override;

private:
    /* Controls for the height of the pyramid. */
    Temporary<GSlider> mHeightSlider;
    int mHeight;

    /* Whether the pyramid itself is dirty. This happens if the height
     * changes.
     */
    bool mPyramidDirty = true;

    /* Which person, if any, is selected. We use the values of -1 as a
     * sentinel if no one is selected.
     */
    int mSelectedRow = -1, mSelectedCol = -1;

    /* Whether the selected person is dirty. This happens if the mouse moves around
     * in a way that changes things.
     */
    bool mPersonDirty = false;

    /* Last bounding rectangle for the selected person. */
    GRectangle mPersonInfo;

    /* Bounding rectangle for positioning everything. */
    GRectangle mBounds;

    /* Sets which person is selected. */
    void select(int row, int col);
    void clearSelection();

    /* Redraws selected graphics items. */
    void redrawPyramid(GWindow& window);
    void redrawPersonInfo(GWindow& window);
};

REGISTER_HANDLER(HumanPyramidsGUI, "Human Pyramids", 3);

#endif
