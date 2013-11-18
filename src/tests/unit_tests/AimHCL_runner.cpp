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
#include "TestAimHCL.h"

static AimHCL_runner suite_AimHCL_runner;

static CxxTest::List Tests_AimHCL_runner = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_AimHCL_runner( "TestAimHCL.h", 20, "AimHCL_runner", suite_AimHCL_runner, Tests_AimHCL_runner );

static class TestDescription_AimHCL_runner_test_wav_file : public CxxTest::RealTestDescription {
public:
 TestDescription_AimHCL_runner_test_wav_file() : CxxTest::RealTestDescription( Tests_AimHCL_runner, suiteDescription_AimHCL_runner, 44, "test_wav_file" ) {}
 void runTest() { suite_AimHCL_runner.test_wav_file(); }
} testDescription_AimHCL_runner_test_wav_file;

#include <cxxtest/Root.cpp>
