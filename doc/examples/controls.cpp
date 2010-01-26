#include "MarSystemManager.h"
using namespace std;
using namespace Marsyas;

void sfplay(string sfName, float gain, float start)
{
	MarSystemManager mng;

	MarSystem* playbacknet = mng.create("Series", "playbacknet");

	playbacknet->addMarSystem(mng.create("SoundFileSource", "src"));
	playbacknet->addMarSystem(mng.create("Gain", "gt"));
	playbacknet->addMarSystem(mng.create("AudioSink", "dest"));

// calculate the starting position.
	mrs_natural nChannels = playbacknet->getctrl("SoundFileSource/src/mrs_natural/nChannels")->to<mrs_natural>();
	mrs_real srate = playbacknet->getctrl("SoundFileSource/src/mrs_real/israte")->to<mrs_real>();
	mrs_natural startPosition = (mrs_natural) (start * srate * nChannels);

	playbacknet->updctrl("SoundFileSource/src/mrs_string/filename", sfName);
	playbacknet->updctrl("Gain/gt/mrs_real/gain", gain);
	playbacknet->updctrl("AudioSink/dest/mrs_bool/initAudio", true);

// set the starting position of the source
	playbacknet->updctrl("SoundFileSource/src/mrs_natural/pos", startPosition);

	while ( playbacknet->getctrl("SoundFileSource/src/mrs_bool/hasData")->to<mrs_bool>() )
	{
		playbacknet->tick();
	}
	delete playbacknet;
}

int main(int argc, const char **argv)
{
	string fileName;
	float gain, start;
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
	if (argc<4)
	{
		start = 0;
	}
	else
	{
		start = atof(argv[3]);
	}
	cout << "Playing file " << fileName << " at volume " <<
	gain << " starting at " << start << " seconds" << endl;

	sfplay(fileName,gain,start);
	exit(0);
}
