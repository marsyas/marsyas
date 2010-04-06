#include <cxxtest/TestSuite.h>

#include "MarSystemManager.h"

// The class that you want to test, for example "WekaSource"
#include "RunningAutocorrelation.h"

using namespace std;
using namespace Marsyas;

/**
 * Sum of the integers from 0 to a given n (including).
 */
int sum_of_ints(int end) {
	return (end < 0) ? 0 : end * (end + 1) / 2;
}

/**
 * Sum of integers from given begin to end (including).
 */
int sum_of_ints(int n_begin, int n_end) {
	return (n_end < n_begin) ? 0 : sum_of_ints(n_end)
			- sum_of_ints(n_begin - 1);
}

/**
 * Sum of the squares from 0 to the given n (including).
 */
int sum_of_squares(int n) {
	return (n < 0) ? 0 : n * (n + 1) * (2 * n + 1) / 6;
}

/**
 * Sum of squares from given begin to end (including).
 */
int sum_of_squares(int n_begin, int n_end) {
	return (n_end < n_begin) ? 0 : sum_of_squares(n_end) - sum_of_squares(
			n_begin - 1);
}

class RunningAutocorrelation_runner: public CxxTest::TestSuite {
public:
	realvec in, out;
	MarSystemManager mng;
	RunningAutocorrelation *rac;

	void setUp() {
		rac = new RunningAutocorrelation("rac");
	}

	/**
	 * Set up the internal RunningAutocorrelation MarSystem with the input
	 * flow parameters and maxLag and check if the output flow
	 * parameters are set accordingly.
	 */
	void set_flow(mrs_natural inObservations, mrs_natural inSamples,
			mrs_natural maxLag) {
		rac->updctrl("mrs_natural/inObservations", inObservations);
		rac->updctrl("mrs_natural/inSamples", inSamples);
		rac->updctrl("mrs_natural/maxLag", maxLag);
		TS_ASSERT_EQUALS(rac->getControl("mrs_natural/onObservations")->to<mrs_natural>(), (maxLag+1) * inObservations);
		TS_ASSERT_EQUALS(rac->getControl("mrs_natural/onSamples")->to<mrs_natural>(), 1);
	}

	void test_output_flow(void) {
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
	void test_one_slice_single_row(void) {
		mrs_natural inObservations = 1;
		mrs_natural inSamples = 10;
		mrs_natural maxLag = 6;
		set_flow(inObservations, inSamples, maxLag);

		// Prepare input and output
		in.create(inObservations, inSamples);
		for (mrs_natural t = 0; t < inSamples; t++) {
			in(0, t) = t;
		}
		out.create(inObservations * (maxLag + 1), 1);

		// Process.
		rac->myProcess(in, out);

		// Check output.
		mrs_natural expected;
		for (mrs_natural lag = 0; lag <= maxLag; lag++) {
			expected = sum_of_squares(lag, inSamples - 1) - lag * sum_of_ints(
					lag, inSamples - 1);
			TS_ASSERT_EQUALS(out(lag, 0), expected);
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
	void test_one_slice_multirow(void) {
		mrs_natural inObservations = 5;
		mrs_natural inSamples = 10;
		mrs_natural maxLag = 6;
		set_flow(inObservations, inSamples, maxLag);

		// Prepare input and output
		in.create(inObservations, inSamples);
		for (mrs_natural r = 0; r < inObservations; r++) {
			for (mrs_natural t = 0; t < inSamples; t++) {
				in(r, t) = r + t;
			}
		}
		out.create(inObservations * (maxLag + 1), 1);

		// Process.
		rac->myProcess(in, out);

		// Check output.
		mrs_natural expected;
		for (mrs_natural r = 0; r < inObservations; r++) {
			for (mrs_natural lag = 0; lag <= maxLag; lag++) {
				expected = sum_of_squares(lag, inSamples - 1) + (2 * r - lag)
						* sum_of_ints(lag, inSamples - 1) + (r * r - r * lag)
						* (inSamples - lag);
				TS_ASSERT_EQUALS(out(r * (maxLag + 1) + lag, 0), expected);
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
	void test_two_slices_single_row(void) {
		mrs_natural inObservations = 1;
		mrs_natural inSamples = 10;
		mrs_natural maxLag = 6;
		set_flow(inObservations, inSamples, maxLag);

		// Prepare input and output
		in.create(inObservations, inSamples);
		out.create(inObservations * (maxLag + 1), 1);
		// First slice.
		for (mrs_natural t = 0; t < inSamples; t++) {
			in(0, t) = t;
		}
		// Process.
		rac->myProcess(in, out);

		// Second slice
		for (mrs_natural t = 0; t < inSamples; t++) {
			in(0, t) = inSamples + t;
		}
		// Process.
		rac->myProcess(in, out);

		// Check output.
		mrs_natural expected;
		for (mrs_natural lag = 0; lag <= maxLag; lag++) {
			expected = sum_of_squares(lag, 2 * inSamples - 1) - lag
					* sum_of_ints(lag, 2 * inSamples - 1);
			TS_ASSERT_EQUALS(out(lag, 0), expected);
		}

	}

};

