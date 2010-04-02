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
#include "TestRunningAutocorrelation.h"

static RunningAutocorrelation_runner suite_RunningAutocorrelation_runner;

static CxxTest::List Tests_RunningAutocorrelation_runner = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_RunningAutocorrelation_runner( "TestRunningAutocorrelation.h", 41, "RunningAutocorrelation_runner", suite_RunningAutocorrelation_runner, Tests_RunningAutocorrelation_runner );

static class TestDescription_RunningAutocorrelation_runner_test_output_flow : public CxxTest::RealTestDescription {
public:
 TestDescription_RunningAutocorrelation_runner_test_output_flow() : CxxTest::RealTestDescription( Tests_RunningAutocorrelation_runner, suiteDescription_RunningAutocorrelation_runner, 65, "test_output_flow" ) {}
 void runTest() { suite_RunningAutocorrelation_runner.test_output_flow(); }
} testDescription_RunningAutocorrelation_runner_test_output_flow;

static class TestDescription_RunningAutocorrelation_runner_test_one_slice_single_row : public CxxTest::RealTestDescription {
public:
 TestDescription_RunningAutocorrelation_runner_test_one_slice_single_row() : CxxTest::RealTestDescription( Tests_RunningAutocorrelation_runner, suiteDescription_RunningAutocorrelation_runner, 82, "test_one_slice_single_row" ) {}
 void runTest() { suite_RunningAutocorrelation_runner.test_one_slice_single_row(); }
} testDescription_RunningAutocorrelation_runner_test_one_slice_single_row;

static class TestDescription_RunningAutocorrelation_runner_test_one_slice_multirow : public CxxTest::RealTestDescription {
public:
 TestDescription_RunningAutocorrelation_runner_test_one_slice_multirow() : CxxTest::RealTestDescription( Tests_RunningAutocorrelation_runner, suiteDescription_RunningAutocorrelation_runner, 116, "test_one_slice_multirow" ) {}
 void runTest() { suite_RunningAutocorrelation_runner.test_one_slice_multirow(); }
} testDescription_RunningAutocorrelation_runner_test_one_slice_multirow;

#include <cxxtest/Root.cpp>
