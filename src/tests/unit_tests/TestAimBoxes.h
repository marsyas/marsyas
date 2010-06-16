// TestAimBoxes.h
//
// 'Box-cutting' routine to generate dense features
//
// (c) 2009 - sness@sness.net - GPL
//

#include <cxxtest/TestSuite.h>
#include <cstdio>
#include "Collection.h"
#include "MarSystemManager.h"
#include "AimBoxes.h"
#include "CommandLineOptions.h"

#include <vector>

using namespace std;
using namespace Marsyas;

class AimBoxes_runner : public CxxTest::TestSuite
{
public:
  realvec in,out;

  void
  setUp()
  {
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
	net->addMarSystem(mng.create("AimLocalMax", "aimlocalmax"));
	net->addMarSystem(mng.create("AimSAI", "aimsai"));
	net->addMarSystem(mng.create("AimBoxes", "aimboxes"));

    net->updctrl("SoundFileSource/src/mrs_string/filename", "./tests/unit_tests/files/test.wav");

    net->tick();

    out = net->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();

	// Check to see if the generated data is correct.  
    //
    // This data was generated with the AIM-C version of SAI.  I
    // chose three points from the beginning, middle and end of the
    // range, and compare the Marsyas SAI implementation with these
    // results
  	TS_ASSERT_DELTA(out(0,0),0.000271244,0.001);
  	TS_ASSERT_DELTA(out(0,1),0.000429452,0.001);
  	TS_ASSERT_DELTA(out(0,2),0.000647149,0.001);
  	TS_ASSERT_DELTA(out(0,24),0.0513575,0.001);
  	TS_ASSERT_DELTA(out(0,25),0.0501243,0.001);
  	TS_ASSERT_DELTA(out(0,26),0.0487665,0.001);
  	TS_ASSERT_DELTA(out(0,45),0.0183585,0.001);
  	TS_ASSERT_DELTA(out(0,46),0.0170586,0.001);
  	TS_ASSERT_DELTA(out(0,47),0.0158053,0.001);

    //
    // Tick the network 3 more times to see if the values still line up
    //
    net->tick();
    net->tick();
    net->tick();
    out = net->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();
    
	// Check to see if the generated data is correct.  
    
    // This data was generated with the AIM-C version of SAI.  I
    // chose three points from the beginning, middle and end of the
    // range, and compare the Marsyas SAI implementation with these
    // results
  	TS_ASSERT_DELTA(out(0,0),0.045335,0.001);
  	TS_ASSERT_DELTA(out(0,1),0.189983,0.001);
  	TS_ASSERT_DELTA(out(0,2),0.118463,0.001);
  	TS_ASSERT_DELTA(out(0,24),0.198095,0.001);
  	TS_ASSERT_DELTA(out(0,25),0.195077,0.001);
  	TS_ASSERT_DELTA(out(0,26),0.191725,0.001);
  	TS_ASSERT_DELTA(out(0,45),0.095455,0.001);
  	TS_ASSERT_DELTA(out(0,46),0.090432,0.001);
  	TS_ASSERT_DELTA(out(0,47),0.0861151,0.001);
    
    
    delete net;

  }


};
