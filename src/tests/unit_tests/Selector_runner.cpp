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
#include "TestSelector.h"

static Selector_runner suite_Selector_runner;

static CxxTest::List Tests_Selector_runner = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_Selector_runner( "TestSelector.h", 25, "Selector_runner", suite_Selector_runner, Tests_Selector_runner );

static class TestDescription_Selector_runner_test_all_input_copied_to_output_by_default : public CxxTest::RealTestDescription {
public:
 TestDescription_Selector_runner_test_all_input_copied_to_output_by_default() : CxxTest::RealTestDescription( Tests_Selector_runner, suiteDescription_Selector_runner, 66, "test_all_input_copied_to_output_by_default" ) {}
 void runTest() { suite_Selector_runner.test_all_input_copied_to_output_by_default(); }
} testDescription_Selector_runner_test_all_input_copied_to_output_by_default;

static class TestDescription_Selector_runner_test_disabling_an_observation_should_make_it_not_appear_in_the_output : public CxxTest::RealTestDescription {
public:
 TestDescription_Selector_runner_test_disabling_an_observation_should_make_it_not_appear_in_the_output() : CxxTest::RealTestDescription( Tests_Selector_runner, suiteDescription_Selector_runner, 84, "test_disabling_an_observation_should_make_it_not_appear_in_the_output" ) {}
 void runTest() { suite_Selector_runner.test_disabling_an_observation_should_make_it_not_appear_in_the_output(); }
} testDescription_Selector_runner_test_disabling_an_observation_should_make_it_not_appear_in_the_output;

static class TestDescription_Selector_runner_test_should_be_able_to_disable_and_reenable_an_observation : public CxxTest::RealTestDescription {
public:
 TestDescription_Selector_runner_test_should_be_able_to_disable_and_reenable_an_observation() : CxxTest::RealTestDescription( Tests_Selector_runner, suiteDescription_Selector_runner, 98, "test_should_be_able_to_disable_and_reenable_an_observation" ) {}
 void runTest() { suite_Selector_runner.test_should_be_able_to_disable_and_reenable_an_observation(); }
} testDescription_Selector_runner_test_should_be_able_to_disable_and_reenable_an_observation;

#include <cxxtest/Root.cpp>
