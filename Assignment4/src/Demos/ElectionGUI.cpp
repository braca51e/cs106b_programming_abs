#include "ElectionGUI.h"
#include "ElectionParser.h"
#include "GUIUtils.h"
#include "TemporaryComponent.h"
#include "gobjects.h"
#include "ginteractors.h"
#include <unordered_map>
#include <memory>
#include <fstream>
#include <memory>
#include <string>
#include <cmath>
#include <iomanip>
#include <sstream>
#include "strlib.h"
#include "gthread.h"
#include "filelib.h"
using namespace std;

namespace {
    /* Year ranges. */
    const int kLowYear  = 1828;
    const int kHighYear = 2016;
    const int kYearStep = 4;

    /* Struct containing information about colors for states based on whether they
     * (1) didn't vote, (2) did vote, but didn't vote for you, or (3) voted for you.
     */
    struct StateColors {
        string borderColor;
        string fillColor;
    };

    enum StateResult {
        NO_VOTE,
        VOTE_AGAINST,
        VOTE_FOR
    };

    /* Color information. */
    const string kBackgroundColor = "#ADD8E6"; // Light Blue
    const vector<StateColors> kStateColors = {
        { "#87ADB9", "#97BDC9" }, // No vote
        { "#202020", "#566B73" }, // Voted, but not for you
        { "#806030", "#FFCF60" }, // Voted for you
    };

    /* Loading message */
    const string kLoadingMessage   = "Loading...";
    const string kLoadingFont      = "Serif-BOLD-36";
    const string kLoadingFontColor = "White";

    /* Font information. */
    const string kFontColor       = "#FFCF60";
    const string kFont            = "Monospace-BOLD-20";

    /* Descriptive information. */
    const string kHeaderFont  = "Serif-ITALIC-20";
    const string kHeaderColor = "#082567"; // Sapphire

    /* Data file names. */
    const string kStateShapeFile  = "us-borders.txt";
    const string kBaseDir         = "Elections/Visualizer/";

    /* Legend area, obtained via extensive trial and error. :-) */
    const double kLegendX       = 5;
    const double kLegendY       = 325;
    const double kLegendWidth   = 350;
    const double kLegendHeight  = 300;

    const string kLegendFont            = "Serif-BOLD-18";
    const string kLegendFontColor       = kHeaderColor;
    const string kLegendBorderColor     = kHeaderColor;

    /* Summary area, obtained via extensive trial and error. :-) */
    const double kResultX           = 400;
    const double kResultY           = 50;
    const double kResultWidth       = 450;
    const double kResultHeight      = 250;
    const string kResultFont        = "Monospaced-BOLD-16";
    const string kResultFontColor   = kHeaderColor;

    /* Draws a shape with the specified fill and border colors. */
    void drawState(GWindow& window, GCompound* shape,
                   const string& borderColor,
                   const string& fillColor) {
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

            polygon->setColor(borderColor);
            polygon->setFillColor(fillColor);
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
    unordered_map<string, shared_ptr<GCompound>> loadStates(const GRectangle& bounds) {
        ifstream input(kBaseDir + kStateShapeFile);
        if (!input) error("Unable to load state shape file.");

        unordered_map<string, shared_ptr<GCompound>> result;

        for (string stateName, numShapes;
             getline(input, stateName), getline(input, numShapes); ){
            /* Read a GCompound from the file consisting of all the
             * pieces of the state.
             */
            auto compound = make_shared<GCompound>();
            for (int i = 0; i < stringToInteger(numShapes); i++) {
                compound->add(loadSingleShapeFrom(input, bounds));
            }

            result[stateName] = compound;
        }

        return result;
    }

    /* Sums up and returns the popular vote for an election year given the voting states. */
    int popularVote(const Vector<State>& states) {
        int result = 0;
        for (const auto& state: states) {
            result += state.popularVotes;
        }
        return result;
    }

    /* Constructs the legend explaining what the colors mean. */
    shared_ptr<LegendRender> makeLegend() {
        return LegendRender::construct({ "Didn't Participate", "Not Needed to Win", "Needed to Win" },
                                       { kStateColors[NO_VOTE].fillColor,
                                         kStateColors[VOTE_AGAINST].fillColor,
                                         kStateColors[VOTE_FOR].fillColor },
                                       { kLegendX, kLegendY, kLegendWidth, kLegendHeight },
                                       kLegendFontColor,
                                       kLegendFont,
                                       kLegendBorderColor);

    }

    /* Constructs a summary based on the election data. */
    shared_ptr<TextRender> summarize(const ElectionData& data,
                                     const MinInfo& result) {
        ostringstream builder;
        builder << "Election Year:      " << data.year << endl;
        builder << "Popular Votes Cast: " << addCommasTo(popularVote(data.allStates)) << endl;
        builder << "Minimum to Win:     " << addCommasTo(result.popularVotesNeeded) << endl;

        return TextRender::construct(builder.str(),
                                     { kResultX, kResultY, kResultWidth, kResultHeight },
                                     kResultFontColor, kResultFont);
    }
}

struct ElectionGUI::Impl {
    Impl(ElectionGUI* owner) : mOwner(owner) {

    }

    /* Reference back to owner. */
    ElectionGUI* const mOwner;

    /* Slider bar controlling what year we're displaying. */
    Temporary<GSlider> mYearSlider;

    /* Map from state names to state shapes. */
    unordered_map<string, shared_ptr<GCompound>> mStateShapes;

    /* What year this is. This defaults to -1 to force an initial calculation. */
    int mYear = -1;

    /* Map from states to their outcomes. */
    unordered_map<string, StateResult> mVotes;

    /* Legend explaining color choices. */
    shared_ptr<LegendRender> mLegend;

    /* Render explaining outcomes. */
    shared_ptr<TextRender> mSummary;

    /* Recalculates the display given the current settings. */
    void recalculateDisplay();
};

ElectionGUI::ElectionGUI(GWindow& window) : mImpl(new Impl(this)) {
    /* Shapes take a second or two to load; let everyone know that. */
    GThread::runOnQtGuiThread([&] {
        clearDisplay(window, kBackgroundColor);
        drawCenteredText(window, {
                             0, 0,
                             window.getCanvasWidth(),
                             window.getCanvasHeight()
                         },
                         kLoadingMessage, kLoadingFont, kLoadingFontColor);
        window.repaint();
    });

    /* Construct our bounding rectangle. */
    GRectangle bounds = {
        0, 0,
        window.getCanvasWidth(),
        window.getCanvasHeight()
    };

    GThread::runOnQtGuiThread([&] {
        mImpl->mStateShapes = loadStates(bounds);
        mImpl->mLegend = makeLegend();
    });

    /* Set up our slider bar using the date ranges spanned by our data. */
    mImpl->mYearSlider = makeYearSlider(window, kLowYear, kHighYear, kYearStep);
    mImpl->recalculateDisplay();
}

void ElectionGUI::actionPerformed(GObservable* source) {
    if (source == mImpl->mYearSlider) {
        mImpl->recalculateDisplay();
    }
}

void ElectionGUI::repaint(GWindow& window) {
    clearDisplay(window, kBackgroundColor);

    for (const auto& entry: mImpl->mVotes) {
        drawState(window, mImpl->mStateShapes[entry.first].get(),
                  kStateColors[entry.second].borderColor,
                  kStateColors[entry.second].fillColor);
    }

    mImpl->mLegend->draw(window);
    mImpl->mSummary->draw(window);
}

/* TODO: This is a mess. Split this apart into multiple smaller functions. */
void ElectionGUI::Impl::recalculateDisplay() {
    /* See what year this is. If the year hasn't changed, we don't need to do anything. */
    int year = mYearSlider->getValue() * kYearStep + kLowYear;
    if (year == mYear) return;

    /* Pull the data from this year. */
    ElectionData data;
    try {
        data = electionDataFrom(year);
        mYear = year;
    } catch (const ErrorException &) {
        /* No data from this year. */
        return;
    }

    /* Solve the problem. */
    auto solution = minPopularVoteToWin(data.allStates);

    /* Classify each state based on its absence or presence. */
    mVotes.clear();

    /* Copy over all states, initially as not voting. */
    for (const auto& entry: mStateShapes) {
        mVotes[entry.first] = NO_VOTE;
    }

    /* Now, copy over all states that participated in the election, initially as not voting for you. */
    for (const auto& state: data.allStates) {
        mVotes[state.name] = VOTE_AGAINST;
    }

    /* Finally, copy over the ones that voted for us. */
    for (const auto& state: solution.statesUsed) {
        mVotes[state.name] = VOTE_FOR;
    }

    mSummary = summarize(data, solution);
    mOwner->requestRepaint();
}
