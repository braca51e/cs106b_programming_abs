#include "TestingGUI.h"
#include "TemporaryComponent.h"
#include "GColorConsole.h"
#include "Testing/TestDriver.h"
#include "error.h"
using namespace std;

namespace {
    /* Runs all the tests in a given test group. */
    void runTestsIn(GColorConsole& out,
                    const string& name,
                    const multimap<int, pair<string, TestCase>>& tests) {
        out.doWithStyle("#000080", GColorConsole::BOLD, [&] {
            out << "==== Testing " << name << " ====" << endl;
        });

        size_t numTests = 0;
        size_t numSuccesses = 0;

        /* Run each test. */
        for (const auto& entry: tests) {
            const auto& test = entry.second;
            try {
                test.second();
                out.doWithStyle("#404040", GColorConsole::ITALIC, [&] {
                    out << "  pass: " << test.first << endl;
                });
                numSuccesses++;
            } catch (const TestFailedException& e) {
                out.doWithStyle("#800000", GColorConsole::BOLD, [&] {
                    out << "  FAIL:  " << test.first << endl;
                    out << "    " << e.what() << endl;
                });
            } catch (const ErrorException& e) {
                out.doWithStyle("#FF0000", GColorConsole::BOLD, [&] {
                    out << "  ERROR: " << test.first << endl;
                    out << "    Test failed due to the program triggering an ErrorException." << endl;
                    out << endl;
                    out << "    This means that the test did not fail because of a call" << endl;
                    out << "    to EXPECT() or EXPECT_ERROR() failing, but rather because" << endl;
                    out << "    some code explicitly called the error() function." << endl;
                    out << endl;
                    out << "    Error: " << e.getMessage() << endl;
                });
            } catch (const exception& e) {
                out.doWithStyle("#FF0000", GColorConsole::BOLD, [&] {
                    out << "  ERROR: " << test.first << endl;
                    out << "    Test failed due to the program triggering an exception." << endl;
                    out << endl;
                    out << "    This means that the test did not fail because of a call" << endl;
                    out << "    to EXPECT() or an EXPECT_ERROR() failing, but rather because" << endl;
                    out << "    some code - probably an internal C++ library - triggered" << endl;
                    out << "    an error." << endl;
                    out << endl;
                    out << "    Error: " << e.what() << endl;
                });
            } catch (...) {
                out.doWithStyle("#FF0000", GColorConsole::BOLD, [&] {
                    out << "  FAIL: " << test.first << endl;
                    out << "    Test failed due to the program triggering an unknown type" << endl;
                    out << "    of exception. " << endl;
                    out << endl;
                    out << "    This means that the test did not fail because of a call" << endl;
                    out << "    to EXPECT() or an EXPECT_ERROR() failing, but rather because" << endl;
                    out << "    some code triggered an error whose format we couldn't" << endl;
                    out << "    recognize." << endl;
                    out << endl;
                });
            }
            numTests++;
        }

        out.doWithStyle("#404040", GColorConsole::ITALIC, [&] {
            out << "Summary: " << numSuccesses << " / " << numTests
                 << " test" << (numSuccesses == 1? "" : "s") << " passed." << endl;
            out << endl;
            out << endl;
        });
    }
}

struct TestingGUI::Impl {
    Temporary<GColorConsole> console;
};

TestingGUI::TestingGUI(GWindow& window) {
    mImpl = make_shared<Impl>();

    mImpl->console = Temporary<GColorConsole>(new GColorConsole(), window, "CENTER");

    for (const auto& entry: gTestsMap()) {
        runTestsIn(*mImpl->console, entry.first.second, entry.second);
    }
}
