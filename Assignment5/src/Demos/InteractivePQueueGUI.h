#ifndef InteractivePQueueGUI_Included
#define InteractivePQueueGUI_Included

#include "ProblemHandler.h"
#include <memory>

/* Problem handler that lets the user interactively test the PQueue type. */
class InteractivePQueueGUI: public ProblemHandler {
public:
    InteractivePQueueGUI(GWindow& window);

    void actionPerformed(GObservable* source) override;

private:
    struct Impl;
    std::shared_ptr<Impl> mImpl;
};

REGISTER_HANDLER(InteractivePQueueGUI, "Interactive PQueue", 2);

#endif
