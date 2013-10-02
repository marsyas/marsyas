#include <marsyas/system/MarSystemManager.h>
#include <iostream>
using namespace Marsyas;
using namespace std;

int main(int argc, char *argv[])
{
  std::string infile = argv[1];

  MarSystemManager mng;
  MarSystem* pnet = mng.create("Series", "pnet");
  pnet->addMarSystem(mng.create("SoundFileSource", "src"));
  pnet->updctrl("SoundFileSource/src/mrs_string/filename", infile);
  pnet->linkControl("mrs_bool/hasData",
                    "SoundFileSource/src/mrs_bool/hasData");

  pnet->updctrl("mrs_natural/inSamples",256);

  pnet->addMarSystem(mng.create("ShiftInput", "shift"));
  pnet->updctrl("ShiftInput/shift/mrs_natural/WindowSize",512);

  int i=0;
  while ( pnet->getctrl("mrs_bool/hasData")->to<bool>() )
  {
    pnet->tick();
    i++;
  }
  cout<<i<<endl;
  delete pnet;
}

