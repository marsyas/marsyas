// TestAimSAI.h
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
#include <marsyas/marsystems/AimSAI.h>
#include <marsyas/CommandLineOptions.h>

#include <vector>

using namespace std;
using namespace Marsyas;

class AimSAI_runner : public CxxTest::TestSuite
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
    net->addMarSystem(mng.create("AimPZFC", "aimpzfc"));
    net->addMarSystem(mng.create("AimHCL", "aimhcl"));
    net->addMarSystem(mng.create("AimLocalMax", "aimlocalmax"));
    net->addMarSystem(mng.create("AimSAI", "aimsai"));

    net->updControl("SoundFileSource/src/mrs_string/filename", "files/test.wav");

    net->tick();

    out = net->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();

    // Check to see if the generated data is correct.

    // This data was generated with Marsyas,
    // at commit [0f334a9ad5b74c1cf8b4e7ddffc2f94aba0e0daf].

    TS_ASSERT_DELTA(out(0,0),   0.000646012726689387, 0.001);
    TS_ASSERT_DELTA(out(0,1),   0.000644890636603683, 0.001);
    TS_ASSERT_DELTA(out(0,2),   0.000641813084076837, 0.001);
    TS_ASSERT_DELTA(out(0,254), 0.0, 0.001);
    TS_ASSERT_DELTA(out(0,255), 0.0, 0.001);
    TS_ASSERT_DELTA(out(0,256), 0.0, 0.001);
    TS_ASSERT_DELTA(out(0,509), 0.0, 0.001);
    TS_ASSERT_DELTA(out(0,510), 0.0, 0.001);
    TS_ASSERT_DELTA(out(0,511), 0.0, 0.001);

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

    TS_ASSERT_DELTA(out(0,0),   0.0611952130309612, 0.001);
    TS_ASSERT_DELTA(out(0,1),   0.0612242248033321, 0.001);
    TS_ASSERT_DELTA(out(0,2),   0.0612381911658602, 0.001);
    TS_ASSERT_DELTA(out(0,254), 1.60261859212331e-13, 0.001);
    TS_ASSERT_DELTA(out(0,255), 1.35923667274146e-13, 0.001);
    TS_ASSERT_DELTA(out(0,256), 1.15277833266879e-13, 0.001);
    TS_ASSERT_DELTA(out(0,509), 0.346267347088163, 0.001);
    TS_ASSERT_DELTA(out(0,510), 0.344182102483322, 0.001);
    TS_ASSERT_DELTA(out(0,511), 0.342014540153856, 0.001);


    delete net;

  }



};
