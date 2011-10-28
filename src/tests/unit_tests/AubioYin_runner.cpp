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
#include "TestAubioYin.h"

static AubioYin_runner suite_AubioYin_runner;

static CxxTest::List Tests_AubioYin_runner = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_AubioYin_runner( "TestAubioYin.h", 33, "AubioYin_runner", suite_AubioYin_runner, Tests_AubioYin_runner );

static class TestDescription_AubioYin_runner_test_one_window_1000hz : public CxxTest::RealTestDescription {
public:
 TestDescription_AubioYin_runner_test_one_window_1000hz() : CxxTest::RealTestDescription( Tests_AubioYin_runner, suiteDescription_AubioYin_runner, 57, "test_one_window_1000hz" ) {}
 void runTest() { suite_AubioYin_runner.test_one_window_1000hz(); }
} testDescription_AubioYin_runner_test_one_window_1000hz;

static class TestDescription_AubioYin_runner_test_ten_windows_1000hz : public CxxTest::RealTestDescription {
public:
 TestDescription_AubioYin_runner_test_ten_windows_1000hz() : CxxTest::RealTestDescription( Tests_AubioYin_runner, suiteDescription_AubioYin_runner, 84, "test_ten_windows_1000hz" ) {}
 void runTest() { suite_AubioYin_runner.test_ten_windows_1000hz(); }
} testDescription_AubioYin_runner_test_ten_windows_1000hz;

static class TestDescription_AubioYin_runner_test_ten_windows_1000hz_500hz : public CxxTest::RealTestDescription {
public:
 TestDescription_AubioYin_runner_test_ten_windows_1000hz_500hz() : CxxTest::RealTestDescription( Tests_AubioYin_runner, suiteDescription_AubioYin_runner, 151, "test_ten_windows_1000hz_500hz" ) {}
 void runTest() { suite_AubioYin_runner.test_ten_windows_1000hz_500hz(); }
} testDescription_AubioYin_runner_test_ten_windows_1000hz_500hz;

static class TestDescription_AubioYin_runner_test_ten_windows_1000_990_smooth_hz : public CxxTest::RealTestDescription {
public:
 TestDescription_AubioYin_runner_test_ten_windows_1000_990_smooth_hz() : CxxTest::RealTestDescription( Tests_AubioYin_runner, suiteDescription_AubioYin_runner, 224, "test_ten_windows_1000_990_smooth_hz" ) {}
 void runTest() { suite_AubioYin_runner.test_ten_windows_1000_990_smooth_hz(); }
} testDescription_AubioYin_runner_test_ten_windows_1000_990_smooth_hz;

static class TestDescription_AubioYin_runner_test_ten_windows_1000_880_smooth_hz : public CxxTest::RealTestDescription {
public:
 TestDescription_AubioYin_runner_test_ten_windows_1000_880_smooth_hz() : CxxTest::RealTestDescription( Tests_AubioYin_runner, suiteDescription_AubioYin_runner, 299, "test_ten_windows_1000_880_smooth_hz" ) {}
 void runTest() { suite_AubioYin_runner.test_ten_windows_1000_880_smooth_hz(); }
} testDescription_AubioYin_runner_test_ten_windows_1000_880_smooth_hz;

#include <cxxtest/Root.cpp>
