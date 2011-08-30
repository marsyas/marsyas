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
#include "TestAimPZFC.h"

static AimPZFC_runner suite_AimPZFC_runner;

static CxxTest::List Tests_AimPZFC_runner = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_AimPZFC_runner( "TestAimPZFC.h", 22, "AimPZFC_runner", suite_AimPZFC_runner, Tests_AimPZFC_runner );

static class TestDescription_AimPZFC_runner_test_one_window_1000hz : public CxxTest::RealTestDescription {
public:
 TestDescription_AimPZFC_runner_test_one_window_1000hz() : CxxTest::RealTestDescription( Tests_AimPZFC_runner, suiteDescription_AimPZFC_runner, 40, "test_one_window_1000hz" ) {}
 void runTest() { suite_AimPZFC_runner.test_one_window_1000hz(); }
} testDescription_AimPZFC_runner_test_one_window_1000hz;

static class TestDescription_AimPZFC_runner_test_wav_file : public CxxTest::RealTestDescription {
public:
 TestDescription_AimPZFC_runner_test_wav_file() : CxxTest::RealTestDescription( Tests_AimPZFC_runner, suiteDescription_AimPZFC_runner, 84, "test_wav_file" ) {}
 void runTest() { suite_AimPZFC_runner.test_wav_file(); }
} testDescription_AimPZFC_runner_test_wav_file;

#include <cxxtest/Root.cpp>
