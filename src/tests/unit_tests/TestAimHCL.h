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
  	TS_ASSERT_DELTA(out(0,1),0.00350689,0.001);
  	TS_ASSERT_DELTA(out(0,2),0.0128542,0.001);
  	TS_ASSERT_DELTA(out(0,3),0.0294149,0.001);
  	TS_ASSERT_DELTA(out(0,4),0.0537784,0.001);
  }



};
