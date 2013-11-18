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
#include "TestAimLocalMax.h"

static AimLocalMax_runner suite_AimLocalMax_runner;

static CxxTest::List Tests_AimLocalMax_runner = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_AimLocalMax_runner( "TestAimLocalMax.h", 20, "AimLocalMax_runner", suite_AimLocalMax_runner, Tests_AimLocalMax_runner );

static class TestDescription_AimLocalMax_runner_test_wav_file : public CxxTest::RealTestDescription {
public:
 TestDescription_AimLocalMax_runner_test_wav_file() : CxxTest::RealTestDescription( Tests_AimLocalMax_runner, suiteDescription_AimLocalMax_runner, 38, "test_wav_file" ) {}
 void runTest() { suite_AimLocalMax_runner.test_wav_file(); }
} testDescription_AimLocalMax_runner_test_wav_file;

#include <cxxtest/Root.cpp>
