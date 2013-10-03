#include <cxxtest/TestSuite.h>

#include <cstdio>
#include <marsyas/Collection.h>
#include <marsyas/system/MarSystemManager.h>
#include <marsyas/CommandLineOptions.h>

#include <marsyas/marsystems/HarmonicStrength.h>

using namespace std;
using namespace Marsyas;

class HarmonicStrength_runner: public CxxTest::TestSuite
{
public:
  MarSystemManager mng;

  // "constructor"
  void setUp()
  {
  }

  // "destructor"
  void tearDown()
  {
  }

  /**
   * Example test of the HarmonicStrength process.
   */
  void test_process()
  {
    MarSystem* net = mng.create("Series/net");
    net->addMarSystem(mng.create("SoundFileSource/src"));
    net->addMarSystem(mng.create("Windowing", "win"));
    net->addMarSystem(mng.create("Spectrum", "spec"));
    net->addMarSystem(mng.create("PowerSpectrum", "pspec"));
    MarSystem* harmonicStrength = mng.create("HarmonicStrength/harm");
    net->addMarSystem(harmonicStrength);

    net->updControl("SoundFileSource/src/mrs_string/filename", "files/harmonic-mix.wav");

    harmonicStrength->updControl("mrs_natural/harmonicsSize", 5);
    harmonicStrength->updControl("mrs_real/harmonicsWidth", 0.01);
    harmonicStrength->updControl("mrs_natural/type", 0);
    harmonicStrength->updControl("mrs_real/base_frequency", 1000.0);

    net->update();

    net->tick(); // get rid of initial fuzziness in data
    net->tick();

    mrs_realvec out = net->getControl("mrs_realvec/processedData")->to<mrs_realvec>();

    TS_ASSERT_DELTA(out(0,0),0.335,0.001);
    TS_ASSERT_DELTA(out(1,0),0.857,0.001);
    TS_ASSERT_DELTA(out(2,0),0.099,0.001);
    TS_ASSERT_DELTA(out(3,0),0.020,0.001);
    TS_ASSERT_DELTA(out(4,0),0.00,0.001);

    delete net;
  }
};

