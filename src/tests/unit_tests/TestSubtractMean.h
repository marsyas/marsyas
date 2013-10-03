#include <cxxtest/TestSuite.h>

#include <cstdio>
#include <marsyas/Collection.h>
#include <marsyas/system/MarSystemManager.h>

#include <marsyas/marsystems/SubtractMean.h>

#include <vector>

using namespace std;
using namespace Marsyas;

class SubtractMean_runner: public CxxTest::TestSuite
{
public:
  realvec in, out;
  MarSystemManager mng;
  SubtractMean *subtractMean;

  void setUp()
  {
    // Use the normal way for getting a Marsystem from the MarSystemManager
    // to make sure we don't bypass crucial things that should work
    // (e.g. the copy constructor).
    subtractMean = (SubtractMean*) mng.create("SubtractMean",
                   "SubtractMean");
  }


  // "destructor"
  void tearDown()
  {
    delete subtractMean;
  }

  /**
   * Test the SubtractMean flow settings.
   */
  void test_flow_settings()
  {
    // The input and expected output flow settings.
    mrs_natural inObservations = 10;
    mrs_natural inSamples = 128;
    mrs_natural onObservations = 10;
    mrs_natural onSamples = 128;

    // Set up the input flow.
    subtractMean->updControl("mrs_natural/inObservations", inObservations);
    subtractMean->updControl("mrs_natural/inSamples", inSamples);

    // Check the output flow.
    TS_ASSERT_EQUALS(subtractMean->getControl("mrs_natural/onObservations")->to<mrs_natural>(), onObservations);
    TS_ASSERT_EQUALS(subtractMean->getControl("mrs_natural/onSamples")->to<mrs_natural>(), onSamples);
  }

  /**
   * Test the SubtractMean process.
   */
  void test_process()
  {
    // The input and expected output flow settings.
    mrs_natural inObservations = 5;
    mrs_natural inSamples = 16;
    mrs_natural onObservations = 5;
    mrs_natural onSamples = 16;

    // Set up the input flow.
    subtractMean->updControl("mrs_natural/inObservations", inObservations);
    subtractMean->updControl("mrs_natural/inSamples", inSamples);

    // Allocate the input and output slices.
    in.create(inObservations, inSamples);
    out.create(onObservations, onSamples);

    // Fill the input slice.
    for (mrs_natural o = 0; o < inObservations; o++)
    {
      for (mrs_natural t = 0; t < inSamples; t++)
      {
        in(o, t) = o + t;
      }
    }

    // Process.
    subtractMean->myProcess(in, out);

    // Check output slice.
    mrs_real expected;
    for (mrs_natural o = 0; o < onObservations; o++)
    {
      for (mrs_natural t = 0; t < onSamples; t++)
      {
        expected = t - (inSamples - 1.0) / 2.0;
        TS_ASSERT_EQUALS(out(o, t), expected);
      }
    }

  }

};

