#ifndef Womens800MGUI_Included
#define Womens800MGUI_Included

#include "ProblemHandler.h"

/* Problem handler that lets the user progressions in the Women's 800m Freestyle
 * swimming event at a collection of sporting events.
 */
class Womens800MGUI: public ProblemHandler {
public:
    Womens800MGUI(GWindow& window);

    void changeOccurredIn(GObservable* source) override;

protected:
    void repaint(GWindow& window) override;

private:
    struct Impl;
    std::shared_ptr<Impl> mImpl;
};

REGISTER_HANDLER(Womens800MGUI, "Women's 800m Freestyle", 5);

#endif
