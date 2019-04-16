#ifndef ProcessOptimizer_Included
#define ProcessOptimizer_Included

#include "Duration.h"
#include "hashset.h"

/**
 * Type: Task
 *
 * A type representing a single task that needs to be performed.
 */
struct Task {
    std::string name;  // Name of the task
    Duration duration; // How long it takes to complete

    /* Set of all tasks that this task is a prerequisite for. In the pictures of the
     * processes that we've shown, this is the set of all tasks that we have an outgoing
     * arrow to.
     *
     * For example, if task A is a prerequisite of task B, then A's requiredFor set will
     * contain B.
     */
    HashSet<Task *> requiredFor;
};

/**
 * Helper function to print tasks. You can use this function by writing something like
 *
 *    cout << myTask << endl;
 *
 * Remember that if you have a pointer to a task - that is, a Task* - you'd need to write
 *
 *    cout << *myTask << endl;
 *
 * since otherwise you'll just print out the value of the pointer itself, not what it points
 * at.
 */
std::ostream& operator<< (std::ostream& out, const Task& task);

/**
 * Given a process, returns a list of all the tasks in that process that, if optimized in isolation
 * from one another, would improve the completion time of the overall process. Here, we measure the
 * completion time of a process by when the very last process to finish ends up finishing.
 *
 * It's possible that there are no tasks that can be individually optimized to improve the overall
 * runtime. In that case, you should return an empty set.
 *
 * You don't need to consider the case where the process you're given contains a cycle. That would
 * be an impossible process to complete.
 *
 * @param process The process in question.
 * @return A set of all the tasks that, if optimized, would improve the overall completion time.
 */
HashSet<Task *> optimizationCandidatesFor(const HashSet<Task *>& process);

#endif
