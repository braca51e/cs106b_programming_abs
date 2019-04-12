#include "FleschKincaidGUI.h"
#include "FleschKincaid.h"
#include "TemporaryComponent.h"
#include "GColorConsole.h"
#include "ginteractors.h"
#include "filelib.h"
#include <fstream>
using namespace std;

struct FleschKincaidGUI::Impl {
    Temporary<GButton> file;
    Temporary<GButton> rawInput;
    Temporary<GLabel>  inputLabel;
    Temporary<GTextField> inputText;

    Temporary<GColorConsole> display;

    /* Button to clear the console. */
    Temporary<GButton> clear;
};

FleschKincaidGUI::FleschKincaidGUI(GWindow& window) {
    mImpl = make_shared<Impl>();

    /* Standard buttons. */
    mImpl->file  = Temporary<GButton>(new GButton("Choose File..."), window, "SOUTH");
    mImpl->clear = Temporary<GButton>(new GButton("Clear Console"),  window, "WEST");

    /* Raw input. */
    mImpl->inputLabel = Temporary<GLabel>(new GLabel("        Or, enter your own text here: "), window, "SOUTH");

    mImpl->inputText = Temporary<GTextField>(new GTextField(), window, "SOUTH");
    mImpl->rawInput  = Temporary<GButton>(new GButton("Go!"), window, "SOUTH");

    /* Display. */
    mImpl->display   = Temporary<GColorConsole>(new GColorConsole(), window, "CENTER");
}

namespace {
    double gradeLevelFor(DocumentInfo stats) {
        static const double C_0 = -15.59;
        static const double C_1 =   0.39;
        static const double C_2 =  11.80;

        return C_0 +
               C_1 * stats.numWords / stats.numSentences +
               C_2 * stats.numSyllables / stats.numWords;
    }

    /* Inserts commas into a numeric string. */
    string addCommasTo(int val) {
        string asStr = to_string(val);
        const int length = int(asStr.length()) - (val < 0? 1 : 0);

        string result;
        for (int i = 0; i < length; i++) {
            /* Run backwards through the source string so determining where commas go
             * becomes a lot easier.
             */
            result = asStr[length - 1 - i] + result;

            /* Put commas in provided we've already added three characters, but aren't
             * about to use all the digits up.
             */
            if (i % 3 == 2 && i < length - 1) {
                result = ',' + result;
            }
        }
        return val < 0? "-" + result : result;
    }

    /* Processes a data stream, computing statistics about it. */
    void processStream(GColorConsole& out, istream& source) {
        auto info = statisticsFor(source);
        out << "  Sentences:   " << addCommasTo(info.numSentences) << endl;
        out << "  Words:       " << addCommasTo(info.numWords) << endl;
        out << "  Syllables:   " << addCommasTo(info.numSyllables) << endl;

        double level = gradeLevelFor(info);

        out << "  Grade Level: ";
        if (isnan(level)) {
            out.doWithStyle("#800000", GColorConsole::BOLD_ITALIC, [&] {
                out << "error: division by zero" << endl;
            });
        } else {
            out.doWithStyle("#800080", GColorConsole::BOLD, [&] {
                out << level << endl;
            });
        }

        out << endl;
    }
}

void FleschKincaidGUI::actionPerformed(GObservable* source) {
    if (source == mImpl->clear) {
        mImpl->display->clear();
    } else if (source == mImpl->rawInput || source == mImpl->inputText) {
        mImpl->display->doWithStyle("#000080", GColorConsole::BOLD, [&, this] {
            *mImpl->display << "Data Source: Raw Input" << endl;
        });

        istringstream source(mImpl->inputText->getText());
        processStream(*mImpl->display, source);
    } else if (source == mImpl->file) {
        string filename = GFileChooser::showOpenDialog("Choose a text file", ".", "Text Files (*.txt)");
        ifstream input(filename);

        if (input) {
            mImpl->display->doWithStyle("#000080", GColorConsole::BOLD, [&, this] {
                *mImpl->display << "Data File: " << getTail(filename) << endl;
            });
            processStream(*mImpl->display, input);
        }
    }
}
