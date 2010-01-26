#include "MarSystemManager.h"
using namespace std;
using namespace Marsyas;

void sfplay(string sfName, float gain)
{
	MarSystemManager mng;

	MarSystem* playbacknet = mng.create("Series", "playbacknet");

	playbacknet->addMarSystem(mng.create("SoundFileSource", "src"));
	playbacknet->addMarSystem(mng.create("Gain", "gt"));
	playbacknet->addMarSystem(mng.create("AudioSink", "dest"));

	playbacknet->updctrl("SoundFileSource/src/mrs_string/filename", sfName);
	playbacknet->updctrl("Gain/gt/mrs_real/gain", gain);
	playbacknet->updctrl("AudioSink/dest/mrs_bool/initAudio", true);

	while ( playbacknet->getctrl("SoundFileSource/src/mrs_bool/hasData")->to<mrs_bool>() )
	{
		playbacknet->tick();
	}
	delete playbacknet;
}

int main(int argc, const char **argv)
{
	string fileName;
	float gain;
	if (argc<2)
	{
		cout<<"Please enter filename."<<endl;
		exit(1);
	}
	else
	{
		fileName = argv[1];
	}
	if (argc<3)
	{
		gain = 1;
	}
	else
	{
		gain = atof(argv[2]);
	}
	cout << "Playing file " << fileName << " at volume " <<
	gain << endl;

	sfplay(fileName,gain);
	exit(0);
}

