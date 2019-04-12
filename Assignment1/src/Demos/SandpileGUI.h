#ifndef SandpileGUI_Included
#define SandpileGUI_Included

#include "ProblemHandler.h"
#include <memory>

/* Problem handler for visualizing a growing sandpile. */
class SandpileGUI: public ProblemHandler {
public:
    SandpileGUI(GWindow& window);
    void actionPerformed(GObservable* source) override;
    void timerFired() override;

protected:
    void repaint(GWindow& window) override;

private:
    struct Impl;
    std::shared_ptr<Impl> mImpl;
};

REGISTER_HANDLER(SandpileGUI, "Sandpiles", 4);


#endif
