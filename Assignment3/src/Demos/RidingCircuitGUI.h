#ifndef RidingCircuitGUI_Included
#define RidingCircuitGUI_Included

#include "ProblemHandler.h"
#include "TemporaryComponent.h"
#include "ginteractors.h"
#include "gobjects.h"
#include "vector.h"
#include "deque.h"
#include "gtimer.h"
#include <memory>

/* Problem handler to visualize circuit-riding paths. */
class RidingCircuitGUI: public ProblemHandler {
public:
    RidingCircuitGUI(GWindow& window);
    ~RidingCircuitGUI();

    static std::string name();

    /* React to button presses. */
    void actionPerformed(GObservable* source) override;

    /* Advance the animation on a timer tick. */
    void timerFired() override;

protected:
    void repaint(GWindow& window) override;

private:
    struct Impl;
    std::shared_ptr<Impl> mImpl;
};

REGISTER_HANDLER(RidingCircuitGUI, "Riding Circuit", 5);

#endif
