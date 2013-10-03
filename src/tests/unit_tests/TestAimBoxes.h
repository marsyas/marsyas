// TestAimBoxes.h
//
// 'Box-cutting' routine to generate dense features
//
// (c) 2009 - sness@sness.net - GPL
//

#include <cxxtest/TestSuite.h>
#include <cstdio>
#include <marsyas/Collection.h>
#include <marsyas/system/MarSystemManager.h>
#include <marsyas/marsystems/AimBoxes.h>
#include <marsyas/CommandLineOptions.h>

#include <vector>

using namespace std;
using namespace Marsyas;

class AimBoxes_runner : public CxxTest::TestSuite
{
public:
  realvec in,out;

  void
  setUp()
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
    net->addMarSystem(mng.create("AimBoxes", "aimboxes"));

    net->updControl("SoundFileSource/src/mrs_string/filename", "files/test.wav");

    net->tick();

    out = net->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();

    // Check to see if the generated data is correct.

    // This data was generated with Marsyas,
    // at commit [0f334a9ad5b74c1cf8b4e7ddffc2f94aba0e0daf].

    TS_ASSERT_DELTA(out(0,0),  0.266524195671082, 0.001);
    TS_ASSERT_DELTA(out(0,1),  0.220049291849136, 0.001);
    TS_ASSERT_DELTA(out(0,2),  0.195456027984619, 0.001);
    TS_ASSERT_DELTA(out(0,24), 0.0801039263606071, 0.001);
    TS_ASSERT_DELTA(out(0,25), 0.0875107944011688, 0.001);
    TS_ASSERT_DELTA(out(0,26), 0.0948617681860924, 0.001);
    TS_ASSERT_DELTA(out(0,45), 0.103429272770882, 0.001);
    TS_ASSERT_DELTA(out(0,46), 0.098756343126297, 0.001);
    TS_ASSERT_DELTA(out(0,47), 0.0939753353595734, 0.001);

    //
    // Tick the network 3 more times to see if the values still line up
    //
    net->tick();
    net->tick();
    net->tick();
    out = net->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();

    // Check to see if the generated data is correct.

    // This data was generated with Marsyas,
    // at commit [0f334a9ad5b74c1cf8b4e7ddffc2f94aba0e0daf].

    TS_ASSERT_DELTA(out(0,0),  0.392716139554977, 0.001);
    TS_ASSERT_DELTA(out(0,1),  0.352966338396072, 0.001);
    TS_ASSERT_DELTA(out(0,2),  0.330254524946213, 0.001);
    TS_ASSERT_DELTA(out(0,24), 0.170649513602257, 0.001);
    TS_ASSERT_DELTA(out(0,25), 0.175857156515121, 0.001);
    TS_ASSERT_DELTA(out(0,26), 0.180058494210243, 0.001);
    TS_ASSERT_DELTA(out(0,45), 0.173320427536964, 0.001);
    TS_ASSERT_DELTA(out(0,46), 0.173203453421593, 0.001);
    TS_ASSERT_DELTA(out(0,47), 0.173799067735672, 0.001);


    delete net;

  }


};
