#ifndef ProblemHandler_Included
#define ProblemHandler_Included

#include "gwindow.h"
#include "gobjects.h"
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <utility>
#include <typeindex>
#include <mutex>
#include <unordered_map>

/* Interface representing a type that's capable of displaying a problem
 * graphically and responding to events.
 *
 * The intent is that each problem handler will be spun up and torn down
 * based on what the user chooses to do, so don't assume you will have
 * access to long-running state in the course of implementing these types.
 *
 * Each ProblemHandler must also export the following functions:
 *
 *      Constructor(GWindow&): Construct a new handler in the associated window.
 */
class ProblemHandler {
public:
    /* Polymorphic classes need virtual destructors. */
    virtual ~ProblemHandler() = default;

    /* Respond to action events. */
    virtual void actionPerformed(GObservable* source);

    /* Respond to timer events. */
    virtual void timerFired();

    /* Redraws the window. */
    void draw(GWindow& window);

    /* Registry associating problem names with constructors that build ProblemHandlers.
     * All handler types are expected to register themselves here. The GUI driver can
     * then retrieve the constructors.
     *
     * Each registration should provide an index to display at and a name.
     *
     * Add the following macro to a each handler's header file - outside of the body
     * of the handler - to automatically hook it into the system:
     *
     *    REGISTER_HANDLER(typename, "description", index);
     */
    using Constructor = std::function<std::shared_ptr<ProblemHandler>(GWindow &)>;

    template <typename Problem>
    static void registerHandler(int index, const std::string& name);

    /* Returns a list of all the handlers registered so far, in order. */
    static std::vector<std::pair<std::string, Constructor>> handlers();

protected:
    /* Draw the current state of things. */
    virtual void repaint(GWindow& window);

    /* Marks the region as dirty. */
    void requestRepaint();

private:
    /* Dirty bit. We're initially dirty because nothing's been drawn yet. */
    bool isDirty = true;

    /* Master list of handlers. This is stored as a multimap; keys are indices,
     * values are the pairs of strings and constructors. We use a multimap so that
     * if there's multiple handlers submitted with the same index, we don't
     * run into trouble.
     */
    static std::multimap<int, std::pair<std::string, Constructor>>& handlerTable();

    /* Master map from handler types to std::once_flags, used to ensure that we don't
     * accidentally install the same handler twice.
     */
    static std::unordered_map<std::type_index, std::once_flag>& onceTable();
};

/* * * * * * Implementation Below This Point * * * * * */
template <typename Problem>
void ProblemHandler::registerHandler(int index, const std::string& name) {
    /* Use call_once to guard against repeat initialization with the same type, which
     * could happen if, for example, muliple files include the same header for the
     * type.
     */
    std::call_once(onceTable()[std::type_index(typeid(Problem))], [&] {
        std::pair<int, std::pair<std::string, Constructor>> entry;
        entry.first = index;
        entry.second.first = name;
        entry.second.second = [] (GWindow& window) {
            return std::make_shared<Problem>(window);
        };

        handlerTable().insert(entry);
    });
}

/* DECLARE_HANDLER macro. */
#ifdef REGISTER_HANDLER
#undef REGISTER_HANDLER
#endif

#define REGISTER_HANDLER(type, description, index)                         \
    static const int _unused_initializer_for_##type = [] {                \
        ProblemHandler::registerHandler<type>(index, description);        \
        return 137;                                                       \
    } ()


#endif
