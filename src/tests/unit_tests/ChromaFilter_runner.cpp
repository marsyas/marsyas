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
#include "TestChromaFilter.h"

static Sum_runner suite_Sum_runner;

static CxxTest::List Tests_Sum_runner = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_Sum_runner( "TestChromaFilter.h", 23, "Sum_runner", suite_Sum_runner, Tests_Sum_runner );

static class TestDescription_Sum_runner_test_chroma : public CxxTest::RealTestDescription {
public:
 TestDescription_Sum_runner_test_chroma() : CxxTest::RealTestDescription( Tests_Sum_runner, suiteDescription_Sum_runner, 27, "test_chroma" ) {}
 void runTest() { suite_Sum_runner.test_chroma(); }
} testDescription_Sum_runner_test_chroma;

#include <cxxtest/Root.cpp>
