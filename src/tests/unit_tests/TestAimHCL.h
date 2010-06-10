// TestAimHCL.h
//
// Halfwave rectification, compression and lowpass filtering
//
// (c) 2009 - sness@sness.net - GPL
//

#include <cxxtest/TestSuite.h>
#include <cstdio>
#include "Collection.h"
#include "MarSystemManager.h"
#include "AimHCL.h"
#include "CommandLineOptions.h"

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
	aimc_hcl->updctrl("mrs_real/israte", 44100.0);
  }

  //
  // Test one single buffer of audio with a 1000Hz tone
  //
  void test_one_window_1000hz(void) 
  {
	// Create input and output realvecs
	int length = 512;
	realvec input_realvec;
	in.create(1,length);
	out.create(1,length);	

	// Fill up the input realvec with a sine wave
	for (int i = 0; i < length; i++) {
	  double d = sin(i/((double)length)*(PI*2.0)*((double)length/44100.0)*1000.0);
	  in(0,i) = d;
	}

	// Process the data with the AIMC_HCL algorithm
 	aimc_hcl->myProcess(in,out);

	// Check to see if the values are correct
  	TS_ASSERT_DELTA(out(0,0),0,0.001);
  	TS_ASSERT_DELTA(out(0,1),0.0035,0.001);
  	TS_ASSERT_DELTA(out(0,2),0.0128,0.001);
  	TS_ASSERT_DELTA(out(0,3),0.0294,0.001);
  	TS_ASSERT_DELTA(out(0,4),0.0537,0.001);
  }

  //
  // Test a sample file, test.wav
  //
  void test_wav_file(void) {
    realvec out;

    MarSystemManager mng;

    MarSystem* net = mng.create("Series", "net");
  
    net->addMarSystem(mng.create("SoundFileSource", "src"));
    net->addMarSystem(mng.create("AimHCL", "aimhcl"));

    net->updctrl("SoundFileSource/src/mrs_string/filename", "./tests/unit_tests/files/test.wav");

    net->tick();

    out = net->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();

	// Check to see if the generated data is correct.  
    //
    // This data was generated with the AIM-C version of HCL.  I
    // chose three points from the beginning, middle and end of the
    // range, and compare the Marsyas HCL implementation with these
    // results
  	TS_ASSERT_DELTA(out(0,0),0.0,0.001);
  	TS_ASSERT_DELTA(out(0,1),0.0,0.001);
  	TS_ASSERT_DELTA(out(0,2),0.0,0.001);

  	TS_ASSERT_DELTA(out(0,254),0.0112265,0.001);
  	TS_ASSERT_DELTA(out(0,255),0.0116393,0.001);
  	TS_ASSERT_DELTA(out(0,256),0.0117777,0.001);

  	TS_ASSERT_DELTA(out(0,509),0.000508549,0.001);
  	TS_ASSERT_DELTA(out(0,510),0.000445525,0.001);
  	TS_ASSERT_DELTA(out(0,511),0.00038975,0.001);

    //
    // Tick the network 3 more times to see if the values still line up
    //
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
  	TS_ASSERT_DELTA(out(0,0),0.00683525,0.001);
  	TS_ASSERT_DELTA(out(0,1),0.00769413,0.001);
  	TS_ASSERT_DELTA(out(0,2),0.00886946,0.001);

  	TS_ASSERT_DELTA(out(0,254),0.00756773,0.001);
  	TS_ASSERT_DELTA(out(0,255),0.00686076,0.001);
  	TS_ASSERT_DELTA(out(0,256),0.00618908,0.001);

  	TS_ASSERT_DELTA(out(0,509),0.0189993,0.001);
  	TS_ASSERT_DELTA(out(0,510),0.0208265,0.001);
  	TS_ASSERT_DELTA(out(0,511),0.0224114,0.001);
    
    
    delete net;

  }



};
