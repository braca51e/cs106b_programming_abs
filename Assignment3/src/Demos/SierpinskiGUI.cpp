#include "SierpinskiGUI.h"
#include "GUIUtils.h"
#include "GVector.h"
#include "Sierpinski.h"
using namespace std;

namespace {
    const string kBackgroundColor = "#FFFFFF";

    /* Slider options. */
    const int kLowOrder     = 0;
    const int kHighOrder    = 8;
    const int kDefaultOrder = 0;

    /* Header information. */
    const string kHeader =
        "Use the slider to change the order of the "
        "triangle. Drag the red ovals to reposition the corners.";
    const string kHeaderFont      = "Serif-ITALIC-16";
    const string kHeaderFontColor = "#36454F"; // Charcoal
    const double kHeaderPadding   = 10;

    /* Control points. */
    const string kControlPointFillColor    = "#C41E3A"; // Cardinal
    const string kControlPointBorderColor  = "#620F1C"; // Half cardinal
    const double kControlPointRadius  = 10;
    const double kControlPointPadding = 25;

    /* Returns the center point of a GOval. */
    GPoint centerOf(const GOval* oval) {
        return {
            oval->getX() + oval->getWidth()  / 2.0,
            oval->getY() + oval->getHeight() / 2.0
        };
    }
}

SierpinskiGUI::SierpinskiGUI(GWindow& window) {
    mOrderSlider = Temporary<GSlider>(new GSlider(kLowOrder, kHighOrder, kDefaultOrder),
                                      window,
                                      "SOUTH");
    mOrderSlider->setPaintLabels(true);
    mOrderSlider->setPaintTicks(true);
    mOrderSlider->setSnapToTicks(true);
    mOrderSlider->setMajorTickSpacing(1);
    mOrderSlider->setBounds(0, 0, window.getCanvasWidth(), mOrderSlider->getHeight());

    mOrder = kDefaultOrder;

    /* Set up the control points. First, see how much space we'll need to draw the header. */
    auto headerArea = measureWrappedText(kHeader,
                                         kHeaderPadding, kHeaderPadding,
                                         window.getCanvasWidth() - 2 * kHeaderPadding,
                                         kHeaderFontColor, kHeaderFont, window);
    mWorkspace = expand(0, headerArea.getY() + headerArea.getHeight(),
                        window.getCanvasWidth(),
                        window.getCanvasHeight() - headerArea.getY() - headerArea.getHeight(),
                        -kControlPointPadding);

    /* Set the control points to the window corners. */
    GPoint corners[3];
    corners[0] = { mWorkspace.getX(),                             mWorkspace.getY() + mWorkspace.getHeight() };
    corners[1] = { mWorkspace.getX() + mWorkspace.getWidth(),     mWorkspace.getY() + mWorkspace.getHeight() };
    corners[2] = { mWorkspace.getX() + mWorkspace.getWidth() / 2, mWorkspace.getY()                      };

    for (size_t i = 0; i < 3; i++) {
        mCornerPoints[i] = new GOval(corners[i].getX() - kControlPointRadius,
                                     corners[i].getY() - kControlPointRadius,
                                     2 * kControlPointRadius,
                                     2 * kControlPointRadius);
        mCornerPoints[i]->setFilled(true);
        mCornerPoints[i]->setColor(kControlPointBorderColor);
        mCornerPoints[i]->setFillColor(kControlPointFillColor);
        window.add(mCornerPoints[i]);
    }
}

string SierpinskiGUI::name() {
    return "Sierpinski Triangle";
}

void SierpinskiGUI::actionPerformed(GObservable* source) {
    if (source == mOrderSlider) {
        /* See if the value changed. If so, redraw things. */
        int order = mOrderSlider->getValue();
        if (order != mOrder) {
            mOrder = order;
            requestRepaint();
        }
    }
}

void SierpinskiGUI::repaint(GWindow& window) {
    /* We only need to draw the header text once. */
    clearDisplay(window, kBackgroundColor);
    drawWrappedText(kHeader,
                    kHeaderPadding, kHeaderPadding,
                    window.getCanvasWidth() - 2 * kHeaderPadding,
                    kHeaderFontColor, kHeaderFont, window);

    /* Redraw the triangle. */
    drawSierpinskiTriangle(window,
                           centerOf(mCornerPoints[0]),
                           centerOf(mCornerPoints[1]),
                           centerOf(mCornerPoints[2]),
                           mOrder);
}

void SierpinskiGUI::mousePressed(double x, double y) {
    /* See if we hit anything. */
    GPoint mouse = clampToWorkspace(x, y);
    for (auto* oval: mCornerPoints) {
        if (magnitudeOf(centerOf(oval) - mouse) <= kControlPointRadius) {
            mSelected = oval;
            mLastMouse = mouse;
            return;
        }
    }

    /* Oops - didn't hit anything. */
    mSelected = nullptr;
}

void SierpinskiGUI::mouseDragged(double x, double y) {
    /* If nothing is selected, there's nothing to do. */
    if (mSelected == nullptr) return;

    /* Shift the selected oval over by the delta between our current and last
     * point.
     */
    GPoint mouse = clampToWorkspace(x, y);
    auto delta = mouse - mLastMouse;

    mSelected->move(delta.x, delta.y);
    requestRepaint();

    mLastMouse = mouse;
}

GPoint SierpinskiGUI::clampToWorkspace(double x, double y) const {
    x = max(min(x, mWorkspace.getX() + mWorkspace.getWidth()),  mWorkspace.getX());
    y = max(min(y, mWorkspace.getY() + mWorkspace.getHeight()), mWorkspace.getY());

    return { x, y };
}
