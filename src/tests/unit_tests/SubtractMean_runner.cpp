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
#include "TestSubtractMean.h"

static SubtractMean_runner suite_SubtractMean_runner;

static CxxTest::List Tests_SubtractMean_runner = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_SubtractMean_runner( "TestSubtractMean.h", 14, "SubtractMean_runner", suite_SubtractMean_runner, Tests_SubtractMean_runner );

static class TestDescription_SubtractMean_runner_test_flow_settings : public CxxTest::RealTestDescription {
public:
 TestDescription_SubtractMean_runner_test_flow_settings() : CxxTest::RealTestDescription( Tests_SubtractMean_runner, suiteDescription_SubtractMean_runner, 40, "test_flow_settings" ) {}
 void runTest() { suite_SubtractMean_runner.test_flow_settings(); }
} testDescription_SubtractMean_runner_test_flow_settings;

static class TestDescription_SubtractMean_runner_test_process : public CxxTest::RealTestDescription {
public:
 TestDescription_SubtractMean_runner_test_process() : CxxTest::RealTestDescription( Tests_SubtractMean_runner, suiteDescription_SubtractMean_runner, 60, "test_process" ) {}
 void runTest() { suite_SubtractMean_runner.test_process(); }
} testDescription_SubtractMean_runner_test_process;

#include <cxxtest/Root.cpp>
