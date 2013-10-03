// TestAimPZFC.h
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
#include <marsyas/marsystems/AimPZFC.h>
#include <marsyas/CommandLineOptions.h>

#include <vector>

using namespace std;
using namespace Marsyas;

class AimPZFC_runner : public CxxTest::TestSuite
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
  // Test one single buffer of audio with a 1000Hz tone
  //
  void test_one_window_1000hz(void)
  {
    MarSystemManager mng;
    AimPZFC *aimc_pzfc;
    realvec in,out;

    aimc_pzfc = new AimPZFC("aimc_pzfc");
    aimc_pzfc->updControl("mrs_real/israte", 44100.0);

    // Create input and output realvecs
    int length = 512;
    realvec input_realvec;
    in.create(1,length);
    out.create(156,length);

    // Fill up the input realvec with a sine wave
    for (int i = 0; i < length; i++) {
      double d = sin(i * 1000.0 / 44100.0 * (PI*2.0));
      in(0,i) = d;
    }

    // Process the data with the AIMC_PZFC algorithm
    aimc_pzfc->myProcess(in,out);

    // Check to see if the generated data is correct.
    //
    // sness - TODO. I just generated this data with the AimPZFC
    // module.  We should check this against AIM-C, but since it's a
    // generated sine wave, this is not super easy to do.
    TS_ASSERT_DELTA(out(0,0),0.0,0.001);
    TS_ASSERT_DELTA(out(0,269),0.0257063,0.0001);
    TS_ASSERT_DELTA(out(0,270),0.0267372,0.0001);
    TS_ASSERT_DELTA(out(0,271),0.0278072,0.0001);
    TS_ASSERT_DELTA(out(0,508),0.120117,0.001);
    TS_ASSERT_DELTA(out(0,509),0.105835,0.001);
    TS_ASSERT_DELTA(out(0,510),0.0916747,0.001);
    TS_ASSERT_DELTA(out(0,511),0.0762939,0.001);

    delete aimc_pzfc;
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

    net->updControl("SoundFileSource/src/mrs_string/filename", "files/test.wav");
    net->tick();

    out = net->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();

    // Check to see if the generated data is correct.
    //
    // This data was generated with the AIM-C version of PZFC.  I
    // chose three points from the beginning, middle and end of the
    // range, and compare the Marsyas PZFC implementation with these
    // results
    TS_ASSERT_DELTA(out(0,0),0.0,0.001);
    TS_ASSERT_DELTA(out(0,1),0.0,0.001);
    TS_ASSERT_DELTA(out(0,2),0.0,0.001);
    TS_ASSERT_DELTA(out(0,254),0.000407077,0.001);
    TS_ASSERT_DELTA(out(0,255),0.000423789,0.001);
    TS_ASSERT_DELTA(out(0,256),0.000440776,0.001);
    TS_ASSERT_DELTA(out(0,509),-0.759234,0.001);
    TS_ASSERT_DELTA(out(0,510),-0.763749,0.001);
    TS_ASSERT_DELTA(out(0,511),-0.768143,0.001);

    //
    // Tick the network 3 more times to see if the values still line up
    //
    net->tick();
    net->tick();
    net->tick();
    out = net->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();

    // Check to see if the generated data is correct.
    //
    // This data was generated with the AIM-C version of PZFC.  I
    // chose three points from the beginning, middle and end of the
    // range, and compare the Marsyas PZFC implementation with these
    // results
    TS_ASSERT_DELTA(out(0,0),0.226439,0.001);
    TS_ASSERT_DELTA(out(0,1),0.225951,0.001);
    TS_ASSERT_DELTA(out(0,2),0.225707,0.001);

    TS_ASSERT_DELTA(out(0,254),-0.0275879,0.001);
    TS_ASSERT_DELTA(out(0,255),-0.0258179,0.001);
    TS_ASSERT_DELTA(out(0,256),-0.0241699,0.001);

    TS_ASSERT_DELTA(out(0,509),-0.0746154,0.001);
    TS_ASSERT_DELTA(out(0,510),-0.0740356,0.001);
    TS_ASSERT_DELTA(out(0,511),-0.0734558,0.001);


    delete net;

  }



};
