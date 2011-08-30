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
#include "TestDelaySamples.h"

static DelaySamples_runner suite_DelaySamples_runner;

static CxxTest::List Tests_DelaySamples_runner = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_DelaySamples_runner( "TestDelaySamples.h", 16, "DelaySamples_runner", suite_DelaySamples_runner, Tests_DelaySamples_runner );

static class TestDescription_DelaySamples_runner_test_flow_settings : public CxxTest::RealTestDescription {
public:
 TestDescription_DelaySamples_runner_test_flow_settings() : CxxTest::RealTestDescription( Tests_DelaySamples_runner, suiteDescription_DelaySamples_runner, 67, "test_flow_settings" ) {}
 void runTest() { suite_DelaySamples_runner.test_flow_settings(); }
} testDescription_DelaySamples_runner_test_flow_settings;

static class TestDescription_DelaySamples_runner_test_process_basic : public CxxTest::RealTestDescription {
public:
 TestDescription_DelaySamples_runner_test_process_basic() : CxxTest::RealTestDescription( Tests_DelaySamples_runner, suiteDescription_DelaySamples_runner, 108, "test_process_basic" ) {}
 void runTest() { suite_DelaySamples_runner.test_process_basic(); }
} testDescription_DelaySamples_runner_test_process_basic;

static class TestDescription_DelaySamples_runner_test_process_one_channel_multiple_slices_small_delay : public CxxTest::RealTestDescription {
public:
 TestDescription_DelaySamples_runner_test_process_one_channel_multiple_slices_small_delay() : CxxTest::RealTestDescription( Tests_DelaySamples_runner, suiteDescription_DelaySamples_runner, 112, "test_process_one_channel_multiple_slices_small_delay" ) {}
 void runTest() { suite_DelaySamples_runner.test_process_one_channel_multiple_slices_small_delay(); }
} testDescription_DelaySamples_runner_test_process_one_channel_multiple_slices_small_delay;

static class TestDescription_DelaySamples_runner_test_process_one_channel_multiple_slices_large_delay : public CxxTest::RealTestDescription {
public:
 TestDescription_DelaySamples_runner_test_process_one_channel_multiple_slices_large_delay() : CxxTest::RealTestDescription( Tests_DelaySamples_runner, suiteDescription_DelaySamples_runner, 117, "test_process_one_channel_multiple_slices_large_delay" ) {}
 void runTest() { suite_DelaySamples_runner.test_process_one_channel_multiple_slices_large_delay(); }
} testDescription_DelaySamples_runner_test_process_one_channel_multiple_slices_large_delay;

static class TestDescription_DelaySamples_runner_test_process_multiple_slices_small_delay : public CxxTest::RealTestDescription {
public:
 TestDescription_DelaySamples_runner_test_process_multiple_slices_small_delay() : CxxTest::RealTestDescription( Tests_DelaySamples_runner, suiteDescription_DelaySamples_runner, 122, "test_process_multiple_slices_small_delay" ) {}
 void runTest() { suite_DelaySamples_runner.test_process_multiple_slices_small_delay(); }
} testDescription_DelaySamples_runner_test_process_multiple_slices_small_delay;

static class TestDescription_DelaySamples_runner_test_process_multiple_slices_large_delay : public CxxTest::RealTestDescription {
public:
 TestDescription_DelaySamples_runner_test_process_multiple_slices_large_delay() : CxxTest::RealTestDescription( Tests_DelaySamples_runner, suiteDescription_DelaySamples_runner, 127, "test_process_multiple_slices_large_delay" ) {}
 void runTest() { suite_DelaySamples_runner.test_process_multiple_slices_large_delay(); }
} testDescription_DelaySamples_runner_test_process_multiple_slices_large_delay;

static class TestDescription_DelaySamples_runner_test_process_multiple_slices_zero_delay : public CxxTest::RealTestDescription {
public:
 TestDescription_DelaySamples_runner_test_process_multiple_slices_zero_delay() : CxxTest::RealTestDescription( Tests_DelaySamples_runner, suiteDescription_DelaySamples_runner, 132, "test_process_multiple_slices_zero_delay" ) {}
 void runTest() { suite_DelaySamples_runner.test_process_multiple_slices_zero_delay(); }
} testDescription_DelaySamples_runner_test_process_multiple_slices_zero_delay;

static class TestDescription_DelaySamples_runner_test_process_negative_delay : public CxxTest::RealTestDescription {
public:
 TestDescription_DelaySamples_runner_test_process_negative_delay() : CxxTest::RealTestDescription( Tests_DelaySamples_runner, suiteDescription_DelaySamples_runner, 137, "test_process_negative_delay" ) {}
 void runTest() { suite_DelaySamples_runner.test_process_negative_delay(); }
} testDescription_DelaySamples_runner_test_process_negative_delay;

#include <cxxtest/Root.cpp>
