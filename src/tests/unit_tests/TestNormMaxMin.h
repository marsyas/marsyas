// TestNormMaxMin.h

//
// The NormMaxMin class normalizes a realvec, and can normalize across
// observations, samples or slices.
//
//                ----------------------
//                |				 	   |
//                |				 	   |
// "observations" |				 	   |
//                |				 	   |
//                |				       |   the whole rectangle
// 	              ----------------------   is a "slice"
//                      "samples"
//

//
// (c) 2008 - sness@sness.net - GPL
//

#include <cxxtest/TestSuite.h>

#include <cstdio>
#include <marsyas/Collection.h>
#include <marsyas/system/MarSystemManager.h>
#include <marsyas/marsystems/NormMaxMin.h>
#include <marsyas/CommandLineOptions.h>

#include <vector>

using namespace std;
using namespace Marsyas;

class NormMaxMin_runner : public CxxTest::TestSuite
{
public:
  realvec in,out;
  MarSystemManager mng;
  NormMaxMin *norm;

  void
  setUp()
  {
    norm = new NormMaxMin("norm");
    norm->updControl("mrs_natural/inObservations", 5);
    norm->updControl("mrs_natural/inSamples", 5);
    norm->updControl("mrs_real/lower", 0.0);
    norm->updControl("mrs_real/upper", 1.0);

    // Create a simple 5x5 matrix
    in.create(5,5);
    clear_input_realvec();

    out.create(5,5);
  }

  void
  tearDown()
  {
    delete norm;
  }

  void
  clear_input_realvec()
  {
    // Clear the data from the in realvec
    for (mrs_natural i=0; i<5; i++) {
      for (mrs_natural j=0; j<5; j++) {
        in(i,j) = i*1000 + j;
      }
    }
  }

  //
  // Observations mode
  //
  void test_two_pass_observations_should_normalize_across_each_observation(void)
  {

    norm->updControl("mrs_string/mode", "twopass");
    norm->updControl("mrs_string/domain", "observations");
    norm->myProcess(in,out);

    // The whole column
    TS_ASSERT_EQUALS(out(0,0), 0.00);
    TS_ASSERT_EQUALS(out(0,1), 0.25);
    TS_ASSERT_EQUALS(out(0,2), 0.50);
    TS_ASSERT_EQUALS(out(0,3), 0.75);
    TS_ASSERT_EQUALS(out(0,4), 1.00);
  }

  void test_train_on_observations_only_should_not_change_input_data(void)
  {
    norm->updControl("mrs_string/mode", "train");
    norm->updControl("mrs_string/domain", "observations");
    norm->myProcess(in,out);

    TS_ASSERT_EQUALS(out(0,0), 0000);
    TS_ASSERT_EQUALS(out(1,1), 1001);
    TS_ASSERT_EQUALS(out(2,2), 2002);
    TS_ASSERT_EQUALS(out(3,3), 3003);
    TS_ASSERT_EQUALS(out(4,4), 4004);

  }

  void test_should_be_able_to_train_observations_and_then_predict(void)
  {
    norm->updControl("mrs_string/domain", "observations");

    // Setup some data for the first training session
    clear_input_realvec();
    in(0,0) = 10000;
    norm->updControl("mrs_string/mode", "train");
    norm->myProcess(in,out);

    // Setup some data for the second training session
    clear_input_realvec();
    in(1,0) = 1000000;
    norm->updControl("mrs_string/mode", "train");
    norm->myProcess(in,out);

    // Predict the normalized matrix
    clear_input_realvec();
    norm->updControl("mrs_string/mode", "predict");
    norm->myProcess(in,out);

    TS_ASSERT_DELTA(out(0,1), 0.0001, 0.00001);
    TS_ASSERT_DELTA(out(1,1), 1.001e-06, 0.001e-07);
  }

  void test_observations_maximums_control_should_be_set_properly_after_train_and_then_predict(void)
  {
    norm->updControl("mrs_string/domain", "observations");

    norm->updControl("mrs_string/mode", "train");
    norm->myProcess(in,out);

    norm->updControl("mrs_string/mode", "predict");
    norm->myProcess(in,out);

    realvec m = norm->getctrl("mrs_realvec/maximums")->to<mrs_realvec>();
    TS_ASSERT_EQUALS(m(4), 4004);

  }

  void test_observations_minimums_control_should_be_set_properly_after_train_and_then_predict(void)
  {
    norm->updControl("mrs_string/domain", "observations");

    norm->updControl("mrs_string/mode", "train");
    norm->myProcess(in,out);

    norm->updControl("mrs_string/mode", "predict");
    norm->myProcess(in,out);

    realvec m = norm->getctrl("mrs_realvec/minimums")->to<mrs_realvec>();
    TS_ASSERT_EQUALS(m(4), 4000);

  }


  //
  // Samples mode
  //

  void test_two_pass_samples_should_normalize_down_each_sample(void)
  {

    norm->updControl("mrs_string/mode", "twopass");
    norm->updControl("mrs_string/domain", "samples");

    norm->myProcess(in,out);

    TS_ASSERT_EQUALS(out(0,0), 0.00);
    TS_ASSERT_EQUALS(out(1,0), 0.25);
    TS_ASSERT_EQUALS(out(2,0), 0.50);
    TS_ASSERT_EQUALS(out(3,0), 0.75);
    TS_ASSERT_EQUALS(out(4,0), 1.00);
  }

  void test_train_on_samples_only_should_not_change_input_data(void)
  {
    norm->updControl("mrs_string/mode", "train");
    norm->updControl("mrs_string/domain", "samples");
    norm->myProcess(in,out);

    TS_ASSERT_EQUALS(out(0,0), 0000);
    TS_ASSERT_EQUALS(out(1,1), 1001);
    TS_ASSERT_EQUALS(out(2,2), 2002);
    TS_ASSERT_EQUALS(out(3,3), 3003);
    TS_ASSERT_EQUALS(out(4,4), 4004);

  }

  void test_should_be_able_to_train_samples_and_then_predict(void)
  {
    norm->updControl("mrs_string/domain", "samples");

    // Setup some data for the first training session
    clear_input_realvec();
    in(0,0) = 10000;
    norm->updControl("mrs_string/mode", "train");
    norm->myProcess(in,out);

    // Setup some data for the second training session
    clear_input_realvec();
    in(0,1) = 1000000;
    norm->updControl("mrs_string/mode", "train");
    norm->myProcess(in,out);

    // Predict the normalized matrix
    clear_input_realvec();
    norm->updControl("mrs_string/mode", "predict");
    norm->myProcess(in,out);

    TS_ASSERT_DELTA(out(4,0), 0.4, 0.01);
    TS_ASSERT_DELTA(out(4,1), 0.004, 0.0001);
  }

  void test_samples_maximums_control_should_be_set_properly_after_train_and_then_predict(void)
  {
    norm->updControl("mrs_string/domain", "samples");

    norm->updControl("mrs_string/mode", "train");
    norm->myProcess(in,out);

    norm->updControl("mrs_string/mode", "predict");
    norm->myProcess(in,out);

    realvec m = norm->getctrl("mrs_realvec/maximums")->to<mrs_realvec>();
    TS_ASSERT_EQUALS(m(4), 4004);

  }

  void test_samples_minimums_control_should_be_set_properly_after_train_and_then_predict(void)
  {
    norm->updControl("mrs_string/domain", "samples");

    norm->updControl("mrs_string/mode", "train");
    norm->myProcess(in,out);

    norm->updControl("mrs_string/mode", "predict");
    norm->myProcess(in,out);

    realvec m = norm->getctrl("mrs_realvec/minimums")->to<mrs_realvec>();
    TS_ASSERT_EQUALS(m(4), 4);

  }

  //
  // Slices mode
  //

  void test_two_pass_slices_should_normalize_over_entire_slice(void)
  {

    norm->updControl("mrs_string/mode", "twopass");
    norm->updControl("mrs_string/domain", "slices");
    norm->myProcess(in,out);

    TS_ASSERT_EQUALS(out(0,0), 0.00);
    TS_ASSERT_EQUALS(out(1,1), 0.25);
    TS_ASSERT_EQUALS(out(2,2), 0.50);
    TS_ASSERT_EQUALS(out(3,3), 0.75);
    TS_ASSERT_EQUALS(out(4,4), 1.00);
  }

  void test_train_on_slices_only_should_not_change_input_data(void)
  {
    norm->updControl("mrs_string/mode", "train");
    norm->updControl("mrs_string/domain", "slices");
    norm->myProcess(in,out);

    TS_ASSERT_EQUALS(out(0,0), 0000);
    TS_ASSERT_EQUALS(out(1,1), 1001);
    TS_ASSERT_EQUALS(out(2,2), 2002);
    TS_ASSERT_EQUALS(out(3,3), 3003);
    TS_ASSERT_EQUALS(out(4,4), 4004);

  }

  void test_should_be_able_to_train_slices_and_then_predict(void)
  {
    norm->updControl("mrs_string/domain", "slices");

    // Setup some data for the first training session
    clear_input_realvec();
    in(0,0) = 10000;
    norm->updControl("mrs_string/mode", "train");
    norm->myProcess(in,out);

    // Setup some data for the second training session
    clear_input_realvec();
    in(0,1) = 1000000;
    norm->updControl("mrs_string/mode", "train");
    norm->myProcess(in,out);

    // Predict the normalized matrix
    clear_input_realvec();
    norm->updControl("mrs_string/mode", "predict");
    norm->myProcess(in,out);

    TS_ASSERT_DELTA(out(2,2), 0.002002, 0.0000001);
  }

  void test_slices_maximums_control_should_be_set_properly_after_train_and_then_predict(void)
  {
    norm->updControl("mrs_string/domain", "slices");

    norm->updControl("mrs_string/mode", "train");
    norm->myProcess(in,out);

    norm->updControl("mrs_string/mode", "predict");
    norm->myProcess(in,out);

    realvec m = norm->getctrl("mrs_realvec/maximums")->to<mrs_realvec>();
    TS_ASSERT_EQUALS(m(0), 4004);

  }

  void test_slices_minimums_control_should_be_set_properly_after_train_and_then_predict(void)
  {
    norm->updControl("mrs_string/domain", "slices");

    norm->updControl("mrs_string/mode", "train");
    norm->myProcess(in,out);

    norm->updControl("mrs_string/mode", "predict");
    norm->myProcess(in,out);

    realvec m = norm->getctrl("mrs_realvec/minimums")->to<mrs_realvec>();
    TS_ASSERT_EQUALS(m(0), 0);

  }

};

