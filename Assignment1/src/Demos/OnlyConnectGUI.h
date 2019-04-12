#ifndef OnlyConnectGUI_Included
#define OnlyConnectGUI_Included

#include "ProblemHandler.h"
#include <memory>

class OnlyConnectGUI: public ProblemHandler {
public:
    OnlyConnectGUI(GWindow& window);
    void actionPerformed(GObservable* source) override;

private:
    struct Impl;
    std::shared_ptr<Impl> mImpl;
};

REGISTER_HANDLER(OnlyConnectGUI, "Only Connect", 2);



#endif
