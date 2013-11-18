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
#include "TestSum.h"

static Sum_runner suite_Sum_runner;

static CxxTest::List Tests_Sum_runner = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_Sum_runner( "TestSum.h", 24, "Sum_runner", suite_Sum_runner, Tests_Sum_runner );

static class TestDescription_Sum_runner_test_orig_sum : public CxxTest::RealTestDescription {
public:
 TestDescription_Sum_runner_test_orig_sum() : CxxTest::RealTestDescription( Tests_Sum_runner, suiteDescription_Sum_runner, 42, "test_orig_sum" ) {}
 void runTest() { suite_Sum_runner.test_orig_sum(); }
} testDescription_Sum_runner_test_orig_sum;

static class TestDescription_Sum_runner_test_add_up_observations : public CxxTest::RealTestDescription {
public:
 TestDescription_Sum_runner_test_add_up_observations() : CxxTest::RealTestDescription( Tests_Sum_runner, suiteDescription_Sum_runner, 88, "test_add_up_observations" ) {}
 void runTest() { suite_Sum_runner.test_add_up_observations(); }
} testDescription_Sum_runner_test_add_up_observations;

static class TestDescription_Sum_runner_test_add_up_samples : public CxxTest::RealTestDescription {
public:
 TestDescription_Sum_runner_test_add_up_samples() : CxxTest::RealTestDescription( Tests_Sum_runner, suiteDescription_Sum_runner, 143, "test_add_up_samples" ) {}
 void runTest() { suite_Sum_runner.test_add_up_samples(); }
} testDescription_Sum_runner_test_add_up_samples;

static class TestDescription_Sum_runner_test_add_up_whole : public CxxTest::RealTestDescription {
public:
 TestDescription_Sum_runner_test_add_up_whole() : CxxTest::RealTestDescription( Tests_Sum_runner, suiteDescription_Sum_runner, 198, "test_add_up_whole" ) {}
 void runTest() { suite_Sum_runner.test_add_up_whole(); }
} testDescription_Sum_runner_test_add_up_whole;

#include <cxxtest/Root.cpp>
