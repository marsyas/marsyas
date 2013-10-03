// TestCARFAC.h
//
// Dick Lyon's Pole-Zero Filter Cascade - implemented in C++ by Tom
// Walters from the AIM-MAT module based on Dick Lyon's code.
//
// (c) 2009 - sness@sness.net - GPL
//


#include <cxxtest/TestSuite.h>
#include <cstdio>
#include <marsyas/Collection.h>
#include <marsyas/system/MarSystemManager.h>
#include <marsyas/marsystems/CARFAC.h>
#include <marsyas/CommandLineOptions.h>

#include <vector>

using namespace std;
using namespace Marsyas;

class CARFAC_runner : public CxxTest::TestSuite
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
    net->addMarSystem(mng.create("CARFAC", "carfac"));

    net->updControl("SoundFileSource/src/mrs_string/filename", "files/binaural.wav");
    net->tick();

    out = net->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();
    // cout << out << endl;
    // Check to see if the generated data is correct.
    //
    // This data was generated with the MATLAB version of CARFAC.  I
    // chose 10 points at the start (mic 1) and 10 points at the end
    // (end of mic 2) to compare within a delta of 0.001.

    // First mic
    TS_ASSERT_DELTA(out(0,511),0.025536,0.001);
    TS_ASSERT_DELTA(out(1,511),0.000000,0.001);
    TS_ASSERT_DELTA(out(2,511),0.000000,0.001);
    TS_ASSERT_DELTA(out(3,511),0.054641,0.001);
    TS_ASSERT_DELTA(out(4,511),0.000000,0.001);
    TS_ASSERT_DELTA(out(5,511),0.095745,0.001);
    TS_ASSERT_DELTA(out(6,511),0.000000,0.001);
    TS_ASSERT_DELTA(out(7,511),0.222046,0.001);
    TS_ASSERT_DELTA(out(8,511),0.000000,0.001);
    TS_ASSERT_DELTA(out(9,511),0.280376,0.001);

    // Second mic
    TS_ASSERT_DELTA(out(182,511),0.000000,0.001);
    TS_ASSERT_DELTA(out(183,511),0.000000,0.001);
    TS_ASSERT_DELTA(out(184,511),0.000000,0.001);
    TS_ASSERT_DELTA(out(185,511),0.091153,0.001);
    TS_ASSERT_DELTA(out(186,511),0.153255,0.001);
    TS_ASSERT_DELTA(out(187,511),0.070381,0.001);
    TS_ASSERT_DELTA(out(188,511),0.004470,0.001);
    TS_ASSERT_DELTA(out(189,511),0.000000,0.001);
    TS_ASSERT_DELTA(out(190,511),0.000000,0.001);
    TS_ASSERT_DELTA(out(191,511),0.000000,0.001);

    delete net;

  }



};
