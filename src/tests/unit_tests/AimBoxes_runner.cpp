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
#include "TestAimBoxes.h"

static AimBoxes_runner suite_AimBoxes_runner;

static CxxTest::List Tests_AimBoxes_runner = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_AimBoxes_runner( "TestAimBoxes.h", 20, "AimBoxes_runner", suite_AimBoxes_runner, Tests_AimBoxes_runner );

static class TestDescription_AimBoxes_runner_test_wav_file : public CxxTest::RealTestDescription {
public:
 TestDescription_AimBoxes_runner_test_wav_file() : CxxTest::RealTestDescription( Tests_AimBoxes_runner, suiteDescription_AimBoxes_runner, 33, "test_wav_file" ) {}
 void runTest() { suite_AimBoxes_runner.test_wav_file(); }
} testDescription_AimBoxes_runner_test_wav_file;

#include <cxxtest/Root.cpp>
