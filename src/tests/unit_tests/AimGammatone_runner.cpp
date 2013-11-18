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
#include "TestAimGammatone.h"

static AimGammatone_runner suite_AimGammatone_runner;

static CxxTest::List Tests_AimGammatone_runner = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_AimGammatone_runner( "TestAimGammatone.h", 20, "AimGammatone_runner", suite_AimGammatone_runner, Tests_AimGammatone_runner );

static class TestDescription_AimGammatone_runner_test_one_window_1000hz : public CxxTest::RealTestDescription {
public:
 TestDescription_AimGammatone_runner_test_one_window_1000hz() : CxxTest::RealTestDescription( Tests_AimGammatone_runner, suiteDescription_AimGammatone_runner, 32, "test_one_window_1000hz" ) {}
 void runTest() { suite_AimGammatone_runner.test_one_window_1000hz(); }
} testDescription_AimGammatone_runner_test_one_window_1000hz;

static class TestDescription_AimGammatone_runner_test_wav_file : public CxxTest::RealTestDescription {
public:
 TestDescription_AimGammatone_runner_test_wav_file() : CxxTest::RealTestDescription( Tests_AimGammatone_runner, suiteDescription_AimGammatone_runner, 75, "test_wav_file" ) {}
 void runTest() { suite_AimGammatone_runner.test_wav_file(); }
} testDescription_AimGammatone_runner_test_wav_file;

#include <cxxtest/Root.cpp>
