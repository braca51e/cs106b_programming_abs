#ifndef ChildMortalityGUI_Included
#define ChildMortalityGUI_Included

#include "ProblemHandler.h"
#include <memory>

/* Problem handler that lets the user see how child mortality rates have improved
 * since 1990.
 */
class ChildMortalityGUI: public ProblemHandler {
public:
    ChildMortalityGUI(GWindow& window);

    void actionPerformed(GObservable* source) override;

protected:
    void repaint(GWindow& window) override;

private:
    struct Impl;
    std::shared_ptr<Impl> mImpl;
};

REGISTER_HANDLER(ChildMortalityGUI, "Child Mortality", 3);


#endif
