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
#include "TestRunningAutocorrelation.h"

static RunningAutocorrelation_runner suite_RunningAutocorrelation_runner;

static CxxTest::List Tests_RunningAutocorrelation_runner = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_RunningAutocorrelation_runner( "TestRunningAutocorrelation.h", 61, "RunningAutocorrelation_runner", suite_RunningAutocorrelation_runner, Tests_RunningAutocorrelation_runner );

static class TestDescription_RunningAutocorrelation_runner_test_output_flow : public CxxTest::RealTestDescription {
public:
 TestDescription_RunningAutocorrelation_runner_test_output_flow() : CxxTest::RealTestDescription( Tests_RunningAutocorrelation_runner, suiteDescription_RunningAutocorrelation_runner, 96, "test_output_flow" ) {}
 void runTest() { suite_RunningAutocorrelation_runner.test_output_flow(); }
} testDescription_RunningAutocorrelation_runner_test_output_flow;

static class TestDescription_RunningAutocorrelation_runner_test_one_slice_single_row : public CxxTest::RealTestDescription {
public:
 TestDescription_RunningAutocorrelation_runner_test_one_slice_single_row() : CxxTest::RealTestDescription( Tests_RunningAutocorrelation_runner, suiteDescription_RunningAutocorrelation_runner, 114, "test_one_slice_single_row" ) {}
 void runTest() { suite_RunningAutocorrelation_runner.test_one_slice_single_row(); }
} testDescription_RunningAutocorrelation_runner_test_one_slice_single_row;

static class TestDescription_RunningAutocorrelation_runner_test_one_slice_multirow : public CxxTest::RealTestDescription {
public:
 TestDescription_RunningAutocorrelation_runner_test_one_slice_multirow() : CxxTest::RealTestDescription( Tests_RunningAutocorrelation_runner, suiteDescription_RunningAutocorrelation_runner, 148, "test_one_slice_multirow" ) {}
 void runTest() { suite_RunningAutocorrelation_runner.test_one_slice_multirow(); }
} testDescription_RunningAutocorrelation_runner_test_one_slice_multirow;

static class TestDescription_RunningAutocorrelation_runner_test_two_slices_single_row : public CxxTest::RealTestDescription {
public:
 TestDescription_RunningAutocorrelation_runner_test_two_slices_single_row() : CxxTest::RealTestDescription( Tests_RunningAutocorrelation_runner, suiteDescription_RunningAutocorrelation_runner, 189, "test_two_slices_single_row" ) {}
 void runTest() { suite_RunningAutocorrelation_runner.test_two_slices_single_row(); }
} testDescription_RunningAutocorrelation_runner_test_two_slices_single_row;

static class TestDescription_RunningAutocorrelation_runner_test_multiple_slices_multirow : public CxxTest::RealTestDescription {
public:
 TestDescription_RunningAutocorrelation_runner_test_multiple_slices_multirow() : CxxTest::RealTestDescription( Tests_RunningAutocorrelation_runner, suiteDescription_RunningAutocorrelation_runner, 233, "test_multiple_slices_multirow" ) {}
 void runTest() { suite_RunningAutocorrelation_runner.test_multiple_slices_multirow(); }
} testDescription_RunningAutocorrelation_runner_test_multiple_slices_multirow;

static class TestDescription_RunningAutocorrelation_runner_test_maxlag_larger_than_slice_size : public CxxTest::RealTestDescription {
public:
 TestDescription_RunningAutocorrelation_runner_test_maxlag_larger_than_slice_size() : CxxTest::RealTestDescription( Tests_RunningAutocorrelation_runner, suiteDescription_RunningAutocorrelation_runner, 270, "test_maxlag_larger_than_slice_size" ) {}
 void runTest() { suite_RunningAutocorrelation_runner.test_maxlag_larger_than_slice_size(); }
} testDescription_RunningAutocorrelation_runner_test_maxlag_larger_than_slice_size;

static class TestDescription_RunningAutocorrelation_runner_test_observation_names : public CxxTest::RealTestDescription {
public:
 TestDescription_RunningAutocorrelation_runner_test_observation_names() : CxxTest::RealTestDescription( Tests_RunningAutocorrelation_runner, suiteDescription_RunningAutocorrelation_runner, 307, "test_observation_names" ) {}
 void runTest() { suite_RunningAutocorrelation_runner.test_observation_names(); }
} testDescription_RunningAutocorrelation_runner_test_observation_names;

static class TestDescription_RunningAutocorrelation_runner_test_flow_change : public CxxTest::RealTestDescription {
public:
 TestDescription_RunningAutocorrelation_runner_test_flow_change() : CxxTest::RealTestDescription( Tests_RunningAutocorrelation_runner, suiteDescription_RunningAutocorrelation_runner, 319, "test_flow_change" ) {}
 void runTest() { suite_RunningAutocorrelation_runner.test_flow_change(); }
} testDescription_RunningAutocorrelation_runner_test_flow_change;

static class TestDescription_RunningAutocorrelation_runner_test_normalization : public CxxTest::RealTestDescription {
public:
 TestDescription_RunningAutocorrelation_runner_test_normalization() : CxxTest::RealTestDescription( Tests_RunningAutocorrelation_runner, suiteDescription_RunningAutocorrelation_runner, 338, "test_normalization" ) {}
 void runTest() { suite_RunningAutocorrelation_runner.test_normalization(); }
} testDescription_RunningAutocorrelation_runner_test_normalization;

static class TestDescription_RunningAutocorrelation_runner_test_normalization_of_zero_input : public CxxTest::RealTestDescription {
public:
 TestDescription_RunningAutocorrelation_runner_test_normalization_of_zero_input() : CxxTest::RealTestDescription( Tests_RunningAutocorrelation_runner, suiteDescription_RunningAutocorrelation_runner, 377, "test_normalization_of_zero_input" ) {}
 void runTest() { suite_RunningAutocorrelation_runner.test_normalization_of_zero_input(); }
} testDescription_RunningAutocorrelation_runner_test_normalization_of_zero_input;

static class TestDescription_RunningAutocorrelation_runner_test_do_not_normalize_for_lag_0 : public CxxTest::RealTestDescription {
public:
 TestDescription_RunningAutocorrelation_runner_test_do_not_normalize_for_lag_0() : CxxTest::RealTestDescription( Tests_RunningAutocorrelation_runner, suiteDescription_RunningAutocorrelation_runner, 403, "test_do_not_normalize_for_lag_0" ) {}
 void runTest() { suite_RunningAutocorrelation_runner.test_do_not_normalize_for_lag_0(); }
} testDescription_RunningAutocorrelation_runner_test_do_not_normalize_for_lag_0;

static class TestDescription_RunningAutocorrelation_runner_test_internal_buffer_clearing : public CxxTest::RealTestDescription {
public:
 TestDescription_RunningAutocorrelation_runner_test_internal_buffer_clearing() : CxxTest::RealTestDescription( Tests_RunningAutocorrelation_runner, suiteDescription_RunningAutocorrelation_runner, 447, "test_internal_buffer_clearing" ) {}
 void runTest() { suite_RunningAutocorrelation_runner.test_internal_buffer_clearing(); }
} testDescription_RunningAutocorrelation_runner_test_internal_buffer_clearing;

#include <cxxtest/Root.cpp>
