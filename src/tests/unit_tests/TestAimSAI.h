// TestAimSAI.h
//
// Dick Lyon's Pole-Zero Filter Cascade - implemented in C++ by Tom
// Walters from the AIM-MAT module based on Dick Lyon's code.
//
// (c) 2009 - sness@sness.net - GPL
//


#include <cxxtest/TestSuite.h>
#include <cstdio>
#include "Collection.h"
#include "MarSystemManager.h"
#include "AimSAI.h"
#include "CommandLineOptions.h"

#include <vector>

using namespace std;
using namespace Marsyas;

class AimSAI_runner : public CxxTest::TestSuite
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

    net->updControl("SoundFileSource/src/mrs_string/filename", "files/test.wav");

    net->tick();

    out = net->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();

	// Check to see if the generated data is correct.  
    //
    // This data was generated with the AIM-C version of SAI.  I
    // chose three points from the beginning, middle and end of the
    // range, and compare the Marsyas SAI implementation with these
    // results
  	TS_ASSERT_DELTA(out(0,0),0.000493759,0.001);
  	TS_ASSERT_DELTA(out(0,1),0.000492902,0.001);
  	TS_ASSERT_DELTA(out(0,2),0.000490549,0.001);
  	TS_ASSERT_DELTA(out(0,254),0.0,0.001);
  	TS_ASSERT_DELTA(out(0,255),0.0,0.001);
  	TS_ASSERT_DELTA(out(0,256),0.0,0.001);
  	TS_ASSERT_DELTA(out(0,509),0.0,0.001);
  	TS_ASSERT_DELTA(out(0,510),0.0,0.001);
  	TS_ASSERT_DELTA(out(0,511),0.0,0.001);

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
  	TS_ASSERT_DELTA(out(0,0),0.0467726,0.001);
  	TS_ASSERT_DELTA(out(0,1),0.0467948,0.001);
  	TS_ASSERT_DELTA(out(0,2),0.0468055,0.001);
  	TS_ASSERT_DELTA(out(0,254),1.22491e-13,0.001);
  	TS_ASSERT_DELTA(out(0,255),1.03889e-13,0.001);
  	TS_ASSERT_DELTA(out(0,256),8.81089e-14,0.001);
  	TS_ASSERT_DELTA(out(0,509),0.264658,0.001);
  	TS_ASSERT_DELTA(out(0,510),0.263065,0.001);
  	TS_ASSERT_DELTA(out(0,511),0.261408,0.001);
    
    
    delete net;

  }



};
