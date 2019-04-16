#include "Demos/ShiftSchedulingGUI.h"
#include "Demos/GUIUtils.h"
#include "ShiftScheduling.h"
#include "random.h"
#include <random>
#include <string>
using namespace std;

namespace {
    /* General graphics constants. */
    const string kBackgroundColor = "#FFFFFF";
    const double kWindowPadding = 10;

    /* Grid properties. */
    const string kHeaderFont      = "SansSerif-14";
    const string kHeaderFontColor = "#989898";      // Spanish Gray
    const double kHeaderHeight    = 50;

    const string kHourFont        = "SansSerif-8";
    const string kHourFontColor   = kHeaderFontColor;
    const double kHourWidth       = 30;

    const string kLineColor       = kHeaderFontColor;

    /* Number of hours to let the person work. */
    const int kStandardHours = 30;

    /* Shift properties. */
    const string kShiftBackgroundColor = "#0093AF"; // Munsell Blue
    const string kShiftBorderColor     = "#004957"; // Components of Munsell Blue, halved
    const string kShiftFontColor       = "#FFFFFF"; // White

    const string kUnchosenShiftBackgroundColor = "#E3DAC9"; // Bone
    const string kUnchosenShiftBorderColor     = "#716D64"; // Components of Bone, halved
    const string kUnchosenShiftFontColor       = "#383632"; // Components of Bone, quartered

    const string kShiftFont            = "SansSerif-14";
    const double kShiftPadding         = 5;

    /* Ranges on shift values. */
    const int kLowWeight  = 0;
    const int kHighWeight = 99 / 8; // Length of the longest shift

    /* Available shifts. */
    const Set<Shift> kStandardShifts = {
        { Day::SUNDAY,  8, 14 },
        { Day::SUNDAY, 12, 18 },

        { Day::MONDAY,  8, 12 },
        { Day::MONDAY, 12, 16 },
        { Day::MONDAY, 16, 20 },
        { Day::MONDAY,  8, 16 },
        { Day::MONDAY, 12, 20 },

        { Day::TUESDAY,  8, 12 },
        { Day::TUESDAY, 12, 16 },
        { Day::TUESDAY, 16, 20 },
        { Day::TUESDAY,  8, 16 },
        { Day::TUESDAY, 12, 20 },

        { Day::WEDNESDAY,  8, 12 },
        { Day::WEDNESDAY, 12, 16 },
        { Day::WEDNESDAY, 16, 20 },
        { Day::WEDNESDAY,  8, 16 },
        { Day::WEDNESDAY, 12, 20 },

        { Day::THURSDAY,  8, 12 },
        { Day::THURSDAY, 12, 16 },
        { Day::THURSDAY, 16, 20 },
        { Day::THURSDAY,  8, 16 },
        { Day::THURSDAY, 12, 20 },

        { Day::FRIDAY,  8, 12 },
        { Day::FRIDAY, 12, 16 },
        { Day::FRIDAY, 16, 20 },
        { Day::FRIDAY,  8, 16 },
        { Day::FRIDAY, 12, 20 },

        { Day::SATURDAY,  8, 14 },
        { Day::SATURDAY, 12, 18 },
    };


    /* Returns a list of all days of the week, in order. */
    const Vector<Day> kAllDays = {
        Day::SUNDAY,
        Day::MONDAY,
        Day::TUESDAY,
        Day::WEDNESDAY,
        Day::THURSDAY,
        Day::FRIDAY,
        Day::SATURDAY
    };

    /* Given a window, returns the drawing area within that window. */
    GRectangle boundsFor(GWindow& window) {
        return expand(0.0, 0.0, window.getCanvasWidth(), window.getCanvasHeight(), -kWindowPadding);
    }

    /* Converts a day into a string. */
    string dayToString(Day day) {
        ostringstream result;
        result << day;
        return result.str();
    }

    /* Draws a single text string, centered, in the given bounds. */
    void drawCenteredText(const string& text,
                          const GRectangle& bounds,
                          const string& font,
                          const string& color,
                          GWindow& window) {
        GText label(text);
        label.setFont(font);
        label.setColor(color);

        double x = bounds.getX() + (bounds.getWidth() - label.getWidth()) / 2.0;

        double fontHeight = label.getFontAscent() + label.getFontDescent();

        /* Center the rectangle, then shift down by the ascent. */
        double y = bounds.getY() + (bounds.getHeight() - fontHeight) / 2.0 + label.getFontAscent();

        window.draw(&label, x, y);
    }

    /* Given the client area of a column, returns a list of GRectangles, each of
     * which corresponds to the bounding box for the hours. The first entry
     * corresponds to time kLowHour, the next to kLowHour + 1, etc.
     *
     * Although each hour has a bounding box, that box is not usually drawn.
     */
    Vector<GRectangle> cellBoundingBoxes(const GRectangle& bounds, int lowHour, int highHour) {
        Vector<GRectangle> result;

        double cellHeight = bounds.getHeight() / (highHour - lowHour + 1);
        for (int hour = lowHour; hour <= highHour; hour++) {
            result.add({
                bounds.getX(), bounds.getY() + cellHeight * (hour - lowHour),
                bounds.getWidth(), cellHeight
            });
        }

        return result;
    }

    /* Draws a single column of the calendar view. The bounds parameter indicates the
     * space that this column is supposed to take up.
     */
    void drawColumnFor(Day day, GWindow& window, const GRectangle& headerSpace,
                       const GRectangle& columnSpace,
                       int lowHour, int highHour) {
        /* Draw the header. */
        window.setColor(kLineColor);
        window.drawRect(headerSpace);
        drawCenteredText(dayToString(day), headerSpace, kHeaderFont, kHeaderFontColor, window);

        /* Draw the outline of the column. */
        window.setColor(kLineColor);
        window.drawRect(columnSpace);

        /* Draw the internal lines within the column. */
        for (auto box: cellBoundingBoxes(columnSpace, lowHour, highHour)) {
            window.drawLine(box.getX(), box.getY() + box.getHeight() / 2,
                            box.getX() + box.getWidth(), box.getY() + box.getHeight() / 2);
        }
    }

    /* Given an hour, returns a human-readable representation of that hour. */
    string hourToString(int hour) {
        hour %= 24;

        if (hour ==  0) return "12AM";
        if (hour < 12)  return to_string(hour) + "AM";
        if (hour == 12) return "12PM";
        return to_string(hour - 12) + "PM";
    }

    /* Draws the headers in front of each of the rows. */
    void drawRowHeaders(GWindow& window, const GRectangle& bounds, int lowHour, int highHour) {
        auto boxes = cellBoundingBoxes(bounds, lowHour, highHour);

        for (int i = 0; i < boxes.size(); i++) {
            int hour = lowHour + i;
            drawCenteredText(hourToString(hour), boxes[i], kHourFont, kHourFontColor, window);
        }
    }

    /* Draws a calendar grid in the indicated space. */
    void drawGrid(GWindow& window,
                  const GRectangle& rowSpace,
                  const GRectangle& columnHeaderSpace,
                  const GRectangle& columnSpace,
                  double columnWidth,
                  int lowHour, int highHour) {
        /* Draw all columns. */
        for (auto day: kAllDays) {
            double x = columnSpace.getX() + static_cast<double>(day) * columnWidth;
            drawColumnFor(day, window, {
                              x, columnHeaderSpace.getY(),
                              columnWidth,
                              columnHeaderSpace.getHeight()
                          }, {
                              x, columnSpace.getY(),
                              columnWidth, columnSpace.getHeight()
                          }, lowHour, highHour);
        }

        /* Draw the row headers. */
        drawRowHeaders(window, rowSpace, lowHour, highHour);
    }

    /* Renders the selected collection of shifts, which are all assumed to be on the same
     * day, within the bounding box assigned to that particular day.
     */
    void drawShiftsForDay(GWindow& window, const GRectangle& bounds,
                          const Set<Shift>& shifts,
                          const Set<Shift>& chosen,
                          int lowHour, int highHour) {
        /* Edge case: If there are no shifts, there's nothing to draw! */
        if (shifts.isEmpty()) return;

        /* The question, now, is how to minimally subdivide this column. This turns out
         * to be a graph coloring problem for a class of graphs called interval graphs
         * (nodes correspond to intervals, with edges corresponding to overlapping
         * intervals)! Graph coloring is, in general, intractible, but fortunately
         * the graph coloring problem for interval graphs is known to be solvable in
         * polynomial-time via a greedy algorithm (sort intervals by start time, and
         * place each interval in the first slot where it fits). This follows from the
         * fact that interval graphs are perfect graphs.
         *
         * We'll use this to assign each interval to a virtual subcolumn. Fortunately,
         * our comparison function for shifts implicitly sorts them by start time,
         * so we can just iterate over the shifts and find the first subcolumn that
         * doesn't conflict with them!
         *
         * We could speed this up by using more clever data structures, but since we're
         * dealing with small numbers of intervals there's no need to do this here.
         */
        Map<Shift, int> subcolumns;       // Key is a shift, index is its subcolumn.
        Map<int,   int> subcolumnBottoms; // Key is a subcolumn, index is the next free
                                          // spot in that subcolumn.

        for (const auto& shift: shifts) {
            /* Try all subcolumns until one is found where it fits. */
            for (int i = 0; ; i++) {
                /* Autoinsert defaults to 0, which will always work for us. */
                if (subcolumnBottoms[i] <= shift.startHour) {
                    subcolumns[shift] = i;
                    subcolumnBottoms[i] = shift.endHour;
                    break;
                }
            }
        }

        /* The number of subcolumns needed is the size of the subcolumnBottoms map, since
         * it's touched only when we needed to probe a particular subcolumn.
         */
        double width = bounds.getWidth() / subcolumnBottoms.size();
        auto boxes = cellBoundingBoxes(bounds, lowHour, highHour);

        /* Draw each shift in its subcolumn. */
        for (const auto& shift: shifts) {

            /* Convert from logical hours to 0-indexed hours. */
            int startIndex = shift.startHour - lowHour;
            int endIndex   = shift.endHour   - lowHour;

            double x = bounds.getX() + width * subcolumns[shift];
            double y = boxes[startIndex].getY() + boxes[startIndex].getHeight() / 2.0;

            /* All boxes have the same height, so we can just see how far apart they are. */
            double height = boxes[endIndex].getY() - boxes[startIndex].getY();

            auto box = expand(x, y, width, height, -kShiftPadding);

            /* Draw the box. */
            window.setColor(chosen.contains(shift)? kShiftBackgroundColor : kUnchosenShiftBackgroundColor);
            window.fillRect(box);
            window.setColor(chosen.contains(shift)? kShiftBorderColor : kUnchosenShiftBorderColor);
            window.drawRect(box);

            /* Draw the value. */
            drawCenteredText(to_string(valueOf(shift)), box, kShiftFont, chosen.contains(shift)? kShiftFontColor : kUnchosenShiftFontColor, window);
        }
    }

    /* Draws the specified set of shifts into the calendar grid. */
    void drawShifts(GWindow& window, const GRectangle& columnSpace, double columnWidth,
                    const Set<Shift>& shifts, const Set<Shift>& chosen,
                    int lowHour, int highHour) {
        /* Partition shifts into days for rendering purposes. */
        Map<Day, Set<Shift>> byDay;
        for (const auto& shift: shifts) {
            byDay[shift.day] += shift;
        }

        /* Render each day separately. */
        for (Day day: byDay) {
            double x = columnSpace.getX() + columnWidth * static_cast<double>(day);
            drawShiftsForDay(window, {
                                 x, columnSpace.getY(), columnWidth, columnSpace.getHeight()
                             }, byDay[day], chosen, lowHour, highHour);
        }
    }

    /* Given a collection of shifts, returns the total value of those shifts. */
    int valueOf(const Set<Shift>& shifts) {
        int result = 0;
        for (const auto& shift: shifts) {
            result += valueOf(shift);
        }
        return result;
    }

    /* Given a collection of shifts, returns the total length of those shifts. */
    int lengthOf(const Set<Shift>& shifts) {
        int result = 0;
        for (const auto& shift: shifts) {
            result += lengthOf(shift);
        }
        return result;
    }
}

struct ShiftSchedulingGUI::Impl {
    Set<Shift> mShifts; // Shifts to display
    Set<Shift> mChosen; // Which have been picked by the user.

    /* Graphical controls. */
    Temporary<GButton> mSolve;
    Temporary<GButton> mRandomize;
    Temporary<GLabel>  mStatus;

    /* Randomizes the weights in the system. */
    void randomizeWeights();

    /* Returns a string describing the solution produced. */
    string solutionDescription() const;
};

ShiftSchedulingGUI::ShiftSchedulingGUI(GWindow& window) {
    mImpl = make_shared<Impl>();
    mImpl->mSolve     = Temporary<GButton>(new GButton("Optimize"), window, "EAST");
    mImpl->mRandomize = Temporary<GButton>(new GButton("Randomize Values"), window, "EAST");
    mImpl->mStatus    = Temporary<GLabel> (new GLabel(" "), window, "SOUTH");

    mImpl->randomizeWeights();
}

string ShiftSchedulingGUI::name() {
    return "Shift Scheduling";
}

void ShiftSchedulingGUI::actionPerformed(GObservable* source) {
    if (source == mImpl->mSolve) {
        mImpl->mSolve->setEnabled(false);
        mImpl->mRandomize->setEnabled(false);
        mImpl->mStatus->setText("Optimizing...");
        mImpl->mChosen = highestValueScheduleFor(mImpl->mShifts, kStandardHours);
        requestRepaint();
        mImpl->mStatus->setText(mImpl->solutionDescription());
        mImpl->mRandomize->setEnabled(true);
        mImpl->mSolve->setEnabled(true);
    } else if (source == mImpl->mRandomize) {
        mImpl->randomizeWeights();
        mImpl->mChosen.clear();
        requestRepaint();
    }
}

void ShiftSchedulingGUI::repaint(GWindow& window) {
    clearDisplay(window, kBackgroundColor);

    /* Determine where the rows and columns go. */
    auto bounds = boundsFor(window);

    /* The column headers are offset from the row start. */
    GRectangle columnHeaderSpace = {
        bounds.getX() + kHourWidth, bounds.getY(),
        bounds.getWidth() - kHourWidth, kHeaderHeight
    };

    /* The row space shifts down by the amount of the column headers, but is
     * otherwise flush against the border.
     */
    GRectangle rowSpace = {
        bounds.getX(), columnHeaderSpace.getY() + columnHeaderSpace.getHeight(),
        kHourWidth, bounds.getHeight() - columnHeaderSpace.getY() - columnHeaderSpace.getHeight()
    };

    /* The column space sandwiched on both sides by headers. */
    GRectangle columnSpace = {
        rowSpace.getX() + rowSpace.getWidth(), rowSpace.getY(),
        bounds.getWidth() - rowSpace.getWidth(), rowSpace.getHeight()
    };

    /* Determine the width of each column. */
    double columnWidth = columnSpace.getWidth() / kAllDays.size();

    /* Find the range of hours spanned by these shifts. If no shifts exist, default to using
     * 0 (midnight) and 24 (midnight).
     */
    int lowHour = 0;
    int highHour = 24;

    if (!mImpl->mShifts.isEmpty()) {
        lowHour = min_element(mImpl->mShifts.begin(), mImpl->mShifts.end(), [](const Shift& lhs, const Shift& rhs) {
            return lhs.startHour < rhs.startHour;
        })->startHour;
        highHour = max_element(mImpl->mShifts.begin(), mImpl->mShifts.end(), [](const Shift& lhs, const Shift& rhs) {
            return lhs.endHour < rhs.endHour;
        })->endHour;
    }

    drawGrid(window, rowSpace, columnHeaderSpace, columnSpace, columnWidth, lowHour, highHour);
    drawShifts(window, columnSpace, columnWidth, mImpl->mShifts, mImpl->mChosen, lowHour, highHour);
}

string ShiftSchedulingGUI::Impl::solutionDescription() const {
    ostringstream builder;
    builder << "Best solution produces " << valueOf(mChosen)
            << " value, using " << lengthOf(mChosen)
            << " of " << kStandardHours << " available hours.";
    return builder.str();
}

void ShiftSchedulingGUI::Impl::randomizeWeights() {
    random_device rd;
    mt19937 generator(rd());
    uniform_int_distribution<int> distribution(kLowWeight, kHighWeight);

    /* Assign weights randomly on a per-hour basis. That is, generate a per-hour value,
     * then weight the shift by that per-hour value.
     */
    mShifts.clear();
    for (Shift shift: kStandardShifts) {
        shift.value = distribution(generator) * (shift.endHour - shift.startHour);
        mShifts += shift;
    }
}
