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
#include "TestDeltaFirstOrderRegression.h"

static DeltaFirstOrderRegression_runner suite_DeltaFirstOrderRegression_runner;

static CxxTest::List Tests_DeltaFirstOrderRegression_runner = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_DeltaFirstOrderRegression_runner( "TestDeltaFirstOrderRegression.h", 16, "DeltaFirstOrderRegression_runner", suite_DeltaFirstOrderRegression_runner, Tests_DeltaFirstOrderRegression_runner );

static class TestDescription_DeltaFirstOrderRegression_runner_test_flow_settings : public CxxTest::RealTestDescription {
public:
 TestDescription_DeltaFirstOrderRegression_runner_test_flow_settings() : CxxTest::RealTestDescription( Tests_DeltaFirstOrderRegression_runner, suiteDescription_DeltaFirstOrderRegression_runner, 42, "test_flow_settings" ) {}
 void runTest() { suite_DeltaFirstOrderRegression_runner.test_flow_settings(); }
} testDescription_DeltaFirstOrderRegression_runner_test_flow_settings;

static class TestDescription_DeltaFirstOrderRegression_runner_test_obsnames : public CxxTest::RealTestDescription {
public:
 TestDescription_DeltaFirstOrderRegression_runner_test_obsnames() : CxxTest::RealTestDescription( Tests_DeltaFirstOrderRegression_runner, suiteDescription_DeltaFirstOrderRegression_runner, 59, "test_obsnames" ) {}
 void runTest() { suite_DeltaFirstOrderRegression_runner.test_obsnames(); }
} testDescription_DeltaFirstOrderRegression_runner_test_obsnames;

static class TestDescription_DeltaFirstOrderRegression_runner_test_process_one_slice : public CxxTest::RealTestDescription {
public:
 TestDescription_DeltaFirstOrderRegression_runner_test_process_one_slice() : CxxTest::RealTestDescription( Tests_DeltaFirstOrderRegression_runner, suiteDescription_DeltaFirstOrderRegression_runner, 74, "test_process_one_slice" ) {}
 void runTest() { suite_DeltaFirstOrderRegression_runner.test_process_one_slice(); }
} testDescription_DeltaFirstOrderRegression_runner_test_process_one_slice;

static class TestDescription_DeltaFirstOrderRegression_runner_test_process_two_slices : public CxxTest::RealTestDescription {
public:
 TestDescription_DeltaFirstOrderRegression_runner_test_process_two_slices() : CxxTest::RealTestDescription( Tests_DeltaFirstOrderRegression_runner, suiteDescription_DeltaFirstOrderRegression_runner, 117, "test_process_two_slices" ) {}
 void runTest() { suite_DeltaFirstOrderRegression_runner.test_process_two_slices(); }
} testDescription_DeltaFirstOrderRegression_runner_test_process_two_slices;

static class TestDescription_DeltaFirstOrderRegression_runner_test_process_single_sample_slices : public CxxTest::RealTestDescription {
public:
 TestDescription_DeltaFirstOrderRegression_runner_test_process_single_sample_slices() : CxxTest::RealTestDescription( Tests_DeltaFirstOrderRegression_runner, suiteDescription_DeltaFirstOrderRegression_runner, 166, "test_process_single_sample_slices" ) {}
 void runTest() { suite_DeltaFirstOrderRegression_runner.test_process_single_sample_slices(); }
} testDescription_DeltaFirstOrderRegression_runner_test_process_single_sample_slices;

#include <cxxtest/Root.cpp>
