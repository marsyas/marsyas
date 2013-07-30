#include <marsyas/core/MarSystemManager.h>
#include <marsyas/realtime/runner.h>
#include <cstdlib>
#include <iostream>

using namespace Marsyas;
using namespace Marsyas::RealTime;
using namespace std;

int main( int argc, char *argv[] )
{
  mrs_real delay_sec = 0.0;

  if (argc > 1)
  {
    delay_sec = std::atof(argv[1]);
    cout << "Using delay = " << delay_sec << " seconds" << endl;
  }
  else
  {
    cout << "Using default delay = " << delay_sec << " seconds" << endl;
  }

  MarSystemManager mng;

  MarSystem * system = mng.create("Series", "system");
  system->addMarSystem( mng.create("AudioSource", "in") );
  system->addMarSystem( mng.create("Delay", "delay") );
  system->addMarSystem( mng.create("AudioSink", "out") );

  system->setControl("mrs_real/israte", 44100.0);
  system->setControl("mrs_natural/inSamples", 128);
  system->setControl("Delay/delay/mrs_real/maxDelaySeconds", delay_sec);
  system->setControl("Delay/delay/mrs_real/delaySeconds", delay_sec);
  system->setControl("AudioSource/in/mrs_bool/realtime", true);
  system->setControl("AudioSink/out/mrs_bool/realtime", true);

  system->setControl("AudioSource/in/mrs_bool/initAudio", true);
  system->setControl("AudioSink/out/mrs_bool/initAudio", true);

  system->update();

  Runner system_runner(system);
  system_runner.start();
  system_runner.wait();

  return 0;
}
