#ifndef PerformanceGUI_Included
#define PerformanceGUI_Included

#include "ProblemHandler.h"
#include <memory>

/* Problem handler that reports timing information for the different types. */
class PerformanceGUI: public ProblemHandler {
public:
    PerformanceGUI(GWindow& window);

    void timerFired() override;
    void actionPerformed(GObservable* source) override;

protected:
    void repaint(GWindow& window) override;

private:
    struct Impl;
    std::shared_ptr<Impl> mImpl;
};

REGISTER_HANDLER(PerformanceGUI, "Time Tests", 1);

#endif
