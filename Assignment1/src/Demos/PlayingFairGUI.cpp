#include "PlayingFairGUI.h"
#include "PlayingFair.h"
#include "TemporaryComponent.h"
#include "ginteractors.h"
using namespace std;

namespace {
    /* Amount to nudge borders of the world coordinates to avoid degenerate cases. */
    const double kEpsilon = 0.01;

    /* Line thickness. */
    const double kLineWidth = 2;

    /* Margin from the border of the window to the actual crystal. */
    const double kWindowPadding = 20;

    /* Slider controls. */
    const int kMinOrder = 0;
    const int kMaxOrder = 13;
    const int kDefaultOrder = 0;

    /* Foreground and background colors. */
    const Vector<string> kColors = {
        "#000000",
        "#FFFF00"
    };

    const string kLabelFont = "Serif-16";
    const string kLabelColor = "#00FFFF";

    /* Type: Geometry
     *
     * Information about how big each cell should be and how to display things properly in the
     * window.
     */
    struct Geometry {
        double minX, minY;   // Lowest X and Y coordinate of any point
        double baseX, baseY; // Offset from the upper-left corner of the screen to the content
        double scale;        // Scaling factor converting from global to window coordinates
    };

    /* Given a crystal, computes the geometry used to draw it. */
    Geometry geometryFor(const Vector<GPoint>& points, GWindow& window) {
        /* Compute the bounding box. */
        double minX =  numeric_limits<double>::infinity();
        double minY =  numeric_limits<double>::infinity();

        double maxX = -numeric_limits<double>::infinity();
        double maxY = -numeric_limits<double>::infinity();

        /* Expand our box around the spots used in the fractal. */
        for (const auto& loc: points) {
            minX = min(minX, loc.getX());
            minY = min(minY, loc.getY());

            maxX = max(maxX, loc.getX());
            maxY = max(maxY, loc.getY());
        }

        /* Nudge everything ever so slightly to avoid degenerate cases. */
        minX -= kEpsilon;
        minY -= kEpsilon;

        maxX += kEpsilon;
        maxY += kEpsilon;

        /* Get the width and height of the world. */
        double width  = maxX - minX;
        double height = maxY - minY;

        /* Computing the scaling factors needed to scale to the window width and window height. */
        double scaleX = (window.getWidth()  - 2 * kWindowPadding) / width;
        double scaleY = (window.getHeight() - 2 * kWindowPadding) / height;
        double scale = min(scaleX, scaleY);

        /* Compute base x and y based on the scale. */
        double baseX = kWindowPadding + (window.getWidth()  - 2 * kWindowPadding - width  * scale) / 2.0;
        double baseY = kWindowPadding + (window.getHeight() - 2 * kWindowPadding - height * scale) / 2.0;

        return {
            minX, minY,
            baseX, baseY,
            scale
        };
    }

    Vector<GPoint> fractalOfOrder(int n) {
        Vector<GPoint> result;

        GPoint loc(0, 0);

        /* If the order is odd, everything is, by default, rotated 180 degrees and we need to correct for
         * this. If the order is even, it's rotated 30 degrees counterclockwise.
         */
        double theta;
        if (n == 0) theta = 0;
        else if (n % 2 == 0) theta = 11 * M_PI / 6;
        else theta = M_PI;

        result += loc;

        for (char ch: aSequenceOfOrder(n)) {
            if (ch == 'A') {
                loc = {
                    loc.getX() + cos(theta),
                    loc.getY() + sin(theta)
                };

                result += loc;
                theta += M_PI / 3.0;
            } else if (ch == 'B') {
                theta += M_PI;
            } else {
                error("Unknown character in A-sequence: '" + to_string(ch) + "'.");
            }
        }

        return result;
    }

    void drawFractal(int order, GWindow& window) {
        auto points = fractalOfOrder(order);

        window.setColor(kColors[0]);
        window.fillRect(0, 0, window.getCanvasWidth(), window.getCanvasHeight());

        auto g = geometryFor(points, window);

        /* For efficiency's sake, only make one GLine. */
        GLine line;
        line.setColor(kColors[1]);
        line.setLineWidth(kLineWidth);

        for (int i = 0; i + 1 < points.size(); i++) {
            double x0 = g.baseX + (points[i].getX() - g.minX) * g.scale;
            double y0 = g.baseY + (points[i].getY() - g.minY) * g.scale;

            double x1 = g.baseX + (points[i+1].getX() - g.minX) * g.scale;
            double y1 = g.baseY + (points[i+1].getY() - g.minY) * g.scale;

            line.setStartPoint(x0, y0);
            line.setEndPoint(x1, y1);
            window.draw(line);
        }

        /* Display the order, just for simplicity's sake. */
        GText label("Derived from order-" + to_string(order) + " A-sequence.");
        label.setFont(kLabelFont);
        label.setColor(kLabelColor);
        label.setLocation(kWindowPadding, window.getCanvasHeight() - label.getFontDescent() - kWindowPadding);
        window.draw(label);
    }
}

struct PlayingFairGUI::Impl {
    Temporary<GSlider> order;

    int lastOrder = kDefaultOrder;
};

PlayingFairGUI::PlayingFairGUI(GWindow& window) {
    mImpl = make_shared<Impl>();

    auto* slider = new GSlider(kMinOrder, kMaxOrder, kDefaultOrder);
    slider->setPaintLabels(true);
    slider->setPaintTicks(true);
    slider->setSnapToTicks(true);
    slider->setMajorTickSpacing(1);
    slider->setBounds(0, 0, window.getCanvasWidth(), slider->getHeight());

    mImpl->order = Temporary<GSlider>(slider, window, "SOUTH");
}

void PlayingFairGUI::actionPerformed(GObservable* source) {
    if (source == mImpl->order) {
        int currOrder = mImpl->order->getValue();
        if (currOrder != mImpl->lastOrder) {
            mImpl->lastOrder = currOrder;
            requestRepaint();
        }
    }
}

void PlayingFairGUI::repaint(GWindow& window) {
    drawFractal(mImpl->lastOrder, window);
}
