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
CxxTest::StaticSuiteDescription suiteDescription_RunningAutocorrelation_runner( "TestRunningAutocorrelation.h", 62, "RunningAutocorrelation_runner", suite_RunningAutocorrelation_runner, Tests_RunningAutocorrelation_runner );

static class TestDescription_RunningAutocorrelation_runner_test_output_flow : public CxxTest::RealTestDescription {
public:
 TestDescription_RunningAutocorrelation_runner_test_output_flow() : CxxTest::RealTestDescription( Tests_RunningAutocorrelation_runner, suiteDescription_RunningAutocorrelation_runner, 112, "test_output_flow" ) {}
 void runTest() { suite_RunningAutocorrelation_runner.test_output_flow(); }
} testDescription_RunningAutocorrelation_runner_test_output_flow;

static class TestDescription_RunningAutocorrelation_runner_test_one_slice_single_row : public CxxTest::RealTestDescription {
public:
 TestDescription_RunningAutocorrelation_runner_test_one_slice_single_row() : CxxTest::RealTestDescription( Tests_RunningAutocorrelation_runner, suiteDescription_RunningAutocorrelation_runner, 130, "test_one_slice_single_row" ) {}
 void runTest() { suite_RunningAutocorrelation_runner.test_one_slice_single_row(); }
} testDescription_RunningAutocorrelation_runner_test_one_slice_single_row;

static class TestDescription_RunningAutocorrelation_runner_test_one_slice_multirow : public CxxTest::RealTestDescription {
public:
 TestDescription_RunningAutocorrelation_runner_test_one_slice_multirow() : CxxTest::RealTestDescription( Tests_RunningAutocorrelation_runner, suiteDescription_RunningAutocorrelation_runner, 164, "test_one_slice_multirow" ) {}
 void runTest() { suite_RunningAutocorrelation_runner.test_one_slice_multirow(); }
} testDescription_RunningAutocorrelation_runner_test_one_slice_multirow;

static class TestDescription_RunningAutocorrelation_runner_test_two_slices_single_row : public CxxTest::RealTestDescription {
public:
 TestDescription_RunningAutocorrelation_runner_test_two_slices_single_row() : CxxTest::RealTestDescription( Tests_RunningAutocorrelation_runner, suiteDescription_RunningAutocorrelation_runner, 205, "test_two_slices_single_row" ) {}
 void runTest() { suite_RunningAutocorrelation_runner.test_two_slices_single_row(); }
} testDescription_RunningAutocorrelation_runner_test_two_slices_single_row;

static class TestDescription_RunningAutocorrelation_runner_test_multiple_slices_multirow : public CxxTest::RealTestDescription {
public:
 TestDescription_RunningAutocorrelation_runner_test_multiple_slices_multirow() : CxxTest::RealTestDescription( Tests_RunningAutocorrelation_runner, suiteDescription_RunningAutocorrelation_runner, 249, "test_multiple_slices_multirow" ) {}
 void runTest() { suite_RunningAutocorrelation_runner.test_multiple_slices_multirow(); }
} testDescription_RunningAutocorrelation_runner_test_multiple_slices_multirow;

static class TestDescription_RunningAutocorrelation_runner_test_maxlag_larger_than_slice_size : public CxxTest::RealTestDescription {
public:
 TestDescription_RunningAutocorrelation_runner_test_maxlag_larger_than_slice_size() : CxxTest::RealTestDescription( Tests_RunningAutocorrelation_runner, suiteDescription_RunningAutocorrelation_runner, 286, "test_maxlag_larger_than_slice_size" ) {}
 void runTest() { suite_RunningAutocorrelation_runner.test_maxlag_larger_than_slice_size(); }
} testDescription_RunningAutocorrelation_runner_test_maxlag_larger_than_slice_size;

static class TestDescription_RunningAutocorrelation_runner_test_observation_names : public CxxTest::RealTestDescription {
public:
 TestDescription_RunningAutocorrelation_runner_test_observation_names() : CxxTest::RealTestDescription( Tests_RunningAutocorrelation_runner, suiteDescription_RunningAutocorrelation_runner, 323, "test_observation_names" ) {}
 void runTest() { suite_RunningAutocorrelation_runner.test_observation_names(); }
} testDescription_RunningAutocorrelation_runner_test_observation_names;

static class TestDescription_RunningAutocorrelation_runner_test_observation_names_with_unfoldToObservations : public CxxTest::RealTestDescription {
public:
 TestDescription_RunningAutocorrelation_runner_test_observation_names_with_unfoldToObservations() : CxxTest::RealTestDescription( Tests_RunningAutocorrelation_runner, suiteDescription_RunningAutocorrelation_runner, 332, "test_observation_names_with_unfoldToObservations" ) {}
 void runTest() { suite_RunningAutocorrelation_runner.test_observation_names_with_unfoldToObservations(); }
} testDescription_RunningAutocorrelation_runner_test_observation_names_with_unfoldToObservations;

static class TestDescription_RunningAutocorrelation_runner_test_observation_names_with_unfoldToObservations_normalize : public CxxTest::RealTestDescription {
public:
 TestDescription_RunningAutocorrelation_runner_test_observation_names_with_unfoldToObservations_normalize() : CxxTest::RealTestDescription( Tests_RunningAutocorrelation_runner, suiteDescription_RunningAutocorrelation_runner, 341, "test_observation_names_with_unfoldToObservations_normalize" ) {}
 void runTest() { suite_RunningAutocorrelation_runner.test_observation_names_with_unfoldToObservations_normalize(); }
} testDescription_RunningAutocorrelation_runner_test_observation_names_with_unfoldToObservations_normalize;

static class TestDescription_RunningAutocorrelation_runner_test_observation_names_with_unfoldToObservations_normalize_but_not_lag0 : public CxxTest::RealTestDescription {
public:
 TestDescription_RunningAutocorrelation_runner_test_observation_names_with_unfoldToObservations_normalize_but_not_lag0() : CxxTest::RealTestDescription( Tests_RunningAutocorrelation_runner, suiteDescription_RunningAutocorrelation_runner, 351, "test_observation_names_with_unfoldToObservations_normalize_but_not_lag0" ) {}
 void runTest() { suite_RunningAutocorrelation_runner.test_observation_names_with_unfoldToObservations_normalize_but_not_lag0(); }
} testDescription_RunningAutocorrelation_runner_test_observation_names_with_unfoldToObservations_normalize_but_not_lag0;

static class TestDescription_RunningAutocorrelation_runner_test_flow_change : public CxxTest::RealTestDescription {
public:
 TestDescription_RunningAutocorrelation_runner_test_flow_change() : CxxTest::RealTestDescription( Tests_RunningAutocorrelation_runner, suiteDescription_RunningAutocorrelation_runner, 365, "test_flow_change" ) {}
 void runTest() { suite_RunningAutocorrelation_runner.test_flow_change(); }
} testDescription_RunningAutocorrelation_runner_test_flow_change;

static class TestDescription_RunningAutocorrelation_runner_test_normalization_but_not_for_lag0 : public CxxTest::RealTestDescription {
public:
 TestDescription_RunningAutocorrelation_runner_test_normalization_but_not_for_lag0() : CxxTest::RealTestDescription( Tests_RunningAutocorrelation_runner, suiteDescription_RunningAutocorrelation_runner, 425, "test_normalization_but_not_for_lag0" ) {}
 void runTest() { suite_RunningAutocorrelation_runner.test_normalization_but_not_for_lag0(); }
} testDescription_RunningAutocorrelation_runner_test_normalization_but_not_for_lag0;

static class TestDescription_RunningAutocorrelation_runner_test_normalization_of_zero_input : public CxxTest::RealTestDescription {
public:
 TestDescription_RunningAutocorrelation_runner_test_normalization_of_zero_input() : CxxTest::RealTestDescription( Tests_RunningAutocorrelation_runner, suiteDescription_RunningAutocorrelation_runner, 433, "test_normalization_of_zero_input" ) {}
 void runTest() { suite_RunningAutocorrelation_runner.test_normalization_of_zero_input(); }
} testDescription_RunningAutocorrelation_runner_test_normalization_of_zero_input;

static class TestDescription_RunningAutocorrelation_runner_test_internal_buffer_clearing : public CxxTest::RealTestDescription {
public:
 TestDescription_RunningAutocorrelation_runner_test_internal_buffer_clearing() : CxxTest::RealTestDescription( Tests_RunningAutocorrelation_runner, suiteDescription_RunningAutocorrelation_runner, 458, "test_internal_buffer_clearing" ) {}
 void runTest() { suite_RunningAutocorrelation_runner.test_internal_buffer_clearing(); }
} testDescription_RunningAutocorrelation_runner_test_internal_buffer_clearing;

static class TestDescription_RunningAutocorrelation_runner_test_unfoldToObservations_normalize : public CxxTest::RealTestDescription {
public:
 TestDescription_RunningAutocorrelation_runner_test_unfoldToObservations_normalize() : CxxTest::RealTestDescription( Tests_RunningAutocorrelation_runner, suiteDescription_RunningAutocorrelation_runner, 536, "test_unfoldToObservations_normalize" ) {}
 void runTest() { suite_RunningAutocorrelation_runner.test_unfoldToObservations_normalize(); }
} testDescription_RunningAutocorrelation_runner_test_unfoldToObservations_normalize;

static class TestDescription_RunningAutocorrelation_runner_test_unfoldToObservations_normalize_but_not_for_lag0 : public CxxTest::RealTestDescription {
public:
 TestDescription_RunningAutocorrelation_runner_test_unfoldToObservations_normalize_but_not_for_lag0() : CxxTest::RealTestDescription( Tests_RunningAutocorrelation_runner, suiteDescription_RunningAutocorrelation_runner, 541, "test_unfoldToObservations_normalize_but_not_for_lag0" ) {}
 void runTest() { suite_RunningAutocorrelation_runner.test_unfoldToObservations_normalize_but_not_for_lag0(); }
} testDescription_RunningAutocorrelation_runner_test_unfoldToObservations_normalize_but_not_for_lag0;

#include <cxxtest/Root.cpp>
