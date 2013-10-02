#include <marsyas/CommandLineOptions.h>
#include <marsyas/system/MarSystemManager.h>
#include <marsyas/realtime/runner.h>
#include <cstdlib>
#include <iostream>
#include <algorithm>

using namespace Marsyas;
using namespace Marsyas::RealTime;
using namespace std;

int main( int argc, const char **argv )
{
  CommandLineOptions opt;
  opt.addBoolOption("real-time", "r", false);
  opt.addRealOption("sample-rate", "s", 44100.0);
  opt.addNaturalOption("block-size", "b", 128);
  opt.addRealOption("delay", "d", 0.0);

  opt.readOptions(argc, argv);

  mrs_real delay = std::max((mrs_real)0.0, opt.getRealOption("delay"));
  mrs_real sr = opt.getRealOption("sample-rate");
  mrs_natural block = std::max((mrs_natural)1, opt.getNaturalOption("block-size"));
  bool realtime = opt.getBoolOption("real-time");

  cout << "realtime mode = " << (realtime ? "on" : "off") << endl;
  cout << "sample rate = " << sr << " Hz" << endl;
  cout << "block size = " << block << " frames" << endl;
  cout << "delay = " << delay << " seconds" << endl;

  MarSystemManager mng;

  MarSystem * system = mng.create("Series", "system");
  system->addMarSystem( mng.create("AudioSource", "in") );
  system->addMarSystem( mng.create("Delay", "delay") );
  system->addMarSystem( mng.create("AudioSink", "out") );

  system->setControl("mrs_real/israte", sr);
  system->setControl("mrs_natural/inSamples", block);
  system->setControl("Delay/delay/mrs_real/maxDelaySeconds", delay);
  system->setControl("Delay/delay/mrs_real/delaySeconds", delay);
  system->setControl("AudioSource/in/mrs_bool/realtime", realtime);
  system->setControl("AudioSink/out/mrs_bool/realtime", realtime);

  system->setControl("AudioSource/in/mrs_bool/initAudio", true);
  system->setControl("AudioSink/out/mrs_bool/initAudio", true);

  system->update();

  Runner system_runner(system);
  system_runner.start();
  system_runner.wait();

  return 0;
}
