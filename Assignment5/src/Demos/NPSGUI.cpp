#include "NPSGUI.h"
#include "GUIUtils.h"
#include "Utilities/CSV.h"
#include "MultiwayMerge.h"
#include "TopK.h"
#include "LowerBound.h"
#include <fstream>
#include <memory>
#include <string>
#include <cmath>
#include "strlib.h"
#include "gthread.h"
using namespace std;

namespace {
    /* Font information. */
    const string kFontColor       = "#FFCF60";
    const string kFont            = "Monospace-BOLD-20";

    /* Descriptive information. */
    const string kHeaderText =
        "The US National Park Service oversees a number of National Parks, National Monuments, "
        "National Recreation Areas, and other public lands. These areas draw hundreds of millions "
        "of visitors per year. This visualization shows the number of visitors to each of the "
        "parks, showing the introduction of new parks and shifts in their popularity.\n"
        "All data taken from the National Park Service.";
    const string kHeaderFont  = "Serif-ITALIC-20";
    const string kHeaderColor = "#082567"; // Sapphire
    const double kHeaderWidth   = 600;
    const double kHeaderHeight  = 150;
    const double kHeaderPadding = 5;

    /* Data file names. */
    const string kStateShapeFile  = "us-borders.txt";
    const string kBaseDirectory   = "NPS-Data/";
    const string kNPSDataFile     = "NPS-Data/nps.csv";

    /* Color scheme. */
    const string kBackgroundColor  = "#ADD8E6"; // Light Blue
    const string kStateFillColor   = "#74C365"; // Mantis
    const string kStateBorderColor = "#006600"; // Pakistan Green
    const string kParkFillColor    = "#FFFF99"; // Canary Yellow
    const string kParkBorderColor  = "#FFD700"; // Golden

    /* Information for drawing the "top parks" box. */
    const int kNumTopParks = 5;

    const double kTopParksX       = 5;
    const double kTopParksY       = 275;
    const double kTopParksWidth   = 350;
    const double kTopParksHeight  = 300;
    const double kTopParksPadding = 10;
    const string kTopParksFont = "Serif-BOLD-18";

    const string kTopParksFontColor       = kHeaderColor;
    const string kTopParksBorderColor     = kHeaderColor;
    const string kTopParksBackgroundColor = "#FDF5E6"; // Old Lace

    /* Draws a shape with the specified fill and border colors. */
    void drawState(GWindow& window, GCompound* shape) {
        /* GCompound is not itself a GFillable, but its components
         * are all GPolygons.
         */
        for (int i = 0; i < shape->getElementCount(); i++) {
            /* The dynamic_cast here isn't necessary, but if we decide
             * to do things like add in the number of electoral votes
             * or state names we may need to do some switching logic
             * here. The safety doesn't hurt, either.
             */
            GPolygon* polygon = dynamic_cast<GPolygon*>(shape->getElement(i));
            if (!polygon) error("Expected a GPolygon underlying type.");

            polygon->setColor(kStateBorderColor);
            polygon->setFillColor(kStateFillColor);
        }
        window.draw(shape);
    }

    /* Determined by a lot of trial and error. */
    const double kMinX = -0.70;
    const double kMaxX = +0.43;
    const double kMinY = -0.45;
    const double kMaxY = +0.37;

    /* Geographic center of the US. */
    const double kCenterLatitude  =   44.966666667;
    const double kCenterLongitude = -103.766666667;

    double mollweideToX(double mollX, const GRectangle& bounds) {
        return bounds.getX() + (mollX - kMinX) / (kMaxX - kMinX) * bounds.getWidth();
    }

    double mollweideToY(double mollY, const GRectangle& bounds) {
        /* Remember to flip the coordinate system! */
        return bounds.getY() + bounds.getHeight() - (mollY - kMinY) / (kMaxY - kMinY) * bounds.getHeight();
    }

    /**
     * Loads a single polygon from the input stream. The polygon format
     * is
     *
     *    Line 1:    Ignore this line
     *    Lines 2-n: Longitude Latitude
     *    Line n+1:  Blank
     */
    GPolygon* loadSingleShapeFrom(istream& input, const GRectangle& bounds) {
        GPolygon* result = new GPolygon();
        result->setFilled(true);

        /* Skip the first line, since it's in the middle of the region. */
        {
            string throwaway;
            getline(input, throwaway);
        }

        /* Parse the remaining lines until we get to a blank line. */
        for (string line; getline(input, line) && line != ""; ) {
            auto pieces = stringSplit(line, " ");
            auto coordinates = mollweideProjectionOf(stringToReal(pieces[1]),
                                                     stringToReal(pieces[0]),
                                                     kCenterLongitude, kCenterLatitude);
            double x = mollweideToX(get<0>(coordinates), bounds);
            double y = mollweideToY(get<1>(coordinates), bounds);

            result->addVertex(x, y);
        }
        return result;
    }

    /* Draws all the states in the indicated window. */
    void drawStatesIn(GWindow& window, const GRectangle& bounds) {
        ifstream input(kStateShapeFile);
        if (!input) error("Unable to load state shape file.");

        for (string stateName, numShapes;
             getline(input, stateName), getline(input, numShapes); ){
            /* Read a GCompound from the file consisting of all the
             * pieces of the state.
             */
            auto compound = make_shared<GCompound>();
            for (int i = 0; i < stringToInteger(numShapes); i++) {
                compound->add(loadSingleShapeFrom(input, bounds));
            }
            drawState(window, compound.get());
        }
    }

    /* Given CSV data containing all the parks, builds GOvals for each of those parks. */
    unordered_map<string, GOval *> createGraphicsFor(const CSV& parks,
                                                     GWindow& window,
                                                     const GRectangle& bounds) {
        unordered_map<string, GOval *> result;

        for (int row = 0; row < parks.numRows(); row++) {
            string code = parks[row]["Code"];

            double latitude  = stringToReal(parks[row]["Latitude"]);
            double longitude = stringToReal(parks[row]["Longitude"]);

            auto coordinates = mollweideProjectionOf(latitude, longitude,
                                                     kCenterLongitude, kCenterLatitude);

            double x = mollweideToX(get<0>(coordinates), bounds);
            double y = mollweideToY(get<1>(coordinates), bounds);

            auto* oval = new GOval(x, y, 0, 0);
            oval->setFilled(true);
            oval->setColor(kParkBorderColor);
            oval->setFillColor(kParkFillColor);
            oval->setVisible(false);
            window.add(oval);
            result[code] = oval;
        }

        return result;
    }

    /* Given the name of a park, returns a sorted timeline of visit counts for that
     * park.
     */
    Vector<DataPoint> timelineFor(const string& parkName) {
        string filename = kBaseDirectory + parkName + ".csv";
        ifstream input(filename);
        if (!input) error("Cannot open data file " + filename);\

        CSV parkData = CSV::parse(input);

        Vector<DataPoint> result;
        for (int row = 0; row < parkData.numRows(); row++) {
            result.add({
                parkName + " " + parkData[row][1],
                stringToInteger(parkData[row][0])
            });
        }

        return result;
    }

    /* Given parks data, returns a list of sorted timelines of visit counts, where each
     * timeline contains data points of the form { "PPPP Visitors", YYYY }.
     */
    Vector<Vector<DataPoint>> timelinesFor(const CSV& parks) {
        Vector<Vector<DataPoint>> result;

        for (int row = 0; row < parks.numRows(); row++) {
            result += timelineFor(parks[row]["Code"]);
        }

        return result;
    }

    /* Returns the radius to use when displaying a park, given its visitor count.
     * This is based on the idea that
     *
     *    1. The area of a circle is proportional to the square of its radius, and
     *    2. A very, very popular park (for some definition of "very large") should
     *       have a modest radius (for some definition of "modest.")
     */
    double radiusFor(double numVisitors) {
        static const double kLotsOfVisitors = 10000000;
        static const double kLargeRadius    = 50;

        return kLargeRadius * sqrt(numVisitors / kLotsOfVisitors);
    }

    /* Updates the display to reflect a certain park's visitor count. */
    void updatePark(int visitors, GOval* oval) {
        /* Determine where we're centered. */
        double x = oval->getX() + oval->getWidth()  / 2.0;
        double y = oval->getY() + oval->getHeight() / 2.0;

        /* Update its radius. */
        double radius = radiusFor(visitors);
        oval->setBounds(x - radius, y - radius, 2 * radius, 2 * radius);
        oval->setVisible(true);
    }

    /* Compares two doubles for equality, under the assumption that they're both really
     * integers.
     */
    bool areEqual(double x, double y) {
        const double kEpsilon = 0.1;
        return fabs(x - y) <= kEpsilon;
    }

    /* Draws the header text. */
    shared_ptr<TextRender> createHeader(GWindow& window) {
        GRectangle bounds = {
            window.getCanvasWidth() - kHeaderPadding - kHeaderWidth,
            kHeaderPadding,
            kHeaderWidth  - 2 * kHeaderPadding,
            kHeaderHeight - 2 * kHeaderPadding
        };

        return TextRender::construct(kHeaderText, bounds, kHeaderColor, kHeaderFont);
    }

    /* Given the raw CSV data, extracts the name of each park. */
    unordered_map<string, string> parkNamesFrom(const CSV& parks) {
        unordered_map<string, string> result;

        for (int row = 0; row < parks.numRows(); row++) {
            result[parks[row]["Code"]] = parks[row]["Name"];
        }

        return result;
    }

    /* Given a number, inserts commas into that number. */
    string addCommasTo(int value) {
        string asStr = to_string(value);

        string result;

        /* Scan backwards over the digits, adding commas as appropriate. */
        for (size_t i = asStr.size(); i > 0; --i) {
            /* Prepend the character. */
            result = asStr[i - 1] + result;

            /* If we've seen a multiple of three characters and this isn't the
             * very first character, add a comma.
             */
            if ((asStr.size() - i) % 3 == 2 && i != 1) {
                result = "," + result;
            }
        }

        return result;
    }
}

NPSGUI::NPSGUI(GWindow& window) {
    clearDisplay(window, kBackgroundColor);

    /* Construct the render for the header up at the top of the window. */
    GThread::runOnQtGuiThread([&] {
        auto header = createHeader(window);

        /* Draw the header so that people have something to look at while everything loads. */
        header->draw(window);
        window.repaint();
    });

    /* Construct our bounding rectangle. */
    GRectangle bounds = {
        0, 0,
        window.getCanvasWidth(),
        window.getCanvasHeight()
    };

    GThread::runOnQtGuiThread([&] {
        drawStatesIn(window, bounds);
    });

    /* Load park data. */
    ifstream input(kNPSDataFile);
    if (!input) error("Can't open parks data file " + kNPSDataFile);

    CSV parks = CSV::parse(input);
    mGraphics  = createGraphicsFor(parks, window, bounds);
    mTimeline  = mergeSequences(timelinesFor(parks));
    mParkNames = parkNamesFrom(parks);

    /* Set up our slider bar using the date ranges spanned by our data. */
    mYearSlider = makeYearSlider(window, mTimeline.front().weight, mTimeline.back().weight);
    recalculateDisplay();
}

void NPSGUI::changeOccurredIn(GObservable* source) {
    if (source == mYearSlider) {
        recalculateDisplay();
    }
}

void NPSGUI::repaint(GWindow& window) {
    /* The background and the GOvals are handled automatically. We just need to draw
     * the "Top Parks" box.
     */

    /* First, clear the old box, if there is one. */
    window.setColor(kBackgroundColor);
    window.fillRect(mLastTopParksBounds);

    /* Now, draw the new one. */

    /* First, see how big this is going to be. */
    auto text = TextRender::construct(mTopParks,
                                      { kTopParksX + kTopParksPadding,
                                        kTopParksY + kTopParksPadding,
                                        kTopParksWidth  - 2 * kTopParksPadding,
                                        kTopParksHeight - 2 * kTopParksPadding },
                                      kTopParksFontColor, kTopParksFont);
    GRectangle bounds = text->bounds();

    /* Pad around that area and draw the box. */
    mLastTopParksBounds = {
        bounds.getX() - kTopParksPadding,
        bounds.getY() - kTopParksPadding,
        bounds.getWidth()  + 2 * kTopParksPadding,
        bounds.getHeight() + 2 * kTopParksPadding
    };

    window.setColor(kTopParksBackgroundColor);
    window.fillRect(mLastTopParksBounds);
    window.setColor(kTopParksBorderColor);
    window.drawRect(mLastTopParksBounds);

    /* Now, go render the text. */
    text->draw(window);
}

void NPSGUI::recalculateDisplay() {
    /* See what year this is. If the year hasn't changed, we don't need to do anything. */
    int year = mYearSlider->getValue();
    if (year == mYear) return;
    mYear = year;

    /* Reset all park ovals, in case we have no data for them. */
    for (auto& entry: mGraphics) {
        entry.second->setVisible(false);
    }

    /* Find the starting data point for this year. */
    int i = lowerBoundOf(mTimeline, mYear);

    /* If this is out of range, or if this is too early, we're done. */
    if (i == mTimeline.size()) {
        cout << "All done!" << endl;
        return;
    } else if (mTimeline[i].weight != mYear) {
        cout << "Year found was " << mTimeline[i].weight << ", but needed " << mYear << endl;
        return;
    }

    /* Assemble a list of all parks for which we have data, so that we can show which ones
     * are popular.
     */
    Vector<tuple<string, int>> allUsed;

    /* Keep moving forward until we're past the current year. */
    while (i < mTimeline.size() && areEqual(mYear, mTimeline[i].weight)) {
        /* Parse out the park and the visitor count. */
        string park  = mTimeline[i].name.substr(0, 4);
        int visitors = stringToInteger(mTimeline[i].name.substr(5));

        updatePark(visitors, mGraphics.at(park));
        allUsed += make_tuple(park, visitors);

        i++;
    }

    /* Find the most popular parks. */
    stringstream stream;
    for (size_t i = 0; i < allUsed.size(); i++) {
        string name = mParkNames.at(get<0>(allUsed[i])) + " (" + addCommasTo(get<1>(allUsed[i])) + ")";
        int weight  = get<1>(allUsed[i]);
        stream << DataPoint{ name, weight };
    }

    auto popular = topK(stream, kNumTopParks);
    ostringstream builder;

    builder << "Most Popular Parks, " << mYear << ":" << endl;

    for (int i = 0; i < popular.size(); i++) {
        builder << (i + 1) << ": " << popular[i].name << endl;
    }

    mTopParks = builder.str();
    requestRepaint();
}
