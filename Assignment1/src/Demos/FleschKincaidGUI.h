#ifndef FleschKincaidGUI_Included
#define FleschKincaidGUI_Included

#include "ProblemHandler.h"
#include <memory>

class FleschKincaidGUI: public ProblemHandler {
public:
    FleschKincaidGUI(GWindow& window);
    void actionPerformed(GObservable* source) override;

private:
    struct Impl;
    std::shared_ptr<Impl> mImpl;
};

REGISTER_HANDLER(FleschKincaidGUI, "Flesch-Kincaid", 5);



#endif
