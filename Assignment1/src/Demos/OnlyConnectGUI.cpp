#include "OnlyConnectGUI.h"
#include "OnlyConnect.h"
#include "TemporaryComponent.h"
#include "GColorConsole.h"
#include "ginteractors.h"
using namespace std;

struct OnlyConnectGUI::Impl {
    Temporary<GLabel>     inputLabel;
    Temporary<GTextField> inputText;
    Temporary<GButton>    go;

    Temporary<GColorConsole> display;

    /* Button to clear the console. */
    Temporary<GButton> clear;
};

OnlyConnectGUI::OnlyConnectGUI(GWindow& window) {
    mImpl = make_shared<Impl>();

    /* Standard buttons. */
    mImpl->clear = Temporary<GButton>(new GButton("Clear Console"),  window, "WEST");

    /* Raw input. */
    mImpl->inputLabel = Temporary<GLabel>(new GLabel("Enter text to convert: "), window, "SOUTH");

    mImpl->inputText = Temporary<GTextField>(new GTextField(), window, "SOUTH");
    mImpl->go        = Temporary<GButton>(new GButton("Go!"), window, "SOUTH");

    /* Display. */
    mImpl->display   = Temporary<GColorConsole>(new GColorConsole(), window, "CENTER");
}

namespace {
    void processString(GColorConsole& out, const string& text) {
        out << "Input:  " << text << endl;
        out << "Output: ";

        out.doWithStyle("#000080", GColorConsole::BOLD, [&] {
            out << onlyConnectize(text) << endl << endl;
        });
    }
}

void OnlyConnectGUI::actionPerformed(GObservable* source) {
    if (source == mImpl->clear) {
        mImpl->display->clear();
    } else if (source == mImpl->go || source == mImpl->inputText) {
        processString(*mImpl->display, mImpl->inputText->getText());
        mImpl->inputText->setText("");
    }
}
