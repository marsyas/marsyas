// TestSelector.h

//
// The Selector class selects one or more of the input observations
// and copies them to the output.
//

//
// (c) 2009 - sness@sness.net - GPL
//

#include <cxxtest/TestSuite.h>

#include <cstdio>
#include <marsyas/Collection.h>
#include <marsyas/system/MarSystemManager.h>
#include <marsyas/marsystems/Selector.h>
#include <marsyas/CommandLineOptions.h>

#include <vector>

using namespace std;
using namespace Marsyas;

class Selector_runner : public CxxTest::TestSuite
{
public:

  const static int numObservations = 5;
  const	static int numSamples = 1;

  realvec in,out;
  MarSystemManager mng;
  Selector *selector;

  void
  setUp()
  {

    selector = new Selector("selector");
    selector->updControl("mrs_natural/inSamples", numSamples);
    selector->updControl("mrs_natural/inObservations", numObservations);

    // Setup an input realvec for data to feed into the Marsystem
    in.create(0.0,numObservations,numSamples);
    in(0,0) = 0.1;
    in(1,0) = 1.1;
    in(2,0) = 2.1;
    in(3,0) = 3.1;
    in(4,0) = 4.1;

    // Create the output realvec
    out.create(numObservations,numSamples);
  }

  // "destructor"
  void tearDown()
  {
    delete selector;
  }


  //
  // By default, all input should be copied to the output unchanged
  //
  void test_all_input_copied_to_output_by_default(void)
  {
    selector->myProcess(in,out);

    // Make sure the output data is the same as the input data
    TS_ASSERT_EQUALS(out.getRows(), 5);
    TS_ASSERT_EQUALS(out.getCols(), 1);

    TS_ASSERT_EQUALS(out(0,0), 0.1);
    TS_ASSERT_EQUALS(out(1,0), 1.1);
    TS_ASSERT_EQUALS(out(2,0), 2.1);
    TS_ASSERT_EQUALS(out(3,0), 3.1);
    TS_ASSERT_EQUALS(out(4,0), 4.1);
  }

  //
  // If you disable an observation, it should not get copied to output
  //
  void test_disabling_an_observation_should_make_it_not_appear_in_the_output(void)
  {
    selector->updControl("mrs_natural/disable", 2);
    selector->myProcess(in,out);

    TS_ASSERT_EQUALS(out(0,0), 0.1);
    TS_ASSERT_EQUALS(out(1,0), 1.1);
    TS_ASSERT_EQUALS(out(2,0), 3.1);
    TS_ASSERT_EQUALS(out(3,0), 4.1);
  }

  //
  // If you disable an observation, it should not get copied to output
  //
  void test_should_be_able_to_disable_and_reenable_an_observation(void)
  {
    selector->updControl("mrs_natural/disable", 2);
    selector->updControl("mrs_natural/disable", 3);
    selector->updControl("mrs_natural/enable", 2);
    selector->myProcess(in,out);

    TS_ASSERT_EQUALS(out(0,0), 0.1);
    TS_ASSERT_EQUALS(out(1,0), 1.1);
    TS_ASSERT_EQUALS(out(2,0), 2.1);
    TS_ASSERT_EQUALS(out(3,0), 4.1);
  }




};

