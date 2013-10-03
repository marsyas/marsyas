// TestAimLocalMax.h
//
// Halfwave rectification, compression and lowpass filtering
//
// (c) 2009 - sness@sness.net - GPL
//

#include <cxxtest/TestSuite.h>
#include <cstdio>
#include <marsyas/Collection.h>
#include <marsyas/system/MarSystemManager.h>
#include <marsyas/marsystems/AimLocalMax.h>
#include <marsyas/CommandLineOptions.h>

#include <vector>

using namespace std;
using namespace Marsyas;

class AimLocalMax_runner : public CxxTest::TestSuite
{
public:
  realvec in,out;

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
    MarSystemManager mng;

    MarSystem* net = mng.create("Series", "net");

    net->addMarSystem(mng.create("SoundFileSource", "src"));
    net->addMarSystem(mng.create("AimPZFC", "aimpfzc"));
    net->addMarSystem(mng.create("AimHCL", "aimhcl"));
    net->addMarSystem(mng.create("AimLocalMax", "aimlocalmax"));

    net->updControl("SoundFileSource/src/mrs_string/filename", "files/test.wav");

    net->tick();

    out = net->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();

    // Check to see if the generated data is correct.
    //
    // This data was generated with the AIM-C version of Gammatone.  I
    // chose three points from the beginning, middle and end of the
    // range, and compare the Marsyas Gammatone implementation with these
    // results
    TS_ASSERT_DELTA(out(156,0),0.0,0.001);
    TS_ASSERT_DELTA(out(156,289),0.0,0.001);
    TS_ASSERT_DELTA(out(156,290),1.0,0.001);
    TS_ASSERT_DELTA(out(156,291),0.0,0.001);

    delete net;

  }




};
