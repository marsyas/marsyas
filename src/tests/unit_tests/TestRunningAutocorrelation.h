#include <cxxtest/TestSuite.h>

#include <marsyas/system/MarSystemManager.h>

// The class that you want to test, for example "WekaSource"
#include <marsyas/marsystems/RunningAutocorrelation.h>

using namespace std;
using namespace Marsyas;
const mrs_real EPSILON = 1e-6;

/**
 * Sum of the integers from 0 to a given n (including).
 */
int sum_of_ints(int end)
{
  return (end < 0) ? 0 : end * (end + 1) / 2;
}

/**
 * Sum of integers from given begin to end (including).
 */
int sum_of_ints(int n_begin, int n_end)
{
  return (n_end < n_begin) ? 0 : sum_of_ints(n_end)
         - sum_of_ints(n_begin - 1);
}

/**
 * Sum of the squares from 0 to the given n (including).
 */
int sum_of_squares(int n)
{
  return (n < 0) ? 0 : n * (n + 1) * (2 * n + 1) / 6;
}

/**
 * Sum of squares from given begin to end (including).
 */
int sum_of_squares(int n_begin, int n_end)
{
  return (n_end < n_begin) ? 0 : sum_of_squares(n_end) - sum_of_squares(
           n_begin - 1);
}

/**
 * Calculate the autocorrelation function of
 *   x[n] = a * n + b  for n>=0
 *        = 0          for n < 0
 * The calculation for lag k (up to N samples):
 *   Rxx[k] = sum_{n=k}^{N} (a * n + b) * (a * n + b - k))
 *          = a^2 * sum_{n=k}^{N} n^2
 *          	+ (2ab - ak) * sum_{n=k}^{N} n
 *              + (b^2 - bk) * sum_{n=k}^{N} 1
 */
int autocorrelation_of_anplusb(int a, int b, int lag, int N)
{
  return a * a * sum_of_squares(lag, N) + (2 * a * b - a * lag)
         * sum_of_ints(lag, N) + (b * b - b * lag) * (N + 1 - lag);
}

class RunningAutocorrelation_runner: public CxxTest::TestSuite
{
public:
  realvec in, out;
  MarSystemManager mng;
  RunningAutocorrelation *rac;

  void setUp()
  {
    // Use the normal way for getting a Marsystem from the MarSystemManager
    // to make sure we don't bypass crucial things that should work
    // (e.g. the copy constructor).
    rac = (RunningAutocorrelation*) mng.create("RunningAutocorrelation",
          "rac");
  }

  // "destructor"
  void tearDown()
  {
    delete rac;
  }


  /**
   * Set up the internal RunningAutocorrelation MarSystem with the input
   * flow parameters and maxLag and check if the output flow
   * parameters are set accordingly.
   * Also allocate the input and output realvecs.
   */
  void set_flow(mrs_natural inObservations, mrs_natural inSamples,
                mrs_natural maxLag, mrs_bool unfoldToObservations = false)
  {
    mrs_natural onObservations = inObservations;
    mrs_natural onSamples = maxLag + 1;
    if (unfoldToObservations)
    {
      rac->updControl("mrs_bool/unfoldToObservations", true);
      onObservations = inObservations * (maxLag + 1);
      onSamples = 1;
    }
    rac->updControl("mrs_natural/inObservations", inObservations);
    rac->updControl("mrs_natural/inSamples", inSamples);
    rac->updControl("mrs_natural/maxLag", maxLag);
    TS_ASSERT_EQUALS(rac->getControl("mrs_natural/onObservations")->to<mrs_natural>(), onObservations);
    TS_ASSERT_EQUALS(rac->getControl("mrs_natural/onSamples")->to<mrs_natural>(), onSamples);
    // Allocate the input and output vectors.
    in.create(inObservations, inSamples);
    out.create(onObservations, onSamples);
  }

  void test_output_flow(void)
  {
    // Try some flow settings
    set_flow(1, 100, 10);
    set_flow(10, 10, 7);
    set_flow(10, 4, 10);
    set_flow(4, 10, 10);
  }

  /**
   * Test the processing of one slice (differently put: the previous slices
   * were all zero).
   * The input is a single rows [0, 1, 2, ..., 9]:
   *    x[0, n] = n     for n=0..9
   * The output for lag=k should be
   *    Rxx[k] = sum_{n=k}^{9} n * (n - k)
   *           = sum_{n=k}^{9} n^2 - k * sum_{n=k}^9 n
   */
  void test_one_slice_single_row(void)
  {
    mrs_natural inObservations = 1;
    mrs_natural inSamples = 10;
    mrs_natural maxLag = 6;
    set_flow(inObservations, inSamples, maxLag);

    // Create input
    for (mrs_natural t = 0; t < inSamples; t++)
    {
      in(0, t) = t;
    }

    // Process.
    rac->myProcess(in, out);

    // Check output.
    mrs_natural expected;
    for (mrs_natural lag = 0; lag <= maxLag; lag++)
    {
      expected = autocorrelation_of_anplusb(1, 0, lag, inSamples - 1);
      TS_ASSERT_EQUALS(out(0, lag), expected);
    }

  }

  /**
   * Test the processing of one slice with several rows
   *    x[r, n] = r + n  for r=0..5 and n=0..9
   *    e.g. row r: [r, r + 1, ..., r + 9]
   * The output for lag=k should be for row r:
   *     Rxx[k] = sum_{n=k}^{9} (r + n) * (r + n - k)
   *            = sum_{n=k}^{9} n^2 + (2r - k) * sum_{n=k}^9 n + (r^2-rk) * sum_{n=k}^9 1
   */
  void test_one_slice_multirow(void)
  {
    mrs_natural inObservations = 5;
    mrs_natural inSamples = 10;
    mrs_natural maxLag = 6;
    set_flow(inObservations, inSamples, maxLag);

    // Create input.
    for (mrs_natural r = 0; r < inObservations; r++)
    {
      for (mrs_natural t = 0; t < inSamples; t++)
      {
        in(r, t) = r + t;
      }
    }

    // Process.
    rac->myProcess(in, out);

    // Check output.
    mrs_natural expected;
    for (mrs_natural r = 0; r < inObservations; r++)
    {
      for (mrs_natural lag = 0; lag <= maxLag; lag++)
      {
        expected = autocorrelation_of_anplusb(1, r, lag, inSamples - 1);
        TS_ASSERT_EQUALS(out(r, lag), expected);
      }
    }
  }

  /**
   * Test the processing of two successive slices (single row).
   * First slice: [0, 1, 2, ..., 9]:
   *    x[0, n] = n     for n=0..9
   * Second slice: [10, 11, 12, ..., 19]:
   *    x[0,n] = 10 + n  for n=0..9
   * The output for second slice for lag=k should be
   *    Rxx[k] = sum_{n=k}^{19} n * (n - k)
   *           = sum_{n=k}^{19} n^2 - k * sum_{n=k}^19 n
   */
  void test_two_slices_single_row(void)
  {
    mrs_natural inObservations = 1;
    mrs_natural inSamples = 10;
    mrs_natural maxLag = 6;
    set_flow(inObservations, inSamples, maxLag);

    // Create input.
    // First slice.
    for (mrs_natural t = 0; t < inSamples; t++)
    {
      in(0, t) = t;
    }
    // Process.
    rac->myProcess(in, out);

    // Second slice
    for (mrs_natural t = 0; t < inSamples; t++)
    {
      in(0, t) = inSamples + t;
    }
    // Process.
    rac->myProcess(in, out);

    // Check output.
    mrs_natural expected;
    for (mrs_natural lag = 0; lag <= maxLag; lag++)
    {
      expected = autocorrelation_of_anplusb(1, 0, lag, 2 * inSamples - 1);
      TS_ASSERT_EQUALS(out(0, lag), expected);
    }
  }

  /**
   * Test the processing of several slices (10 samples per slice)
   * coming from a multiple channel signal:
   *    x[r, n] = r + n  for r=0..5 and n=0..\infty
   *    e.g. row r: [r, r+1, r+2, ...]
   * The output after S slices for lag=k should be for row r:
   *     Rxx[k] = sum_{n=k}^{10 S - 1} (r + n) * (r + n - k))
   *            = sum_{n=k}^{10 S - 1} n^2
   *                + (2r - k) * sum_{n=k}^{10 S - 1} n
   *                + (r^2-rk) * sum_{n=k}^{10 S - 1} 1
   */
  void test_multiple_slices_multirow(void)
  {
    mrs_natural inObservations = 5;
    mrs_natural inSamples = 10;
    mrs_natural maxLag = 6;
    mrs_natural slices = 7;
    set_flow(inObservations, inSamples, maxLag);

    // Feed with multiple multirow slices.
    for (mrs_natural s = 0; s < slices; s++)
    {
      for (mrs_natural r = 0; r < inObservations; r++)
      {
        for (mrs_natural t = 0; t < inSamples; t++)
        {
          in(r, t) = r + (s * inSamples + t);
        }
      }
      rac->myProcess(in, out);
    }

    // Check output.
    mrs_natural expected;
    for (mrs_natural r = 0; r < inObservations; r++)
    {
      for (mrs_natural lag = 0; lag <= maxLag; lag++)
      {
        expected = autocorrelation_of_anplusb(1, r, lag, slices
                                              * inSamples - 1);
        TS_ASSERT_EQUALS(out(r, lag), expected);
      }
    }
  }

  /**
   * Test with a maxLag larger than the slice size
   */
  void test_maxlag_larger_than_slice_size(void)
  {
    mrs_natural inObservations = 5;
    mrs_natural inSamples = 7;
    mrs_natural maxLag = 33;
    mrs_natural S = 7;
    set_flow(inObservations, inSamples, maxLag);

    // Feed with multiple multirow slices.
    for (mrs_natural s = 0; s < S; s++)
    {
      for (mrs_natural r = 0; r < inObservations; r++)
      {
        for (mrs_natural t = 0; t < inSamples; t++)
        {
          in(r, t) = r + (s * inSamples + t);
        }
      }
      rac->myProcess(in, out);
    }

    // Check output.
    mrs_natural expected;
    for (mrs_natural r = 0; r < inObservations; r++)
    {
      for (mrs_natural lag = 0; lag <= maxLag; lag++)
      {
        expected = autocorrelation_of_anplusb(1, r, lag, S * inSamples
                                              - 1);
        TS_ASSERT_EQUALS(out(r, lag), expected);
      }
    }
  }

  /**
   * Test the observation names
   */
  void test_observation_names()
  {
    set_flow(2, 5, 3);
    rac->updControl("mrs_string/inObsNames", "foo,bar,");
    mrs_string onObsNames = rac->getctrl("mrs_string/onObsNames")->to<
                            mrs_string> ();
    TS_ASSERT_EQUALS(onObsNames, "foo,bar,")
  }

  void test_observation_names_with_unfoldToObservations()
  {
    set_flow(2, 5, 3, true);
    rac->updControl("mrs_string/inObsNames", "foo,bar,");
    mrs_string onObsNames = rac->getctrl("mrs_string/onObsNames")->to<
                            mrs_string> ();
    TS_ASSERT_EQUALS(onObsNames, "Autocorr0_foo,Autocorr1_foo,Autocorr2_foo,Autocorr3_foo,Autocorr0_bar,Autocorr1_bar,Autocorr2_bar,Autocorr3_bar,")
  }

  void test_observation_names_with_unfoldToObservations_normalize()
  {
    set_flow(2, 5, 3, true);
    rac->updControl("mrs_string/inObsNames", "foo,bar,");
    rac->updControl("mrs_bool/normalize", true);
    mrs_string onObsNames = rac->getctrl("mrs_string/onObsNames")->to<
                            mrs_string> ();
    TS_ASSERT_EQUALS(onObsNames, "NormalizedAutocorr0_foo,NormalizedAutocorr1_foo,NormalizedAutocorr2_foo,NormalizedAutocorr3_foo,NormalizedAutocorr0_bar,NormalizedAutocorr1_bar,NormalizedAutocorr2_bar,NormalizedAutocorr3_bar,")
  }

  void test_observation_names_with_unfoldToObservations_normalize_but_not_lag0()
  {
    set_flow(2, 5, 3, true);
    rac->updControl("mrs_string/inObsNames", "foo,bar,");
    rac->updControl("mrs_bool/normalize", true);
    rac->updControl("mrs_bool/doNotNormalizeForLag0", true);
    mrs_string onObsNames = rac->getctrl("mrs_string/onObsNames")->to<
                            mrs_string> ();
    TS_ASSERT_EQUALS(onObsNames, "Autocorr0_foo,NormalizedAutocorr1_foo,NormalizedAutocorr2_foo,NormalizedAutocorr3_foo,Autocorr0_bar,NormalizedAutocorr1_bar,NormalizedAutocorr2_bar,NormalizedAutocorr3_bar,")
  }

  /**
   * Test flow change: the internal buffers should be updated accordingly.
   */
  void test_flow_change()
  {
    mrs_natural inObservations = 2;
    mrs_natural inSamples = 10;
    mrs_natural maxLag = 6;
    set_flow(inObservations, inSamples, maxLag);
    // Inrease the inObservations: internal buffers should be increased too.
    // (without using set_flow(), to avoid extra updates)
    inObservations = 200;
    rac->updControl("mrs_natural/inObservations", inObservations);
    in.create(inObservations, inSamples);
    out.create(inObservations, maxLag + 1);
    // This should not raise an out of array assertion.
    rac->myProcess(in, out);
  }

  /**
   * Test the normalization of autocorrelation values.
   */
  void test_normalization(mrs_bool doNotNormalizeForLag0 = false)
  {
    mrs_natural inObservations = 5;
    mrs_natural inSamples = 10;
    mrs_natural maxLag = 6;
    mrs_natural slices = 7;
    set_flow(inObservations, inSamples, maxLag);
    rac->updControl("mrs_bool/normalize", true);
    rac->updControl("mrs_bool/doNotNormalizeForLag0", doNotNormalizeForLag0);

    // Feed with multiple multirow slices.
    for (mrs_natural s = 0; s < slices; s++)
    {
      for (mrs_natural r = 0; r < inObservations; r++)
      {
        for (mrs_natural t = 0; t < inSamples; t++)
        {
          in(r, t) = r + (s * inSamples + t);
        }
      }
      rac->myProcess(in, out);
    }

    // Check output.
    mrs_real expected;
    for (mrs_natural r = 0; r < inObservations; r++)
    {
      for (mrs_natural lag = 0; lag <= maxLag; lag++)
      {
        expected = autocorrelation_of_anplusb(1, r, lag, slices
                                              * inSamples - 1);
        if (!(doNotNormalizeForLag0 && lag == 0))
        {
          expected /= autocorrelation_of_anplusb(1, r, 0, slices
                                                 * inSamples - 1);
        }
        TS_ASSERT_DELTA(out(r, lag), expected, EPSILON);
      }
    }
  }

  void test_normalization_but_not_for_lag0()
  {
    test_normalization(true);
  }

  /**
   * Test normalization with zero input.
   */
  void test_normalization_of_zero_input()
  {
    mrs_natural inObservations = 5;
    mrs_natural inSamples = 10;
    mrs_natural maxLag = 6;
    set_flow(inObservations, inSamples, maxLag);
    rac->updControl("mrs_bool/normalize", true);

    // Process.
    in.setval(0);
    rac->myProcess(in, out);

    // Check output.
    for (mrs_natural r = 0; r < inObservations; r++)
    {
      for (mrs_natural lag = 0; lag <= maxLag; lag++)
      {
        TS_ASSERT_EQUALS(out(r, lag), 0);
      }
    }
  }

  /**
   * Test clearing the internal buffers.
   */
  void test_internal_buffer_clearing(void)
  {
    mrs_natural inObservations = 5;
    mrs_natural inSamples = 10;
    mrs_natural maxLag = 6;
    set_flow(inObservations, inSamples, maxLag);

    // Create an input slice.
    for (mrs_natural r = 0; r < inObservations; r++)
    {
      for (mrs_natural t = 0; t < inSamples; t++)
      {
        in(r, t) = r + t;
      }
    }

    // Feed the monkey a couple of times but clear buffers first every time.
    for (mrs_natural i = 0; i < 10; i++)
    {
      rac->updControl("mrs_bool/clear", true);
      rac->myProcess(in, out);
    }

    // Check output.
    mrs_natural expected;
    for (mrs_natural r = 0; r < inObservations; r++)
    {
      for (mrs_natural lag = 0; lag <= maxLag; lag++)
      {
        expected = autocorrelation_of_anplusb(1, r, lag, inSamples - 1);
        TS_ASSERT_EQUALS(out(r, lag), expected);
      }
    }

  }

  /**
   * Test the unfoldToObservations feature.
   */
  void test_unfoldToObservations(mrs_bool normalize = false,
                                 mrs_bool doNotNormalizeForLag0 = false)
  {
    mrs_natural inObservations = 5;
    mrs_natural inSamples = 10;
    mrs_natural maxLag = 6;
    set_flow(inObservations, inSamples, maxLag, true);
    rac->updControl("mrs_bool/normalize", normalize);
    rac->updControl("mrs_bool/doNotNormalizeForLag0", doNotNormalizeForLag0);

    // Create input.
    for (mrs_natural r = 0; r < inObservations; r++)
    {
      for (mrs_natural t = 0; t < inSamples; t++)
      {
        in(r, t) = r + t;
      }
    }

    // Process.
    rac->myProcess(in, out);

    // Check output.
    mrs_real expected;
    for (mrs_natural r = 0; r < inObservations; r++)
    {
      for (mrs_natural lag = 0; lag <= maxLag; lag++)
      {
        expected = autocorrelation_of_anplusb(1, r, lag, inSamples - 1);
        if (normalize && !(doNotNormalizeForLag0 && lag == 0))
        {
          expected /= autocorrelation_of_anplusb(1, r, 0, inSamples
                                                 - 1);
        }
        TS_ASSERT_DELTA(out(r*(maxLag + 1) + lag, 0), expected, EPSILON);
      }
    }
  }

  void test_unfoldToObservations_normalize(void)
  {
    test_unfoldToObservations(true);
  }

  void test_unfoldToObservations_normalize_but_not_for_lag0(void)
  {
    test_unfoldToObservations(true, true);
  }

};

