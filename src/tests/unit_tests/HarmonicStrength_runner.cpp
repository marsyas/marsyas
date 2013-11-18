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
#include "TestHarmonicStrength.h"

static HarmonicStrength_runner suite_HarmonicStrength_runner;

static CxxTest::List Tests_HarmonicStrength_runner = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_HarmonicStrength_runner( "TestHarmonicStrength.h", 13, "HarmonicStrength_runner", suite_HarmonicStrength_runner, Tests_HarmonicStrength_runner );

static class TestDescription_HarmonicStrength_runner_test_process : public CxxTest::RealTestDescription {
public:
 TestDescription_HarmonicStrength_runner_test_process() : CxxTest::RealTestDescription( Tests_HarmonicStrength_runner, suiteDescription_HarmonicStrength_runner, 31, "test_process" ) {}
 void runTest() { suite_HarmonicStrength_runner.test_process(); }
} testDescription_HarmonicStrength_runner_test_process;

#include <cxxtest/Root.cpp>
