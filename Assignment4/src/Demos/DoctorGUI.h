#ifndef DoctorGUI_Included
#define DoctorGUI_Included

#include "ProblemHandler.h"
#include <memory>

/* Problem handler to play around with the Doctors Without Orders
 * problem on some synthetic data sets.
 */
class DoctorGUI: public ProblemHandler {
public:
    DoctorGUI(GWindow& window);

    void actionPerformed(GObservable* source) override;

protected:
    void repaint(GWindow& window) override;

private:
    struct Impl;
    std::shared_ptr<Impl> mImpl;
};

REGISTER_HANDLER(DoctorGUI, "Doctors Without Orders", 1);


#endif
