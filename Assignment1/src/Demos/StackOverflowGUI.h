#ifndef StackOverflowGUI_Included
#define StackOverflowGUI_Included

#include "ProblemHandler.h"
#include <memory>

/* This is an unusual one - it doesn't actually visualize anything, and instead
 * either (1) crashes or (2) aborts with an exception.
 */
class StackOverflowGUI: public ProblemHandler {
public:
    StackOverflowGUI(GWindow&);
};

REGISTER_HANDLER(StackOverflowGUI, "Stack Overflow", 1);



#endif
