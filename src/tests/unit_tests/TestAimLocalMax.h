// TestAimLocalMax.h
//
// Halfwave rectification, compression and lowpass filtering
//
// (c) 2009 - sness@sness.net - GPL
//

#include <cxxtest/TestSuite.h>
#include <cstdio>
#include "Collection.h"
#include "MarSystemManager.h"
#include "AimLocalMax.h"
#include "CommandLineOptions.h"

#include <vector>

using namespace std;
using namespace Marsyas;

class AimLocalMax_runner : public CxxTest::TestSuite
{
public:
  realvec in,out;
  MarSystemManager mng;
  AimLocalMax *aimc_localmax;

  void
  setUp()
  {
	aimc_localmax = new AimLocalMax("aimc_localmax");
	aimc_localmax->updctrl("mrs_real/israte", 44100.0);
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
	out.create(2,length);	

	// Fill up the input realvec with a sine wave
	for (int i = 0; i < length; i++) {
	  double d = sin(i/((double)length)*(PI*2.0)*((double)length/44100.0)*1000.0);
	  in(0,i) = d * i;
	}

	// Process the data with the AIMC_LocalMax algorithm
 	aimc_localmax->myProcess(in,out);

	// There should be one strobe point at index 15, everything else
	// should be 0.
  	TS_ASSERT_DELTA(out(0,0),0,0.001);
  	TS_ASSERT_DELTA(out(0,14),0,0.001);
  	TS_ASSERT_DELTA(out(0,15),1,0.001);
  	TS_ASSERT_DELTA(out(0,16),0,0.001);
  }



};
