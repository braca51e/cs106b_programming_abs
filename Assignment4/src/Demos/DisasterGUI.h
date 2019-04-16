#ifndef DisasterGUI_Included
#define DisasterGUI_Included

#include "ProblemHandler.h"
#include <memory>

/* Problem handler to play around with the Disaster Prep function for
 * realistic data sets.
 */
class DisasterGUI: public ProblemHandler {
public:
    DisasterGUI(GWindow& window);

    void actionPerformed(GObservable* source) override;

protected:
    void repaint(GWindow& window) override;

private:
    struct Impl;
    std::shared_ptr<Impl> mImpl;
};

REGISTER_HANDLER(DisasterGUI, "Disaster Planning", 2);

#endif
