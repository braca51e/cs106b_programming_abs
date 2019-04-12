#include "StackOverflowGUI.h"
#include "StackOverflow.h"
#include "goptionpane.h"
using namespace std;

namespace {
    const string kWarningMessage =
R"(Warning! This is going to crash your program.
Make sure you ran your program using the "Debug" option,
the green triangle with the little bug icon on it.)";
}

StackOverflowGUI::StackOverflowGUI(GWindow &) {
    if (GOptionPane::showConfirmDialog(kWarningMessage, "Warning!") == GOptionPane::CONFIRM_YES) {
        triggerStackOverflow(137);
    } else {
        error("User declined to trigger a stack overflow.");
    }
}
