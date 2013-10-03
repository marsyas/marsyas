#include <cxxtest/TestSuite.h>

#include <cstdio>
#include <marsyas/Collection.h>
#include <marsyas/system/MarSystemManager.h>
#include <marsyas/CommandLineOptions.h>

#include <marsyas/marsystems/DelaySamples.h>

#include <vector>
#include <sstream>

using namespace std;
using namespace Marsyas;

class DelaySamples_runner: public CxxTest::TestSuite
{
public:
  realvec in, out;
  MarSystemManager mng;
  DelaySamples *delay;

  void setUp()
  {
    // Use the normal way for getting a Marsystem from the MarSystemManager
    // to make sure we don't bypass crucial things that should work
    // (e.g. the copy constructor).
    delay = (DelaySamples*) mng.create("DelaySamples", "delay");
  }

  // "destructor"
  void tearDown()
  {
    delete delay;
  }


  /**
   * Helper function to set up (and check) the flow settings.
   */
  void setUpAndCheckFlow(mrs_natural inObservations, mrs_natural inSamples,
                         mrs_natural delay)
  {
    // Set up the input flow.
    this->delay->updControl("mrs_natural/inObservations", inObservations);
    this->delay->updControl("mrs_natural/inSamples", inSamples);
    this->delay->updControl("mrs_natural/delay", delay);
    ostringstream inObsNames;
    for (mrs_natural i = 0; i < inObservations; i++)
    {
      inObsNames << "Input" << i << ",";
    }
    this->delay->updControl("mrs_string/inObsNames", inObsNames.str());

    // Check the output flow.
    TS_ASSERT_EQUALS(this->delay->getControl("mrs_natural/onObservations")->to<mrs_natural>(), inObservations);
    TS_ASSERT_EQUALS(this->delay->getControl("mrs_natural/onSamples")->to<mrs_natural>(), inSamples);
    ostringstream onObsNames;
    for (mrs_natural i = 0; i < inObservations; i++)
    {
      onObsNames << "DelaySamples" << (delay > 0 ? delay : 0) << "_Input"
                 << i << ",";
    }
    TS_ASSERT_EQUALS(this->delay->getControl("mrs_string/onObsNames")->to<mrs_string>(), onObsNames.str());
  }

  void test_flow_settings()
  {
    setUpAndCheckFlow(10, 64, 5);
  }

  void _test_process(mrs_natural inObservations = 3, mrs_natural inSamples =
                       12, mrs_natural delay = 3, mrs_natural slices = 5)
  {
    // The input and expected output flow settings.
    setUpAndCheckFlow(inObservations, inSamples, delay);

    // Allocate the input and output slices.
    in.create(inObservations, inSamples);
    out.create(inObservations, inSamples);

    mrs_real expected;
    for (mrs_natural s = 0; s < slices; s++)
    {
      //Feed input
      for (mrs_natural o = 0; o < inObservations; o++)
      {
        for (mrs_natural t = 0; t < inSamples; t++)
        {
          in(o, t) = (o + 1) * (s * inSamples + t);
        }
      }
      this->delay->myProcess(in, out);

      // Check output slice.
      for (mrs_natural o = 0; o < inObservations; o++)
      {
        for (mrs_natural t = 0; t < inSamples; t++)
        {
          expected = (o + 1) * (s * inSamples + t - delay);
          expected = expected > 0 ? expected : 0;
          TS_ASSERT_EQUALS(out(o, t), expected);
        }
      }
    }
  }

  void test_process_basic()
  {
    _test_process(4, 10, 3, 1);
  }
  void test_process_one_channel_multiple_slices_small_delay()
  {
    _test_process(1, 8, 4, 3);
  }

  void test_process_one_channel_multiple_slices_large_delay()
  {
    _test_process(1, 4, 8, 3);
  }

  void test_process_multiple_slices_small_delay()
  {
    _test_process(3, 12, 4);
  }

  void test_process_multiple_slices_large_delay()
  {
    _test_process(3, 12, 20);
  }

  void test_process_multiple_slices_zero_delay()
  {
    _test_process(3, 12, 0);
  }

  void test_process_negative_delay()
  {
    mrs_natural inObservations = 3;
    mrs_natural inSamples = 12;
    mrs_natural slices = 5;

    // The input and expected output flow settings.
    setUpAndCheckFlow(inObservations, inSamples, -10);
    mrs_natural delay = this->delay->getctrl("mrs_natural/delay")->to<
                        mrs_natural> ();
    TS_ASSERT_EQUALS(delay, 0);

    // Allocate the input and output slices.
    in.create(inObservations, inSamples);
    out.create(inObservations, inSamples);

    for (mrs_natural s = 0; s < slices; s++)
    {
      //Feed input
      for (mrs_natural o = 0; o < inObservations; o++)
      {
        for (mrs_natural t = 0; t < inSamples; t++)
        {
          in(o, t) = (o + 1) * (s * inSamples + t);
        }
      }
      this->delay->myProcess(in, out);

      // Check output slice.
      for (mrs_natural o = 0; o < inObservations; o++)
      {
        for (mrs_natural t = 0; t < inSamples; t++)
        {
          TS_ASSERT_EQUALS(out(o, t), in(o,t));
        }
      }
    }
  }

};

