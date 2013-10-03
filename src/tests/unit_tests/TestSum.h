// TestSum.h

//
// The Sum class sums up various parts of the input
//

//
// (c) 2009 - sness@sness.net - GPL
//

#include <cxxtest/TestSuite.h>

#include <cstdio>
#include <marsyas/Collection.h>
#include <marsyas/system/MarSystemManager.h>
#include <marsyas/marsystems/Sum.h>
#include <marsyas/CommandLineOptions.h>

#include <vector>

using namespace std;
using namespace Marsyas;

class Sum_runner : public CxxTest::TestSuite
{
public:

  void
  setUp()
  {
  }

  // "destructor"
  void tearDown()
  {
  }


  //
  // Should be able to add up observations like it used to
  //
  void test_orig_sum(void)
  {
    const static int numObservations = 5;
    const static int numSamples = 1;

    MarSystemManager mng;

    // A series containing a RealvecSource followed by an AubioYin
    MarSystem* net = mng.create("Series", "series");
    net->addMarSystem(mng.create("RealvecSource", "src"));
    net->addMarSystem(mng.create("Sum", "sum"));

    net->updControl("mrs_natural/inObservations", numObservations);
    net->updControl("mrs_natural/inSamples", numSamples);

    // Setup an input realvec for data to feed into the Marsystem
    realvec in;
    in.create(0.0,numObservations,numSamples);
    in(0,0) = 0.1;
    in(1,0) = 1.1;
    in(2,0) = 2.1;
    in(3,0) = 3.1;
    in(4,0) = 4.1;

    // cout << in << endl;

    // Copy in to the RealvecSource
    net->updControl("RealvecSource/src/mrs_realvec/data", in);

    // Tick the network once to push the data through the network
    net->tick();

    // Get the output of the network
    realvec out;
    out = net->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();

    // cout << out;

    TS_ASSERT_DELTA(out(0,0),10.5,0.001);

    delete net;
  }

  //
  // Should be able to add up observations
  //
  void test_add_up_observations(void)
  {
    const static int numObservations = 3;
    const static int numSamples = 3;

    MarSystemManager mng;

    // A series containing a RealvecSource followed by an AubioYin
    MarSystem* net = mng.create("Series", "series");
    net->addMarSystem(mng.create("RealvecSource", "src"));
    net->addMarSystem(mng.create("Sum", "sum"));

    net->updControl("mrs_natural/inObservations", numObservations);
    net->updControl("mrs_natural/inSamples", numSamples);

    // Setup an input realvec for data to feed into the Marsystem
    realvec in;
    in.create(0.0,numObservations,numSamples);
    in(0,0) = 0.1;
    in(0,1) = 0.2;
    in(0,2) = 0.3;

    in(1,0) = 1.1;
    in(1,1) = 1.2;
    in(1,2) = 1.3;

    in(2,0) = 2.1;
    in(2,1) = 2.2;
    in(2,2) = 2.3;

    net->updControl("Sum/sum/mrs_string/mode", "sum_observations");

    // Copy in to the RealvecSource
    net->updControl("RealvecSource/src/mrs_realvec/data", in);

    // Tick the network once to push the data through the network
    net->tick();

    // Get the output of the network
    realvec out;
    out = net->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();

    // cout << out;
    TS_ASSERT_EQUALS(out.getCols(),1);
    TS_ASSERT_EQUALS(out.getRows(),3);

    TS_ASSERT_DELTA(out(0,0),0.6,0.001);
    TS_ASSERT_DELTA(out(1,0),3.6,0.001);
    TS_ASSERT_DELTA(out(2,0),6.6,0.001);
    delete net;
  }

  //
  // Should be able to add up samples
  //
  void test_add_up_samples(void)
  {
    const static int numObservations = 3;
    const static int numSamples = 3;

    MarSystemManager mng;

    // A series containing a RealvecSource followed by an AubioYin
    MarSystem* net = mng.create("Series", "series");
    net->addMarSystem(mng.create("RealvecSource", "src"));
    net->addMarSystem(mng.create("Sum", "sum"));

    net->updControl("mrs_natural/inObservations", numObservations);
    net->updControl("mrs_natural/inSamples", numSamples);

    // Setup an input realvec for data to feed into the Marsystem
    realvec in;
    in.create(0.0,numObservations,numSamples);
    in(0,0) = 0.1;
    in(0,1) = 0.2;
    in(0,2) = 0.3;

    in(1,0) = 1.1;
    in(1,1) = 1.2;
    in(1,2) = 1.3;

    in(2,0) = 2.1;
    in(2,1) = 2.2;
    in(2,2) = 2.3;

    net->updControl("Sum/sum/mrs_string/mode", "sum_samples");

    // Copy in to the RealvecSource
    net->updControl("RealvecSource/src/mrs_realvec/data", in);

    // Tick the network once to push the data through the network
    net->tick();

    // Get the output of the network
    realvec out;
    out = net->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();

    // cout << out;
    TS_ASSERT_EQUALS(out.getCols(),3);
    TS_ASSERT_EQUALS(out.getRows(),1);

    TS_ASSERT_DELTA(out(0,0),3.3,0.001);
    TS_ASSERT_DELTA(out(0,1),3.6,0.001);
    TS_ASSERT_DELTA(out(0,2),3.9,0.001);
    delete net;
  }

  //
  // Should be able to add up observations
  //
  void test_add_up_whole(void)
  {
    const static int numObservations = 3;
    const static int numSamples = 3;

    MarSystemManager mng;

    // A series containing a RealvecSource followed by an AubioYin
    MarSystem* net = mng.create("Series", "series");
    net->addMarSystem(mng.create("RealvecSource", "src"));
    net->addMarSystem(mng.create("Sum", "sum"));

    net->updControl("mrs_natural/inObservations", numObservations);
    net->updControl("mrs_natural/inSamples", numSamples);

    // Setup an input realvec for data to feed into the Marsystem
    realvec in;
    in.create(0.0,numObservations,numSamples);
    in(0,0) = 0.1;
    in(0,1) = 0.2;
    in(0,2) = 0.3;

    in(1,0) = 1.1;
    in(1,1) = 1.2;
    in(1,2) = 1.3;

    in(2,0) = 2.1;
    in(2,1) = 2.2;
    in(2,2) = 2.3;

    net->updControl("Sum/sum/mrs_string/mode", "sum_whole");

    // Copy in to the RealvecSource
    net->updControl("RealvecSource/src/mrs_realvec/data", in);

    // Tick the network once to push the data through the network
    net->tick();

    // Get the output of the network
    realvec out;
    out = net->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();

    // cout << out;
    TS_ASSERT_EQUALS(out.getCols(),1);
    TS_ASSERT_EQUALS(out.getRows(),1);

    TS_ASSERT_DELTA(out(0,0),10.8,0.001);
    delete net;
  }



};
