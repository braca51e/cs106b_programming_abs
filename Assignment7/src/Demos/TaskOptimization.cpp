#include "ProblemHandler.h"
#include "TemporaryComponent.h"
#include "ProcessOptimizer.h"
#include "DownloadCache.h"
#include "hashset.h"
#include "gevents.h"
#include "ginteractors.h"
#include "linkedhashset.h"
#include "linkedhashmap.h"
#include "filelib.h"
#include "error.h"
#include "strlib.h"
#include <fstream>
#include <istream>
#include <iomanip>
#include <sstream>
#include <cstdint>
using namespace std;

namespace {
    /* Window display attributes. */
    const string kBackgroundColor = "White";

    /* Text to display when solving. */
    const string kSolvingString             = "Solving...";
    const string kDownloadingString         = "Downloading visualization...";
    const string kRemoteDownloadErrorString = "Oops! We weren't able to download a visualization for this process. (This isn't your fault. Try that operation again.)";
    const string kVisualizationErrorString  = "Oops! We weren't able to download a visualization for this process. (Check your network connection?)";

    /* Button labels. */
    const string kLoadButtonText     = "Load Process";
    const string kOptimizeButtonText = "Find Optimization Candidates";

    /* Constants for transferring data from the Google Charts API to the display. */
    const string kAPIBase = "https://chart.googleapis.com/chart?cht=gv&chof=png&chl=";

    /* Where to look for files. */
    const string kBasePath = "processes/";
    const string kFileSuffix = ".process";

    /* Given a set of tasks, sorts those tasks alphabetically. This is used to get a
     * canonical ordering of the tasks in a group.
     */
    vector<Task *> sortedVersionOf(const HashSet<Task *>& tasks) {
        vector<Task *> result(tasks.begin(), tasks.end());
        sort(result.begin(), result.end(), [](Task* lhs, Task* rhs) {
            return lhs->name < rhs->name;
        });
        return result;
    }

    /* Given a process, produces a string containing a GraphViz representation of that process. */
    string toGraphviz(const HashSet<Task *>& process, const HashSet<Task*>& hotSpots) {
        /* We need to come up with a canonical ordering for processes to ensure that we generate
         * consistent GraphViz files for the same process. We'll do this by sorting the
         * processes alphabetically.
         */
        vector<Task *> orderedProcess = sortedVersionOf(process);

        /* Each task gets mapped to an integer ID equal to its position. This is the reverse
         * mapping.
         */
        HashMap<Task*, int> taskToID;
        for (int i = 0; i < orderedProcess.size(); i++) {
            taskToID[orderedProcess[i]] = i;
        }

        ostringstream output;
        output << "digraph G { " << endl;

        /* Define each of the nodes. */
        for (auto task: orderedProcess) {
            output << "  " << taskToID[task] << " [shape=box,penwidth=2,";
            if (hotSpots.contains(task)) {
                output << "fillcolor=gold,style=filled,";
            }
            output << "label=\"" << task->name << "\\n(Time: " << task->duration << ")\"];" << endl;
        }

        /* Define the edges. */
        for (auto task: orderedProcess) {
            for (auto dependency: sortedVersionOf(task->requiredFor)) {
                output << "  " << taskToID[task] << " -> " << taskToID[dependency] << " [style=bold]" << endl;
            }
        }

        output << "}";

        return output.str();
    }

    /* Converts a string that will be used as part of a URL into a series of escaped
     * characters.
     */
    string urlEscape(const string& str) {
        ostringstream result;
        for (char ch: str) {
            if (isalnum(ch)) {
                result << ch;
            } else {
                result << '%' << setw(2) << setfill('0') << hex << int((unsigned char)ch);
            }
        }
        return result.str();
    }

    /* Returns the name of a file containing a visualization of the given process. */
    unique_ptr<GImage>
    imageFor(const HashSet<Task *>& process,
             const HashSet<Task *>& hotSpots,
             GLabel* statusLine) {

        /* Get the URL of the API call to make. */
        auto url = kAPIBase + urlEscape(toGraphviz(process, hotSpots));
        auto result = webContentsOf(url, ".png", [&] (DownloadStatus status) {
            if (status == DownloadStatus::DOWNLOADING) {
                statusLine->setText(kDownloadingString);
            } else if (status == DownloadStatus::FINISHED) {
                statusLine->setText(" ");
            }
        });

        return unique_ptr<GImage>(new GImage(*result));
    }
}

namespace {
    /* Names of the different headers that appear in a file describing a task. */
    const string kNameHeader = "Task:";
    const string kDependencyHeader = "Depends On:";
    const string kDurationHeader = "Duration:";

    /* Given a string consisting of a name, possibly with leading or trailing spaces,
     * returns a canonical version of that name.
     */
    string extractNameFrom(const string& name) {
        string result = trim(name);
        if (result.empty() || result.find(',') != string::npos) error("Illegal name.");

        return result;
    }

    /* Given a name line, returns the name from that line. */
    string parseName(const string& nameLine) {
        if (!startsWith(nameLine, kNameHeader)) error("Malformed name.");

        return extractNameFrom(nameLine.substr(kNameHeader.length()));
    }

    /* Given a dependencies line, returns a list of dependencies from that line. */
    LinkedHashSet<string> parseDependencies(const string& dependenciesLine) {
        if (!startsWith(dependenciesLine, kDependencyHeader)) error("Malformed dependencies.");

        /* The rest of the line is a comma-separated list of dependencies. */
        auto rawNames = stringSplit(dependenciesLine.substr(kDependencyHeader.size()), ",");

        LinkedHashSet<string> result;
        for (string name: rawNames) {
            result += extractNameFrom(name);
        }

        return result;
    }

    /* Given a duration line, returns a number of seconds equal to that duration. */
    Duration parseDuration(const string& durationLine) {
        if (!startsWith(durationLine, kDurationHeader)) error("Malformed duration.");

        istringstream time(durationLine.substr(kDurationHeader.size()));
        Duration result;
        time >> result;
        return result;
    }

    /* A type representing a task, except that task links are referenced purely as
     * string names rather than hard pointers. This is used internally inside loadProcess
     * to build up the graph.
     */
    struct ProtoTask {
        string name;
        Duration duration;
        LinkedHashSet<string> dependsOn; // To maintain the relative order.
    };

    /* Reads a ProtoTask from an input stream, reporting an error if it's not possible to
     * do so.
     */
    istream& operator>> (istream& input, ProtoTask& pt) {
        istream::sentry sentry(input);
        if (sentry) {
            string name, duration, dependencies;

            /* If we can't even read the name line, we're done. */
            if (!getline(input, name)) return input;

            /* Otherwise, read the rest, which should succeed. */
            getline(input, duration);
            getline(input, dependencies);

            /* Strip whitespace. */
            name = trim(name);
            duration = trim(duration);
            dependencies = trim(dependencies);

            pt.name      = parseName(name);
            pt.duration  = parseDuration(duration);
            pt.dependsOn = parseDependencies(dependencies);
        }

        return input;
    }

    HashSet<Task *> loadTaskGroup(istream& input) {
        /* Read a list of prototasks. */
        LinkedHashMap<string, ProtoTask> prototasks;
        for (ProtoTask task; input >> task; ) {
            if (prototasks.containsKey(task.name)) error("Duplicate name: " + task.name);
            prototasks.put(task.name, task);
        }

        /* Validate that all internal links are to existing tasks. */
        for (const auto& task: prototasks) {
            for (const auto& dependency: prototasks[task].dependsOn) {
                if (!prototasks.containsKey(dependency)) error("Task " + task + " depends on unknown task " + dependency);
            }
        }

        /* Convert everything into a proper set of tasks. */
        LinkedHashMap<string, Task *> tasks;
        HashSet<Task *> result;
        for (const auto& task: prototasks) {
            tasks.put(task, new Task{ prototasks[task].name, prototasks[task].duration, {} });
        }
        for (const auto& task: prototasks) {
            for (const auto& dependency: prototasks[task].dependsOn) {
                tasks[dependency]->requiredFor += tasks[task];
            }
        }

        for (const auto& task: tasks) {
            result += tasks[task];
        }

        return result;
    }

    /* Cleans up the memory used by a process. */
    void freeProcess(HashSet<Task *>& process) {
        for (auto task: process) {
            delete task;
        }
    }

    /* Returns all sample problems found in the example directory. */
    vector<string> sampleProblems() {
        vector<string> result;
        for (const auto& file: listDirectory(kBasePath)) {
            if (endsWith(file, kFileSuffix)) {
                result.push_back(file);
            }
        }
        return result;
    }
}

/* Output a human-readable representation of a task. */
ostream& operator<< (ostream& out, const Task& task) {
    ostringstream result;
    result << "{ "
           << kNameHeader << " " << task.name << ", "
           << kDurationHeader << " " << task.duration << ", "
           << kDependencyHeader << " ";

    /* Linearize the tasks. */
    vector<Task*> orderedTasks(task.requiredFor.begin(), task.requiredFor.end());
    for (int i = 0; i < orderedTasks.size(); i++) {
        result << orderedTasks[i]->name;
        if (i + 1 != orderedTasks.size()) result << ", ";
    }
    result << " }";

    return out << result.str();
}

/* Problem class for task optimization */
class TaskOptimization: public ProblemHandler {
public:
    /* Construct handler, assuming the input stream contains the data to read. */
    TaskOptimization(GWindow& window);

    /* Destructor frees the underlying process. */
    ~TaskOptimization();

    /* File extension. */
    static std::string fileExtension();

    /* Respond to action events. */
    void actionPerformed(GObservable* source) override;
    void changeOccurredIn(GObservable* source) override;

protected:
    /* Draw the current state of things. */
    void repaint(GWindow& window) override;

private:
    Temporary<GContainer> container;
    GComboBox* processChooser;
    GButton*   solveButton;
    GLabel*    statusLine;

    /* Image to display in the window. */
    unique_ptr<GImage> image;

    /* Underlying process and identified hot spots. */
    HashSet<Task *> process;
    HashSet<Task*> hotSpots;

    void setActiveProcess(const string& filename);

    /* Attempts to change the currently-displayed picture based on the process to display.
     * This may fail because there's no way to load an image, in which case the image is
     * left blank.
     */
    void recalculateImage();
};

TaskOptimization::TaskOptimization(GWindow& window) {
    auto* rawContainer = new GContainer(GContainer::LAYOUT_GRID);

    processChooser = new GComboBox();
    for (const string& file: sampleProblems()) {
        processChooser->addItem(file);
    }
    processChooser->setEditable(false);
    solveButton = new GButton(kOptimizeButtonText);
    statusLine  = new GLabel(" ");

    rawContainer->addToGrid(processChooser,  0, 0);
    rawContainer->addToGrid(solveButton, 0, 1);
    rawContainer->addToGrid(statusLine,  1, 0, 1, 2);

    container = Temporary<GContainer>(rawContainer, window, "SOUTH");
    setActiveProcess(processChooser->getSelectedItem());
}

TaskOptimization::~TaskOptimization() {
    freeProcess(process);
}

void TaskOptimization::actionPerformed(GObservable* source) {
    if (source == solveButton) {
        container->setEnabled(false);
        statusLine->setText(kSolvingString);

        hotSpots = optimizationCandidatesFor(process);
        recalculateImage();

        statusLine->setText(" ");
        container->setEnabled(true);
    }
}

void TaskOptimization::changeOccurredIn(GObservable* source) {
    if (source == processChooser) {
        setActiveProcess(processChooser->getSelectedItem());
    }
}

void TaskOptimization::repaint(GWindow& window) {
    /* Clear the display. */
    window.setColor(kBackgroundColor);
    window.fillRect(0, 0, window.getCanvasWidth(), window.getCanvasHeight());

    /* If there is no active process, there's nothing to do. */
    if (image) {
        /* Scale the image so that we fit, but don't distort things. */
        const double scale = min(1.0, min(window.getCanvasWidth() / image->getWidth(), window.getCanvasHeight() / image->getHeight()));
        image->scale(scale);

        /* Center the image in the window. */

        /* TODO: As of today (2/27/19), there is a bug in the graphics package that causes scaled images
         * to factor the scale into the translation in addition to the width and height. We need to undo
         * those effects here.
         */
        double x = (window.getCanvasWidth()  - image->getWidth()  * scale) / (2.0 * scale);
        double y = (window.getCanvasHeight() - image->getHeight() * scale) / (2.0 * scale);

        window.draw(*image, x, y);
    }
}

void TaskOptimization::setActiveProcess(const string& filename) {
    ifstream input(kBasePath + filename);
    if (!input) error("Could not open file " + kBasePath + filename);

    statusLine->setText(" ");

    freeProcess(process);
    hotSpots.clear();

    process = loadTaskGroup(input);
    recalculateImage();
}

void TaskOptimization::recalculateImage() {
    image.release();
    try {
        image = imageFor(process, hotSpots, statusLine);
    } catch (const DownloadError& e) {
        if (e.errorCode() < 0) {
            statusLine->setText(kVisualizationErrorString);
        } else {
            statusLine->setText(kRemoteDownloadErrorString);
        }
    }
    requestRepaint();
}

shared_ptr<ProblemHandler> makeOptimizerGUI(GWindow& window) {
    return make_shared<TaskOptimization>(window);
}
