#include "DoctorGUI.h"
#include "DoctorsWithoutOrdersParser.h"
#include "TemporaryComponent.h"
#include "GUIUtils.h"
#include "hashmap.h"
#include "hashset.h"
#include <fstream>
#include <memory>
#include <string>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <vector>
#include "filelib.h"
#include "strlib.h"
#include "gthread.h"
using namespace std;

namespace {
    /* File constants. */
    const string kProblemSuffix = ".dwo";
    const string kBasePath = "DoctorsWithoutOrders/";

    const string kBackgroundColor = "White";

    /* Text box information. */
    const double kBoxWidth       = 250;
    const double kBoxHeight      = 45;

    /* Global top/bottom padding. */
    const double kVerticalPadding = kBoxHeight / 2.0;

    /* Spacing between columns. */
    const double kColumnSpacing = 200;

    /* Different colors for matched and unmatched boxes. */
    const string kMatchedBoxColor       = "#F3E5AB"; // Vanilla
    const string kMatchedBoxBorderColor = "Black";
    const string kMatchedBoxFontColor   = "Black";
    const string kMatchedBoxFont        = "Sans Serif-BOLD-13";

    const string kUnusedBoxColor       = "White";
    const string kUnusedBoxBorderColor = "Gray";
    const string kUnusedBoxFontColor   = "Gray";
    const string kUnusedBoxFont        = "Sans Serif-13";

    /* Line information. */
    const string kUnusedLineColor      = "#C0C0C0";
    const string kMatchedLineColor     = "Blue";
    const double kUnusedLineThickness  = 3;
    const double kMatchedLineThickness = 5;

    /* Lays out a single column. The X coordinate of the left side of the boxes is given
     * as input, along with the total window height.
     */
    template <typename PersonType>
    void layOutColumn(const Vector<PersonType>& nodes, HashMap<string, GRectangle>& result,
                      double leftX, double height) {
        /* Edge case: If there are no nodes, don't do anything. */
        if (nodes.size() == 0) return;

        /* Edge case: If there is one node, center it vertically. */
        if (nodes.size() == 1) {
            result[nodes.front().name] = { leftX, (height - kBoxHeight) / 2.0, kBoxWidth, kBoxHeight };
            return;
        }

        /* Adjust for the top/bottom margins. */
        double columnHeight = height - 2 * kVerticalPadding;

        /* Compute the spacing between boxes. */
        double boxSpacing = (columnHeight - kBoxHeight * nodes.size()) / (nodes.size() - 1);

        /* The base Y coordinate is the quantity that centers all these boxes. */
        double topY = (height - columnHeight) / 2.0;

        /* Lay out all the nodes. */
        int index = 0;
        for (const auto& node: nodes) {
            result[node.name] = { leftX, topY + (kBoxHeight + boxSpacing) * index, kBoxWidth, kBoxHeight };
            index++;
        }
    }

    /* Given a graph, determines the location of each node. */
    HashMap<string, GRectangle> layOutNodes(GWindow& window, const HospitalTestCase& hospital) {
        /* Determine the locations of the left and right columns. The entire
         * arrangement will be centered in the window with the appropriate
         * buffer spacing between.
         */
        double centerX = window.getCanvasWidth() / 2.0;
        double height  = window.getCanvasHeight();

        HashMap<string, GRectangle> result;
        layOutColumn(hospital.doctors,  result, centerX - kColumnSpacing / 2.0 - kBoxWidth, height);
        layOutColumn(hospital.patients, result, centerX + kColumnSpacing / 2.0, height);

        return result;
    }

    /* Draw edges with the specified graphics properties. */
    void drawEdges(GWindow& window, const Map<string, Set<string>>& edges,
                   const HashMap<string, GRectangle>& nodeLocations,
                   const string& color, double thickness) {
        GLine line(0, 0, 0, 0);
        line.setLineWidth(thickness);
        line.setColor(color);

        for (const auto& src: edges) {
            for (const auto& dst: edges[src]) {
                /* Line originates on the right-hand side of the doctor node and is vertically
                 * centered.
                 */
                auto doctorPos = nodeLocations[src];
                double x0 = doctorPos.getX() + doctorPos.getWidth();
                double y0 = doctorPos.getY() + doctorPos.getHeight() / 2.0;

                /* Line ends on the left-hand side of the patient node and is vertically
                 * centered.
                 */
                auto patientPos = nodeLocations[dst];
                double x1 = patientPos.getX();
                double y1 = patientPos.getY() + patientPos.getHeight() / 2.0;

                line.setLocation(x0, y0);
                line.setEndPoint(x1, y1);
                window.draw(&line);
            }
        }
    }

    /* Draws each node in a nice pretty box. */
    void drawNodes(GWindow& window,
                   const Set<string>& toDraw,
                   const HashMap<string, GRectangle>& nodeLocations,
                   const Map<string, string>& nodeModifiers,
                   const string& boxColor, const string& borderColor,
                   const string& font, const string& fontColor) {
        for (const auto& node: toDraw) {
            auto location = nodeLocations[node];

            /* Draw the background. */
            window.setColor(boxColor);
            window.fillRect(location);

            /* Draw the border. */
            window.setColor(borderColor);
            window.drawRect(location);

            /* Draw the name in the top half of the box. */
            drawCenteredText(window, {
                                 location.getX(), location.getY(),
                                 location.getWidth(), location.getHeight() / 2.0
                             }, node, font, fontColor);

            /* Draw the modifier in the top half of the box. */
            drawCenteredText(window, {
                                 location.getX(), location.getY() + location.getHeight() / 2.0,
                                 location.getWidth(), location.getHeight() / 2.0
                             }, nodeModifiers[node], font, fontColor);
        }
    }

    /* Converts a list of doctors or patients into a set. */
    template <typename Container>
    Set<string> asSet(const Container& c) {
        Set<string> result;
        for (const auto& elem: c) {
            result += elem.name;
        }
        return result;
    }

    /* Returns a list of all sample problems associated with this problem type. */
    vector<string> sampleProblems(const string& basePath) {
        vector<string> result;
        for (const auto& file: listDirectory(basePath)) {
            if (endsWith(file, kProblemSuffix)) {
                result.push_back(file);
            }
        }
        return result;
    }
}

struct DoctorGUI::Impl {
    Impl(DoctorGUI* owner) : mOwner(owner) {

    }

    /* Reference back to owner. */
    DoctorGUI* const mOwner;

    /* Dropdown of all the problems to choose from. */
    Temporary<GComboBox> mProblems;

    /* Button to trigger the solver. */
    Temporary<GButton> mSolve;

    /* Current network and solution. */
    HospitalTestCase mHospital;
    Map<string, Set<string>> mAssignment;
    bool mIsSolvable;

    /* Loads the world with the given name. */
    void loadWorld(const string& filename);

    /* Computes an optimal solution. */
    void solve();
};

DoctorGUI::DoctorGUI(GWindow& window) : mImpl(new Impl(this)) {
    GComboBox* choices = new GComboBox();
    for (const string& file: sampleProblems(kBasePath)) {
        choices->addItem(file);
    }
    choices->setEditable(false);

    mImpl->mProblems = Temporary<GComboBox>(choices, window, "SOUTH");
    mImpl->mSolve    = Temporary<GButton>(new GButton("Solve"), window, "SOUTH");

    mImpl->loadWorld(choices->getSelectedItem());
}

void DoctorGUI::actionPerformed(GObservable* source) {
    if (source == mImpl->mProblems) {
        mImpl->loadWorld(mImpl->mProblems->getSelectedItem());
    } else if (source == mImpl->mSolve) {
        mImpl->solve();
    }
}

void DoctorGUI::repaint(GWindow& window) {
    /* Clear display. */
    window.setColor(kBackgroundColor);
    window.fillRect(0, 0, window.getCanvasWidth(), window.getCanvasHeight());

    /* Map each node to its bounding box. */
    auto nodeLocations = layOutNodes(window, mImpl->mHospital);

    /* See which patients are covered. */
    Set<string> matchedNodes;
    for (const auto& doctor: mImpl->mAssignment) {
        for (const auto& patient: mImpl->mAssignment[doctor]) {
            matchedNodes += patient;
        }
        matchedNodes += doctor;
    }

    /* These modifiers are suffixed onto the node names and contain information about the
     * number of hours needed.
     */
    Map<string, string> nodeModifiers;
    for (const auto& doctor: mImpl->mHospital.doctors) {
        nodeModifiers[doctor.name] = "(" + pluralize(doctor.hoursFree, "hour") + " free)";
    }
    for (const auto& patient: mImpl->mHospital.patients) {
        nodeModifiers[patient.name] = "(" + pluralize(patient.hoursNeeded, "hour") + " needed)";
    }

    /* Draw lines connecting patients to their doctor. */
    drawEdges(window, mImpl->mAssignment, nodeLocations, kMatchedLineColor, kMatchedLineThickness);

    /* Draw matched and unmatched nodes separately for efficiency (minimize graphics calls). */
    drawNodes(window, (asSet(mImpl->mHospital.doctors) + asSet(mImpl->mHospital.patients)) - matchedNodes,
              nodeLocations,
              nodeModifiers,
              kUnusedBoxColor, kUnusedBoxBorderColor, kUnusedBoxFont, kUnusedBoxFontColor);
    drawNodes(window, matchedNodes, nodeLocations, nodeModifiers,
              kMatchedBoxColor, kMatchedBoxBorderColor, kMatchedBoxFont, kMatchedBoxFontColor);
}

void DoctorGUI::Impl::loadWorld(const string& filename) {
    ifstream input(kBasePath + filename);
    if (!input) error("Cannot open file.");

    mHospital = loadHospitalTestCase(input);
    mAssignment.clear();
    mOwner->requestRepaint();
}

void DoctorGUI::Impl::solve() {
    mAssignment.clear();
    mIsSolvable = canAllPatientsBeSeen(mHospital.doctors, mHospital.patients, mAssignment);
    if (mIsSolvable) {
        mOwner->requestRepaint();
    } else {
        GOptionPane::showMessageDialog("Sorry, there's no way for everyone to be seen.", "No Solution");
    }
}

