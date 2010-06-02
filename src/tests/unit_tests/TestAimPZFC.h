// TestAimPZFC.h
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
#include "AimPZFC.h"
#include "CommandLineOptions.h"

#include <vector>

using namespace std;
using namespace Marsyas;

class AimPZFC_runner : public CxxTest::TestSuite
{
public:
  realvec in,out;
  MarSystemManager mng;
  AimPZFC *aimc_pzfc;

  void
  setUp()
  {
	aimc_pzfc = new AimPZFC("aimc_pzfc");
	aimc_pzfc->updctrl("mrs_real/israte", 44100.0);
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

	// Process the data with the AIMC_PZFC algorithm
 	aimc_pzfc->myProcess(in,out);

	// Check to see if the pitch is correct.
	// double pitch = out(0,0);
  	// TS_ASSERT_DELTA(pitch,989.011,0.001);
  }



};
