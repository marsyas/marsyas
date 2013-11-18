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
#include "TestWekaSource.h"

static WekaSource_runner suite_WekaSource_runner;

static CxxTest::List Tests_WekaSource_runner = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_WekaSource_runner( "TestWekaSource.h", 22, "WekaSource_runner", suite_WekaSource_runner, Tests_WekaSource_runner );

static class TestDescription_WekaSource_runner_test_pass : public CxxTest::RealTestDescription {
public:
 TestDescription_WekaSource_runner_test_pass() : CxxTest::RealTestDescription( Tests_WekaSource_runner, suiteDescription_WekaSource_runner, 36, "test_pass" ) {}
 void runTest() { suite_WekaSource_runner.test_pass(); }
} testDescription_WekaSource_runner_test_pass;

static class TestDescription_WekaSource_runner_test_memory_fold : public CxxTest::RealTestDescription {
public:
 TestDescription_WekaSource_runner_test_memory_fold() : CxxTest::RealTestDescription( Tests_WekaSource_runner, suiteDescription_WekaSource_runner, 56, "test_memory_fold" ) {}
 void runTest() { suite_WekaSource_runner.test_memory_fold(); }
} testDescription_WekaSource_runner_test_memory_fold;

#include <cxxtest/Root.cpp>
