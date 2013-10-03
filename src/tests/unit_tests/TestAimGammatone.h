// TestAimGammatone.h
//
// Slaney's gammatone filterbank
//
// (c) 2009 - sness@sness.net - GPL
//

#include <cxxtest/TestSuite.h>
#include <cstdio>
#include <marsyas/Collection.h>
#include <marsyas/system/MarSystemManager.h>
#include <marsyas/marsystems/AimGammatone.h>
#include <marsyas/CommandLineOptions.h>

#include <vector>

using namespace std;
using namespace Marsyas;

class AimGammatone_runner : public CxxTest::TestSuite
{
public:

  void
  setUp()
  {
  }

  //
  // Test one single buffer of audio with a 1000Hz tone
  //
  void test_one_window_1000hz(void)
  {
    MarSystemManager mng;
    AimGammatone *aimc_gammatone;
    realvec in,out;

    aimc_gammatone = new AimGammatone("aimc_gammatone");
    aimc_gammatone->updControl("mrs_real/israte", 44100.0);

    // Create input and output realvecs
    int length = 512;
    realvec input_realvec;
    in.create(1,length);
    out.create(400,length);

    // Fill up the input realvec with a sine wave
    for (int i = 0; i < length; i++) {
      double d = sin(i * 1000.0 / 44100.0 * (PI*2.0));
      in(0,i) = d;
    }

    // Process the data with the AIMC_Gammatone algorithm
    aimc_gammatone->myProcess(in,out);

    // Check to see if the generated data is correct.
    //
    // sness - TODO. I just generated this data with the AimGammatone
    // module.  We should check this against AIM-C, but since it's a
    // generated sine wave, this is not super easy to do.
    TS_ASSERT_DELTA(out(0,0),0.0,0.001);
    TS_ASSERT_DELTA(out(0,254),-0.00660633,0.0001);
    TS_ASSERT_DELTA(out(0,255),-0.00665357,0.0001);
    TS_ASSERT_DELTA(out(0,256),-0.00669978,0.0001);
    TS_ASSERT_DELTA(out(0,509),0.0149255,0.001);
    TS_ASSERT_DELTA(out(0,510),0.0149466,0.001);
    TS_ASSERT_DELTA(out(0,511),0.0149655,0.001);

    delete aimc_gammatone;
  }

  //
  // Test a sample file, test.wav
  //
  void test_wav_file(void) {
    realvec out;

    MarSystemManager mng;

    MarSystem* net = mng.create("Series", "net");

    net->addMarSystem(mng.create("SoundFileSource", "src"));
    net->addMarSystem(mng.create("AimGammatone", "aimgammatone")); \

    net->updControl("SoundFileSource/src/mrs_string/filename", "files/test.wav");

    net->tick();

    out = net->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();
    // cout << out << endl;

    // Check to see if the generated data is correct.
    //
    // This data was generated with the AIM-C version of Gammatone.  I
    // chose three points from the beginning, middle and end of the
    // range, and compare the Marsyas Gammatone implementation with these
    // results
    TS_ASSERT_DELTA(out(0,0),0.0,0.001);
    TS_ASSERT_DELTA(out(0,1),0.0,0.001);
    TS_ASSERT_DELTA(out(0,2),0.0,0.001);

    TS_ASSERT_DELTA(out(0,254),-0.000436661,0.001);
    TS_ASSERT_DELTA(out(0,255),-0.000443985,0.001);
    TS_ASSERT_DELTA(out(0,256),-0.000451316,0.001);

    TS_ASSERT_DELTA(out(0,509),0.00257957,0.001);
    TS_ASSERT_DELTA(out(0,510),0.00259566,0.001);
    TS_ASSERT_DELTA(out(0,511),0.00261137,0.001);

    //
    // Tick the network 3 more times to see if the values still line up
    //
    net->tick();
    net->tick();
    net->tick();
    out = net->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();

    // Check to see if the generated data is correct.
    //
    // This data was generated with the AIM-C version of Gammatone.  I
    // chose three points from the beginning, middle and end of the
    // range, and compare the Marsyas Gammatone implementation with these
    // results
    TS_ASSERT_DELTA(out(0,0),-0.00344842,0.001);
    TS_ASSERT_DELTA(out(0,1),-0.00337253,0.001);
    TS_ASSERT_DELTA(out(0,2),-0.00329647,0.001);

    TS_ASSERT_DELTA(out(0,254),0.00590044,0.001);
    TS_ASSERT_DELTA(out(0,255),0.00587644,0.001);
    TS_ASSERT_DELTA(out(0,256),0.00585197,0.001);

    TS_ASSERT_DELTA(out(0,509),-0.00582105,0.001);
    TS_ASSERT_DELTA(out(0,510),-0.00583112,0.001);
    TS_ASSERT_DELTA(out(0,511),-0.00584065,0.001);

    delete net;

  }



};
