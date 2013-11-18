// TestSample.h

#include <cxxtest/TestSuite.h>

#include <cstdio>
#include <marsyas/Collection.h>
#include <marsyas/system/MarSystemManager.h>
#include <marsyas/CommandLineOptions.h>

#include <marsyas/marsystems/RunningStatistics.h>

// Include TestRunningAutocorrelation.h for helper functions for sum of ints, etc.
#include "TestRunningAutocorrelation.h"

#include <vector>
#include <algorithm>

using namespace std;
using namespace Marsyas;


class RunningStatistics_runner: public CxxTest::TestSuite
{
public:
  realvec in, out;
  MarSystemManager mng;
  RunningStatistics * rs;

  void setUp()
  {
    // Use the normal way for getting a Marsystem from the MarSystemManager
    // to make sure we don't bypass crucial things that should work
    // (e.g. the copy constructor).
    rs = (RunningStatistics*) mng.create("RunningStatistics",
                                         "runningstatistics");
  }

  // "destructor"
  void tearDown()
  {
    delete rs;
  }

  /**
   * Helper function to calculate the statistics from a stream of zero and
   * ones.
   */
  void statistics_from_zero_and_ones(mrs_natural total, mrs_natural ones,
                                     mrs_real& mean, mrs_real& stddev, mrs_real&skewness)
  {
    mrs_natural zeros = total - ones;
    mean = (mrs_real) ones / total;
    mrs_real var = (ones * (1 - mean) * (1 - mean) + (zeros * mean * mean))
                   / total;
    stddev = sqrt(var);
    skewness = (ones * (1 - mean) * (1 - mean) * (1 - mean) + (zeros
                * -mean * -mean * -mean)) / total;
    skewness = var > 0.0 ? skewness / (var * stddev) : 0.0;
  }

  /**
   * Test the default flow settings.
   */
  void test_default_flow_settings()
  {
    // Set up the input flow.
    rs->updControl("mrs_natural/inObservations", 3);
    rs->updControl("mrs_natural/inSamples", 128);
    rs->updControl("mrs_string/inObsNames", "foo0,foo1,foo2,");
    // Check the output flow.
    TS_ASSERT_EQUALS(rs->getControl("mrs_natural/onObservations")->to<mrs_natural>(), 6);
    TS_ASSERT_EQUALS(rs->getControl("mrs_natural/onSamples")->to<mrs_natural>(), 1);
    TS_ASSERT_EQUALS(rs->getControl("mrs_string/onObsNames")->to<mrs_string>(), "RunningMean_foo0,RunningMean_foo1,RunningMean_foo2,RunningStddev_foo0,RunningStddev_foo1,RunningStddev_foo2,");
  }

  /**
   * Helper function for customized flows.
   */
  void _test_flow_settings(mrs_bool enable_mean = true,
                           mrs_bool enable_stddev = true, mrs_bool enable_skewness = false)
  {
    mrs_natural inObservations = 3;
    // Set up the input flow.
    rs->updControl("mrs_natural/inObservations", inObservations);
    rs->updControl("mrs_natural/inSamples", 128);
    rs->updControl("mrs_string/inObsNames", "foo0,foo1,foo2,");
    rs->updControl("mrs_bool/enableMean", enable_mean);
    rs->updControl("mrs_bool/enableStddev", enable_stddev);
    rs->updControl("mrs_bool/enableSkewness", enable_skewness);
    // Check the output flow.
    mrs_natural fanout = (mrs_natural) enable_mean
                         + (mrs_natural) enable_stddev + (mrs_natural) enable_skewness;
    TS_ASSERT_EQUALS(rs->getControl("mrs_natural/onObservations")->to<mrs_natural>(), fanout * inObservations);
    TS_ASSERT_EQUALS(rs->getControl("mrs_natural/onSamples")->to<mrs_natural>(), 1);
    mrs_string onObsNames("");
    if (enable_mean)
    {
      onObsNames += "RunningMean_foo0,RunningMean_foo1,RunningMean_foo2,";
    }
    if (enable_stddev)
    {
      onObsNames
      += "RunningStddev_foo0,RunningStddev_foo1,RunningStddev_foo2,";
    }
    if (enable_skewness)
    {
      onObsNames
      += "RunningSkewness_foo0,RunningSkewness_foo1,RunningSkewness_foo2,";
    }
    TS_ASSERT_EQUALS(rs->getControl("mrs_string/onObsNames")->to<mrs_string>(), onObsNames);
  }

  void test_flow_settings()
  {
    for (mrs_natural enable_mean = 0; enable_mean < 2; enable_mean++)
    {
      for (mrs_natural enable_stddev = 0; enable_stddev < 2; enable_stddev++)
      {
        for (mrs_natural enable_skewness = 0; enable_skewness < 2; enable_skewness++)
        {
          _test_flow_settings((bool) enable_mean,
                              (bool) enable_stddev,
                              (bool) enable_skewness);
        }
      }
    }
  }

  /**
   * Helper function for setting up the MarSystem and its processing.
   */
  void _test_process(mrs_natural inObservations, mrs_natural inSamples,
                     mrs_natural onObservations, mrs_bool enable_mean,
                     mrs_bool enable_stddev, mrs_bool enable_skewness)
  {
    mrs_natural onSamples = 1;
    // Set up the input flow.
    rs->updControl("mrs_natural/inObservations", inObservations);
    rs->updControl("mrs_natural/inSamples", inSamples);
    rs->updControl("mrs_bool/enableMean", enable_mean);
    rs->updControl("mrs_bool/enableStddev", enable_stddev);
    rs->updControl("mrs_bool/enableSkewness", enable_skewness);
    // Check the output flow.
    TS_ASSERT_EQUALS(rs->getControl("mrs_natural/onObservations")->to<mrs_natural>(), onObservations);
    TS_ASSERT_EQUALS(rs->getControl("mrs_natural/onSamples")->to<mrs_natural>(), onSamples);

    // Allocate the input and output slices.
    in.create(inObservations, inSamples);
    out.create(onObservations, onSamples);

    // Fill the input slice.
    for (mrs_natural o = 0; o < inObservations; o++)
    {
      for (mrs_natural t = 0; t < inSamples; t++)
      {
        in(o, t) = mrs_natural(o > t);
      }
    }

    // Process.
    rs->myProcess(in, out);
  }

  /**
   * Test mean.
   */
  void test_process_mean()
  {
    mrs_natural inObservations = 10;
    mrs_natural inSamples = 10;
    mrs_natural onObservations = inObservations;
    _test_process(inObservations, inSamples, onObservations, true, false,
                  false);

    // Check output slice.
    for (mrs_natural i = 0; i < inObservations; i++)
    {
      mrs_real mean = (mrs_real) i / inSamples;
      TS_ASSERT_DELTA(out(i, 0), mean, EPSILON);
    }
  }

  /**
   * Test stddev.
   */
  void test_process_stddev()
  {
    mrs_natural inObservations = 10;
    mrs_natural inSamples = 10;
    mrs_natural onObservations = inObservations;
    _test_process(inObservations, inSamples, onObservations, false, true,
                  false);

    // Check output slice.
    mrs_real mean, stddev, skewness;
    for (mrs_natural i = 0; i < inObservations; i++)
    {
      statistics_from_zero_and_ones(inSamples, i, mean, stddev, skewness);
      TS_ASSERT_DELTA(out(i, 0), stddev, EPSILON);
    }
  }

  /**
   * Test skewness.
   */
  void test_process_skewness()
  {
    mrs_natural inObservations = 10;
    mrs_natural inSamples = 10;
    mrs_natural onObservations = inObservations;
    _test_process(inObservations, inSamples, onObservations, false, false,
                  true);

    // Check output slice.
    mrs_real mean, stddev, skewness;
    for (mrs_natural i = 0; i < inObservations; i++)
    {
      statistics_from_zero_and_ones(inSamples, i, mean, stddev, skewness);
      TS_ASSERT_DELTA(out(i, 0), skewness, EPSILON);
    }
  }

  void test_process_mean_and_stddev()
  {
    mrs_natural inObservations = 10;
    mrs_natural inSamples = 10;
    mrs_natural onObservations = 2 * inObservations;
    _test_process(inObservations, inSamples, onObservations, true, true,
                  false);

    // Check output slice.
    mrs_real mean, stddev, skewness;
    for (mrs_natural i = 0; i < inObservations; i++)
    {
      statistics_from_zero_and_ones(inSamples, i, mean, stddev, skewness);
      TS_ASSERT_DELTA(out(i, 0), mean, EPSILON);
      TS_ASSERT_DELTA(out(inObservations + i, 0), stddev, EPSILON);
    }
  }

  void test_process_stddev_and_skewness()
  {
    mrs_natural inObservations = 10;
    mrs_natural inSamples = 10;
    mrs_natural onObservations = 2 * inObservations;
    _test_process(inObservations, inSamples, onObservations, false, true,
                  true);

    // Check output slice.
    mrs_real mean, stddev, skewness;
    for (mrs_natural i = 0; i < inObservations; i++)
    {
      statistics_from_zero_and_ones(inSamples, i, mean, stddev, skewness);
      TS_ASSERT_DELTA(out(i, 0), stddev, EPSILON);
      TS_ASSERT_DELTA(out(inObservations + i, 0), skewness, EPSILON);
    }
  }

  void test_process_mean_and_stddev_and_skewness()
  {
    mrs_natural inObservations = 10;
    mrs_natural inSamples = 10;
    mrs_natural onObservations = 3 * inObservations;
    _test_process(inObservations, inSamples, onObservations, true, true,
                  true);

    // Check output slice.
    mrs_real mean, stddev, skewness;
    for (mrs_natural i = 0; i < inObservations; i++)
    {
      statistics_from_zero_and_ones(inSamples, i, mean, stddev, skewness);
      TS_ASSERT_DELTA(out(i, 0), mean, EPSILON);
      TS_ASSERT_DELTA(out(inObservations + i, 0), stddev, EPSILON);
      TS_ASSERT_DELTA(out(2*inObservations + i, 0), skewness, EPSILON);
    }
  }

  /**
   * Test for the "running" property of the statistics.
   */
  void test_running_property()
  {
    mrs_natural inObservations = 10;
    mrs_natural inSamples = 10;
    mrs_natural onObservations = 3 * inObservations;
    mrs_natural slices = 4;
    mrs_natural onSamples = 1;
    // Set up the input flow.
    rs->updControl("mrs_natural/inObservations", inObservations);
    rs->updControl("mrs_natural/inSamples", inSamples);
    rs->updControl("mrs_bool/enableMean", true);
    rs->updControl("mrs_bool/enableStddev", true);
    rs->updControl("mrs_bool/enableSkewness", true);
    // Check the output flow.
    TS_ASSERT_EQUALS(rs->getControl("mrs_natural/onObservations")->to<mrs_natural>(), onObservations);
    TS_ASSERT_EQUALS(rs->getControl("mrs_natural/onSamples")->to<mrs_natural>(), onSamples);

    // Allocate the input and output slices.
    in.create(inObservations, inSamples);
    out.create(onObservations, onSamples);

    // Input is a simple binary pattern.
    for (mrs_natural s = 0; s < slices; s++)
    {
      // Fill the input slice.
      for (mrs_natural o = 0; o < inObservations; o++)
      {
        for (mrs_natural t = 0; t < inSamples; t++)
        {
          in(o, t) = mrs_natural(o + s > t);
        }
      }
      // Process.
      rs->myProcess(in, out);
    }

    // Check output slice.
    mrs_natural total, ones;
    mrs_real mean, stddev, skewness;
    for (mrs_natural i = 0; i < inObservations; i++)
    {
      // How many ones did we get in each observation channel?
      total = slices * inSamples;
      ones = 0;
      for (mrs_natural s = 0; s < slices; s++)
      {
        ones += min(i + s, inSamples);
      }
      statistics_from_zero_and_ones(total, ones, mean, stddev, skewness);
      TS_ASSERT_DELTA(out(i, 0), mean, EPSILON);
      TS_ASSERT_DELTA(out(inObservations + i, 0), stddev, EPSILON);
      TS_ASSERT_DELTA(out(2*inObservations + i, 0), skewness, EPSILON);
    }
  }

  /**
   * Test the clear control.
   */
  void test_clear()
  {
    mrs_natural inObservations = 10;
    mrs_natural inSamples = 10;
    mrs_natural onObservations = 3 * inObservations;
    mrs_natural slices = 4;
    mrs_natural onSamples = 1;
    // Set up the input flow.
    rs->updControl("mrs_natural/inObservations", inObservations);
    rs->updControl("mrs_natural/inSamples", inSamples);
    rs->updControl("mrs_bool/enableMean", true);
    rs->updControl("mrs_bool/enableStddev", true);
    rs->updControl("mrs_bool/enableSkewness", true);
    // Check the output flow.
    TS_ASSERT_EQUALS(rs->getControl("mrs_natural/onObservations")->to<mrs_natural>(), onObservations);
    TS_ASSERT_EQUALS(rs->getControl("mrs_natural/onSamples")->to<mrs_natural>(), onSamples);

    // Allocate the input and output slices.
    in.create(inObservations, inSamples);
    out.create(onObservations, onSamples);

    // Input is a simple binary pattern.
    for (mrs_natural s = 0; s < slices; s++)
    {
      // Clear before second last slice.
      if (s == slices - 2)
      {
        rs->updControl("mrs_bool/clear", true);
      }
      // Fill the input slice.
      for (mrs_natural o = 0; o < inObservations; o++)
      {
        for (mrs_natural t = 0; t < inSamples; t++)
        {
          in(o, t) = mrs_natural(o + s > t);
        }
      }
      // Process.
      rs->myProcess(in, out);
    }

    // Check output slice.
    mrs_natural total, ones;
    mrs_real mean, stddev, skewness;
    for (mrs_natural i = 0; i < inObservations; i++)
    {
      // How many ones did we get in each observation channel?
      total = 2 * inSamples;
      ones = min(i + slices - 2, inSamples) + min(i + slices - 1,
             inSamples);
      statistics_from_zero_and_ones(total, ones, mean, stddev, skewness);
      TS_ASSERT_DELTA(out(i, 0), mean, EPSILON);
      TS_ASSERT_DELTA(out(inObservations + i, 0), stddev, EPSILON);
      TS_ASSERT_DELTA(out(2*inObservations + i, 0), skewness, EPSILON);
    }

  }

  /**
   * Test the clearPerTick control.
   */
  void test_clearPerTick()
  {
    mrs_natural inObservations = 10;
    mrs_natural inSamples = 10;
    mrs_natural onObservations = 3 * inObservations;
    mrs_natural slices = 4;
    mrs_natural onSamples = 1;
    // Set up the input flow.
    rs->updControl("mrs_natural/inObservations", inObservations);
    rs->updControl("mrs_natural/inSamples", inSamples);
    rs->updControl("mrs_bool/enableMean", true);
    rs->updControl("mrs_bool/enableStddev", true);
    rs->updControl("mrs_bool/enableSkewness", true);
    rs->updControl("mrs_bool/clearPerTick", true);
    // Check the output flow.
    TS_ASSERT_EQUALS(rs->getControl("mrs_natural/onObservations")->to<mrs_natural>(), onObservations);
    TS_ASSERT_EQUALS(rs->getControl("mrs_natural/onSamples")->to<mrs_natural>(), onSamples);

    // Allocate the input and output slices.
    in.create(inObservations, inSamples);
    out.create(onObservations, onSamples);

    // Input is a simple binary pattern.
    for (mrs_natural s = 0; s < slices; s++)
    {
      // Fill the input slice.
      for (mrs_natural o = 0; o < inObservations; o++)
      {
        for (mrs_natural t = 0; t < inSamples; t++)
        {
          in(o, t) = mrs_natural(o + s > t);
        }
      }
      // Process.
      rs->myProcess(in, out);
    }

    // Check output slice.
    mrs_natural total, ones;
    mrs_real mean, stddev, skewness;
    for (mrs_natural i = 0; i < inObservations; i++)
    {
      // How many ones did we get in each observation channel?
      total = inSamples;
      ones = min(i + slices - 1, inSamples);
      statistics_from_zero_and_ones(total, ones, mean, stddev, skewness);
      TS_ASSERT_DELTA(out(i, 0), mean, EPSILON);
      TS_ASSERT_DELTA(out(inObservations + i, 0), stddev, EPSILON);
      TS_ASSERT_DELTA(out(2*inObservations + i, 0), skewness, EPSILON);
    }

  }

};

