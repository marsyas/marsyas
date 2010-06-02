// TestAimGammatone.h
//
// Slaney's gammatone filterbank
//
// (c) 2009 - sness@sness.net - GPL
//

#include <cxxtest/TestSuite.h>
#include <cstdio>
#include "Collection.h"
#include "MarSystemManager.h"
#include "AimGammatone.h"
#include "CommandLineOptions.h"

#include <vector>

using namespace std;
using namespace Marsyas;

class AimGammatone_runner : public CxxTest::TestSuite
{
public:
  realvec in,out;
  MarSystemManager mng;
  AimGammatone *aimc_gammatone;

  void
  setUp()
  {
	aimc_gammatone = new AimGammatone("aimc_gammatone");
	aimc_gammatone->updctrl("mrs_real/israte", 44100.0);
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

	// Process the data with the AIMC_Gammatone algorithm
 	// aimc_gammatone->myProcess(in,out);

	// Check to see if the pitch is correct.
	// double pitch = out(0,0);
  	// TS_ASSERT_DELTA(pitch,989.011,0.001);
  }



};
