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
#include "TestAimSAI.h"

static AimSAI_runner suite_AimSAI_runner;

static CxxTest::List Tests_AimSAI_runner = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_AimSAI_runner( "TestAimSAI.h", 22, "AimSAI_runner", suite_AimSAI_runner, Tests_AimSAI_runner );

static class TestDescription_AimSAI_runner_test_wav_file : public CxxTest::RealTestDescription {
public:
 TestDescription_AimSAI_runner_test_wav_file() : CxxTest::RealTestDescription( Tests_AimSAI_runner, suiteDescription_AimSAI_runner, 40, "test_wav_file" ) {}
 void runTest() { suite_AimSAI_runner.test_wav_file(); }
} testDescription_AimSAI_runner_test_wav_file;

#include <cxxtest/Root.cpp>
