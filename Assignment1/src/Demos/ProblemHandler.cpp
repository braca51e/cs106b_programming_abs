#include "ProblemHandler.h"
#include "gthread.h"
using namespace std;

/* Issues a redraw, if necessary. */
void ProblemHandler::draw(GWindow& window) {
    if (isDirty) {
        GThread::runOnQtGuiThread([&, this] {
            repaint(window);
            window.repaint();
        });

        isDirty = false;
    }
}

/* Sets the dirty bit. */
void ProblemHandler::requestRepaint() {
    isDirty = true;
}

/* Default draw does nothing. */
void ProblemHandler::repaint(GWindow &) {
    // Do nothing
}

/* Default handler does nothing. */
void ProblemHandler::actionPerformed(GObservable *) {
    // Do nothing
}

/* Default handler does nothing. */
void ProblemHandler::timerFired() {
    // Do nothing
}

/* * Handler Table * */
multimap<int, pair<string, ProblemHandler::Constructor>>&
ProblemHandler::handlerTable() {
    static multimap<int, pair<string, ProblemHandler::Constructor>> instance;
    return instance;
}

unordered_map<type_index, once_flag>&
ProblemHandler::onceTable() {
    static unordered_map<type_index, once_flag> instance;
    return instance;
}

vector<pair<string, ProblemHandler::Constructor>>
ProblemHandler::handlers() {
    vector<pair<string, ProblemHandler::Constructor>> result;
    for (const auto& entry: handlerTable()) {
        result.push_back(entry.second);
    }
    return result;
}
