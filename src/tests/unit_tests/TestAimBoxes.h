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

    net->updControl("SoundFileSource/src/mrs_string/filename", "files/test.wav");

    net->tick();

    out = net->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();

	// Check to see if the generated data is correct.  
    //
    // This data was generated with the AIM-C version of SAI.  I
    // chose three points from the beginning, middle and end of the
    // range, and compare the Marsyas SAI implementation with these
    // results
  	TS_ASSERT_DELTA(out(0,0),0.203709,0.001);
  	TS_ASSERT_DELTA(out(0,1),0.168188,0.001);
  	TS_ASSERT_DELTA(out(0,2),0.149391,0.001);
  	TS_ASSERT_DELTA(out(0,24),0.0612249,0.001);
  	TS_ASSERT_DELTA(out(0,25),0.0668861,0.001);
  	TS_ASSERT_DELTA(out(0,26),0.0725046,0.001);
  	TS_ASSERT_DELTA(out(0,45),0.079052,0.001);
  	TS_ASSERT_DELTA(out(0,46),0.075481,0.001);
  	TS_ASSERT_DELTA(out(0,47),0.071827,0.001);

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
  	TS_ASSERT_DELTA(out(0,0),0.300160,0.001);
  	TS_ASSERT_DELTA(out(0,1),0.269779,0.001);
  	TS_ASSERT_DELTA(out(0,2),0.252419,0.001);
  	TS_ASSERT_DELTA(out(0,24),0.130430,0.001);
  	TS_ASSERT_DELTA(out(0,25),0.134411,0.001);
  	TS_ASSERT_DELTA(out(0,26),0.137622,0.001);
  	TS_ASSERT_DELTA(out(0,45),0.132472,0.001);
  	TS_ASSERT_DELTA(out(0,46),0.132383,0.001);
  	TS_ASSERT_DELTA(out(0,47),0.132838,0.001);
    
    
    delete net;

  }


};
