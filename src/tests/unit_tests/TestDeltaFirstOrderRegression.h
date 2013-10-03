#include <cxxtest/TestSuite.h>

#include <cstdio>
#include <marsyas/Collection.h>
#include <marsyas/system/MarSystemManager.h>
#include <marsyas/CommandLineOptions.h>

// The class that you want to test, for example "DeltaFirstOrderRegression"
#include <marsyas/marsystems/DeltaFirstOrderRegression.h>

#include <vector>

using namespace std;
using namespace Marsyas;

class DeltaFirstOrderRegression_runner: public CxxTest::TestSuite
{
public:
  realvec in, out;
  MarSystemManager mng;
  DeltaFirstOrderRegression *delta;

  void setUp()
  {
    // Use the normal way for getting a Marsystem from the MarSystemManager
    // to make sure we don't bypass crucial things that should work
    // (e.g. the copy constructor).
    delta = (DeltaFirstOrderRegression*) mng.create(
              "DeltaFirstOrderRegression", "delta");
  }


  // "destructor"
  void tearDown()
  {
    delete delta;
  }

  /**
   * Example test of the DeltaFirstOrderRegression flow settings.
   */
  void test_flow_settings()
  {
    // The input and expected output flow settings.
    mrs_natural inObservations = 10;
    mrs_natural inSamples = 128;
    mrs_natural onObservations = 10;
    mrs_natural onSamples = 128;

    // Set up the input flow.
    delta->updControl("mrs_natural/inObservations", inObservations);
    delta->updControl("mrs_natural/inSamples", inSamples);

    // Check the output flow.
    TS_ASSERT_EQUALS(delta->getControl("mrs_natural/onObservations")->to<mrs_natural>(), onObservations);
    TS_ASSERT_EQUALS(delta->getControl("mrs_natural/onSamples")->to<mrs_natural>(), onSamples);
  }

  void test_obsnames()
  {
    mrs_natural inObservations = 2;
    mrs_natural inSamples = 128;
    delta->updControl("mrs_natural/inObservations", inObservations);
    delta->updControl("mrs_natural/inSamples", inSamples);
    delta->updControl("mrs_string/inObsNames", "Input0,Input1,");
    TS_ASSERT_EQUALS(delta->getControl("mrs_natural/onObservations")->to<mrs_natural>(), inObservations);
    TS_ASSERT_EQUALS(delta->getControl("mrs_natural/onSamples")->to<mrs_natural>(), inSamples);
    TS_ASSERT_EQUALS(delta->getControl("mrs_string/onObsNames")->to<mrs_string>(), "DeltaR1_Input0,DeltaR1_Input1,");
  }

  /**
   * Example test of the DeltaFirstOrderRegression process.
   */
  void test_process_one_slice()
  {
    // The input and expected output flow settings.
    mrs_natural inObservations = 2;
    mrs_natural inSamples = 10;
    mrs_natural onObservations = 2;
    mrs_natural onSamples = 10;

    // Set up the input flow.
    delta->updControl("mrs_natural/inObservations", inObservations);
    delta->updControl("mrs_natural/inSamples", inSamples);

    // Allocate the input and output slices.
    in.create(inObservations, inSamples);
    out.create(onObservations, onSamples);

    // Fill the input slice.
    for (mrs_natural t = 0; t < inSamples; t++)
    {
      in(0, t) = t * t;
      in(1, t) = (t % 3 == 0);
    }

    // Process.
    delta->myProcess(in, out);

    // Check output slice.
    mrs_real expected;
    for (mrs_natural t = 0; t < onSamples; t++)
    {
      // Obchannel 0
      expected = (t == 0 ? 0 : (t == 1 ? 0.5 : (t - 1) * 2));
      TS_ASSERT_EQUALS(out(0, t), expected);
      // Obchannel 1
      expected = (t % 3 == 0 ? 0.5 : (t % 3 == 1 ? 0 : -0.5));
      TS_ASSERT_EQUALS(out(1, t), expected);
    }

  }

  /**
   * Example test of the DeltaFirstOrderRegression process.
   */
  void test_process_two_slices()
  {
    // The input and expected output flow settings.
    mrs_natural inObservations = 2;
    mrs_natural inSamples = 10;
    mrs_natural onObservations = 2;
    mrs_natural onSamples = 10;

    // Set up the input flow.
    delta->updControl("mrs_natural/inObservations", inObservations);
    delta->updControl("mrs_natural/inSamples", inSamples);

    // Allocate the input and output slices.
    in.create(inObservations, inSamples);
    out.create(onObservations, onSamples);

    // Process first input slice.
    for (mrs_natural t = 0; t < inSamples; t++)
    {
      in(0, t) = t * t;
      in(1, t) = (t % 3 == 0);
    }
    delta->myProcess(in, out);

    // Second input slice.
    for (mrs_natural t = 0; t < inSamples; t++)
    {
      mrs_natural t_global = inSamples + t;
      in(0, t) = t_global * t_global;
      in(1, t) = (t_global % 3 == 0);
    }
    delta->myProcess(in, out);

    // Check output slice.
    mrs_real expected;
    for (mrs_natural t = 0; t < onSamples; t++)
    {
      mrs_natural t_global = inSamples + t;
      // Obchannel 0
      expected = 2 * (t_global - 1);
      TS_ASSERT_EQUALS(out(0, t), expected);
      // Obchannel 1
      expected = (t_global % 3 == 0 ? 0.5
                  : (t_global % 3 == 1 ? 0 : -0.5));
      TS_ASSERT_EQUALS(out(1, t), expected);
    }

  }

  void test_process_single_sample_slices()
  {
    // The input and expected output flow settings.
    mrs_natural inObservations = 2;
    mrs_natural inSamples = 1;
    mrs_natural onObservations = 2;
    mrs_natural onSamples = 1;
    mrs_natural slices = 20;

    // Set up the input flow.
    delta->updControl("mrs_natural/inObservations", inObservations);
    delta->updControl("mrs_natural/inSamples", inSamples);

    // Allocate the input and output slices.
    in.create(inObservations, inSamples);
    out.create(onObservations, onSamples);

    for (mrs_natural t = 0; t < slices; t++)
    {
      in(0, 0) = t * t;
      in(1, 0) = (t % 3 == 0);
      delta->myProcess(in, out);

      // Check output slice.
      mrs_real expected;
      // Obchannel 0
      expected = (t == 0 ? 0 : (t == 1 ? 0.5 : (t - 1) * 2));
      TS_ASSERT_EQUALS(out(0, 0), expected);
      // Obchannel 1
      expected = (t % 3 == 0 ? 0.5 : (t % 3 == 1 ? 0 : -0.5));
      TS_ASSERT_EQUALS(out(1, 0), expected);
    }

  }

};

