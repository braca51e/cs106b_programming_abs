#include "EmptyProblemHandler.h"
#include "gobjects.h"
#include <string>
using namespace std;

namespace {
    /* Graphics Constants. */
    const string kFont = "Serif-BOLDITALIC-36";
    const string kBackgroundColor = "#400040";
    const string kFontColor       = "#800080";

    const string kToDisplay = "Choose which demo to run.";
}

void EmptyProblemHandler::repaint(GWindow& window) {
    /* Clear the window. */
    window.setColor(kBackgroundColor);
    window.fillRect(0, 0, window.getCanvasWidth(), window.getCanvasHeight());

    /* Draw some text! */
    GText label(kToDisplay);
    label.setFont(kFont);
    label.setColor(kFontColor);

    double x = (window.getCanvasWidth()  - label.getWidth())      / 2.0;
    double y = (window.getCanvasHeight() + label.getFontAscent()) / 2.0;

    window.draw(&label, x, y);
    window.repaint();
}

void EmptyProblemHandler::actionPerformed(GObservable *) {
    // Do nothing
}
