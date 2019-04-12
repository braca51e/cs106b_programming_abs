#ifndef PlayingFairGUI_Included
#define PlayingFairGUI_Included

#include "ProblemHandler.h"
#include <memory>

class PlayingFairGUI: public ProblemHandler {
public:
    PlayingFairGUI(GWindow& window);
    void actionPerformed(GObservable* source) override;

protected:
    void repaint(GWindow &) override;

private:
    struct Impl;
    std::shared_ptr<Impl> mImpl;
};

REGISTER_HANDLER(PlayingFairGUI, "Playing Fair", 3);



#endif
