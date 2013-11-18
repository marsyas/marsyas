/* Generated file, do not edit */

#ifndef CXXTEST_RUNNING
#define CXXTEST_RUNNING
#endif

#define _CXXTEST_HAVE_STD
#include <cxxtest/TestListener.h>
#include <cxxtest/TestTracker.h>
#include <cxxtest/TestRunner.h>
#include <cxxtest/RealDescriptions.h>
#include <cxxtest/ErrorPrinter.h>

int main() {
 return CxxTest::ErrorPrinter().run();
}
#include "TestFanout.h"

static Fanout_runner suite_Fanout_runner;

static CxxTest::List Tests_Fanout_runner = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_Fanout_runner( "TestFanout.h", 41, "Fanout_runner", suite_Fanout_runner, Tests_Fanout_runner );

static class TestDescription_Fanout_runner_test_basic_routing : public CxxTest::RealTestDescription {
public:
 TestDescription_Fanout_runner_test_basic_routing() : CxxTest::RealTestDescription( Tests_Fanout_runner, suiteDescription_Fanout_runner, 100, "test_basic_routing" ) {}
 void runTest() { suite_Fanout_runner.test_basic_routing(); }
} testDescription_Fanout_runner_test_basic_routing;

#include <cxxtest/Root.cpp>
