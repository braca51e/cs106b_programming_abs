#ifndef ElectionGUI_Included
#define ElectionGUI_Included

#include "ProblemHandler.h"
#include <memory>

/* Problem handler to visualize US Presidential Elections over time. */
class ElectionGUI: public ProblemHandler {
public:
    ElectionGUI(GWindow& window);

    void actionPerformed(GObservable* source) override;

protected:
    void repaint(GWindow& window) override;

private:
    struct Impl;
    std::shared_ptr<Impl> mImpl;
};

REGISTER_HANDLER(ElectionGUI, "Winning the Presidency", 3);

#endif
