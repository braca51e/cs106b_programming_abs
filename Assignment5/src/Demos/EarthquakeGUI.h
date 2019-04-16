#ifndef EarthquakeGUI_Included
#define EarthquakeGUI_Included

#include "ProblemHandler.h"
#include "TemporaryComponent.h"
#include "GUIUtils.h"
#include "ginteractors.h"
#include "vector.h"

/* Problem handler that lets the user see powerful earthquakes over time. */
class EarthquakeGUI: public ProblemHandler {
public:
    EarthquakeGUI(GWindow& window);

    void actionPerformed(GObservable* source) override;

protected:
    void repaint(GWindow& window) override;

private:
    struct Impl;
    std::shared_ptr<Impl> mImpl;
};

REGISTER_HANDLER(EarthquakeGUI, "Earthquakes", 4);

#endif
