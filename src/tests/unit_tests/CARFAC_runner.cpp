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
#include "TestCARFAC.h"

static CARFAC_runner suite_CARFAC_runner;

static CxxTest::List Tests_CARFAC_runner = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_CARFAC_runner( "TestCARFAC.h", 22, "CARFAC_runner", suite_CARFAC_runner, Tests_CARFAC_runner );

static class TestDescription_CARFAC_runner_test_wav_file : public CxxTest::RealTestDescription {
public:
 TestDescription_CARFAC_runner_test_wav_file() : CxxTest::RealTestDescription( Tests_CARFAC_runner, suiteDescription_CARFAC_runner, 40, "test_wav_file" ) {}
 void runTest() { suite_CARFAC_runner.test_wav_file(); }
} testDescription_CARFAC_runner_test_wav_file;

#include <cxxtest/Root.cpp>
