// TestAimHCL.h
//
// Halfwave rectification, compression and lowpass filtering
//
// (c) 2009 - sness@sness.net - GPL
//

#include <cxxtest/TestSuite.h>
#include <cstdio>
#include <marsyas/Collection.h>
#include <marsyas/system/MarSystemManager.h>
#include <marsyas/marsystems/AimHCL.h>
#include <marsyas/CommandLineOptions.h>

#include <vector>

using namespace std;
using namespace Marsyas;

class AimHCL_runner : public CxxTest::TestSuite
{
public:
  realvec in,out;
  MarSystemManager mng;
  AimHCL *aimc_hcl;

  void
  setUp()
  {
    aimc_hcl = new AimHCL("aimc_hcl");
    aimc_hcl->updControl("mrs_real/israte", 44100.0);
  }

  // "destructor"
  void tearDown()
  {
    delete aimc_hcl;
  }


  //
  // Test a sample file, test.wav
  //
  void test_wav_file(void) {
    realvec out;

    MarSystemManager mng;

    MarSystem* net = mng.create("Series", "net");

    net->addMarSystem(mng.create("SoundFileSource", "src"));
    net->addMarSystem(mng.create("AimPZFC", "aimpzfc"));
    net->addMarSystem(mng.create("AimHCL", "aimhcl"));

    net->updControl("SoundFileSource/src/mrs_string/filename", "files/test.wav");

    net->tick();

    out = net->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();
    // cout << out << endl;

    // Check to see if the generated data is correct.
    //
    // This data was generated with the AIM-C version of HCL.  I
    // chose three points from the beginning, middle and end of the
    // range, and compare the Marsyas HCL implementation with these
    // results
    TS_ASSERT_DELTA(out(0,0),0.0,0.001);
    TS_ASSERT_DELTA(out(0,1),0.0,0.001);
    TS_ASSERT_DELTA(out(0,2),0.0,0.001);

    TS_ASSERT_DELTA(out(0,254),0.000261777,0.001);
    TS_ASSERT_DELTA(out(0,255),0.000274693,0.001);
    TS_ASSERT_DELTA(out(0,256),0.000287972,0.001);

    TS_ASSERT_DELTA(out(0,509),4.71047e-18,0.001);
    TS_ASSERT_DELTA(out(0,510),3.98848e-18,0.001);
    TS_ASSERT_DELTA(out(0,511),3.37708e-18,0.001);

    // //
    // // Tick the network 3 more times to see if the values still line up
    // //
    net->tick();
    net->tick();
    net->tick();
    out = net->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();

    // Check to see if the generated data is correct.
    //
    // This data was generated with the AIM-C version of HCL.  I
    // chose three points from the beginning, middle and end of the
    // range, and compare the Marsyas HCL implementation with these
    // results
    TS_ASSERT_DELTA(out(0,0),0.224228,0.001);
    TS_ASSERT_DELTA(out(0,1),0.224561,0.001);
    TS_ASSERT_DELTA(out(0,2),0.224828,0.001);

    TS_ASSERT_DELTA(out(0,254),8.68093e-13,0.001);
    TS_ASSERT_DELTA(out(0,255),7.35964e-13,0.001);
    TS_ASSERT_DELTA(out(0,256),6.23925e-13,0.001);

    TS_ASSERT_DELTA(out(0,509),1.89546e-10,0.001);
    TS_ASSERT_DELTA(out(0,510),1.60974e-10,0.001);
    TS_ASSERT_DELTA(out(0,511),1.36701e-10,0.001);


    delete net;

  }



};
