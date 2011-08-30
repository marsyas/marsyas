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
#include "TestRunningStatistics.h"

static RunningStatistics_runner suite_RunningStatistics_runner;

static CxxTest::List Tests_RunningStatistics_runner = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_RunningStatistics_runner( "TestRunningStatistics.h", 21, "RunningStatistics_runner", suite_RunningStatistics_runner, Tests_RunningStatistics_runner );

static class TestDescription_RunningStatistics_runner_test_default_flow_settings : public CxxTest::RealTestDescription {
public:
 TestDescription_RunningStatistics_runner_test_default_flow_settings() : CxxTest::RealTestDescription( Tests_RunningStatistics_runner, suiteDescription_RunningStatistics_runner, 63, "test_default_flow_settings" ) {}
 void runTest() { suite_RunningStatistics_runner.test_default_flow_settings(); }
} testDescription_RunningStatistics_runner_test_default_flow_settings;

static class TestDescription_RunningStatistics_runner_test_flow_settings : public CxxTest::RealTestDescription {
public:
 TestDescription_RunningStatistics_runner_test_flow_settings() : CxxTest::RealTestDescription( Tests_RunningStatistics_runner, suiteDescription_RunningStatistics_runner, 112, "test_flow_settings" ) {}
 void runTest() { suite_RunningStatistics_runner.test_flow_settings(); }
} testDescription_RunningStatistics_runner_test_flow_settings;

static class TestDescription_RunningStatistics_runner_test_process_mean : public CxxTest::RealTestDescription {
public:
 TestDescription_RunningStatistics_runner_test_process_mean() : CxxTest::RealTestDescription( Tests_RunningStatistics_runner, suiteDescription_RunningStatistics_runner, 165, "test_process_mean" ) {}
 void runTest() { suite_RunningStatistics_runner.test_process_mean(); }
} testDescription_RunningStatistics_runner_test_process_mean;

static class TestDescription_RunningStatistics_runner_test_process_stddev : public CxxTest::RealTestDescription {
public:
 TestDescription_RunningStatistics_runner_test_process_stddev() : CxxTest::RealTestDescription( Tests_RunningStatistics_runner, suiteDescription_RunningStatistics_runner, 184, "test_process_stddev" ) {}
 void runTest() { suite_RunningStatistics_runner.test_process_stddev(); }
} testDescription_RunningStatistics_runner_test_process_stddev;

static class TestDescription_RunningStatistics_runner_test_process_skewness : public CxxTest::RealTestDescription {
public:
 TestDescription_RunningStatistics_runner_test_process_skewness() : CxxTest::RealTestDescription( Tests_RunningStatistics_runner, suiteDescription_RunningStatistics_runner, 204, "test_process_skewness" ) {}
 void runTest() { suite_RunningStatistics_runner.test_process_skewness(); }
} testDescription_RunningStatistics_runner_test_process_skewness;

static class TestDescription_RunningStatistics_runner_test_process_mean_and_stddev : public CxxTest::RealTestDescription {
public:
 TestDescription_RunningStatistics_runner_test_process_mean_and_stddev() : CxxTest::RealTestDescription( Tests_RunningStatistics_runner, suiteDescription_RunningStatistics_runner, 221, "test_process_mean_and_stddev" ) {}
 void runTest() { suite_RunningStatistics_runner.test_process_mean_and_stddev(); }
} testDescription_RunningStatistics_runner_test_process_mean_and_stddev;

static class TestDescription_RunningStatistics_runner_test_process_stddev_and_skewness : public CxxTest::RealTestDescription {
public:
 TestDescription_RunningStatistics_runner_test_process_stddev_and_skewness() : CxxTest::RealTestDescription( Tests_RunningStatistics_runner, suiteDescription_RunningStatistics_runner, 239, "test_process_stddev_and_skewness" ) {}
 void runTest() { suite_RunningStatistics_runner.test_process_stddev_and_skewness(); }
} testDescription_RunningStatistics_runner_test_process_stddev_and_skewness;

static class TestDescription_RunningStatistics_runner_test_process_mean_and_stddev_and_skewness : public CxxTest::RealTestDescription {
public:
 TestDescription_RunningStatistics_runner_test_process_mean_and_stddev_and_skewness() : CxxTest::RealTestDescription( Tests_RunningStatistics_runner, suiteDescription_RunningStatistics_runner, 257, "test_process_mean_and_stddev_and_skewness" ) {}
 void runTest() { suite_RunningStatistics_runner.test_process_mean_and_stddev_and_skewness(); }
} testDescription_RunningStatistics_runner_test_process_mean_and_stddev_and_skewness;

static class TestDescription_RunningStatistics_runner_test_running_property : public CxxTest::RealTestDescription {
public:
 TestDescription_RunningStatistics_runner_test_running_property() : CxxTest::RealTestDescription( Tests_RunningStatistics_runner, suiteDescription_RunningStatistics_runner, 279, "test_running_property" ) {}
 void runTest() { suite_RunningStatistics_runner.test_running_property(); }
} testDescription_RunningStatistics_runner_test_running_property;

static class TestDescription_RunningStatistics_runner_test_clear : public CxxTest::RealTestDescription {
public:
 TestDescription_RunningStatistics_runner_test_clear() : CxxTest::RealTestDescription( Tests_RunningStatistics_runner, suiteDescription_RunningStatistics_runner, 337, "test_clear" ) {}
 void runTest() { suite_RunningStatistics_runner.test_clear(); }
} testDescription_RunningStatistics_runner_test_clear;

static class TestDescription_RunningStatistics_runner_test_clearPerTick : public CxxTest::RealTestDescription {
public:
 TestDescription_RunningStatistics_runner_test_clearPerTick() : CxxTest::RealTestDescription( Tests_RunningStatistics_runner, suiteDescription_RunningStatistics_runner, 398, "test_clearPerTick" ) {}
 void runTest() { suite_RunningStatistics_runner.test_clearPerTick(); }
} testDescription_RunningStatistics_runner_test_clearPerTick;

#include <cxxtest/Root.cpp>
