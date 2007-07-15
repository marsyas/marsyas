// Basic Audio Processing checks
#include "common-reg.h"


void
basic_vibrato(string infile, string outfile)
{
	if (infile == EMPTYSTRING) {
		cout << "Please specify a sound file." << endl;
		exit(1);
	}
	cout << "Testing vibrato" << endl;

	MarSystemManager mng;
	MarSystem* playbacknet = mng.create("Series", "playbacknet");

	playbacknet->addMarSystem(mng.create("SoundFileSource", "src"));
	playbacknet->updctrl("SoundFileSource/src/mrs_string/filename", infile);
	playbacknet->addMarSystem(mng.create("Vibrato", "vib"));
	//playbacknet->addMarSystem(mng.create("Vibrato2", "vib2"));
	if (outfile == EMPTYSTRING)
	{
		playbacknet->addMarSystem(mng.create("AudioSink", "dest"));
		playbacknet->updctrl("AudioSink/dest/mrs_bool/initAudio", true);
	}
	else
	{
		cout<<outfile<<endl;
		playbacknet->addMarSystem(mng.create("SoundFileSink", "dest"));
		playbacknet->updctrl("SoundFileSink/dest/mrs_string/filename", outfile);
	}

	playbacknet->linkControl("mrs_bool/notEmpty", "SoundFileSource/src/mrs_bool/notEmpty");
	playbacknet->linkControl("mrs_natural/pos", "SoundFileSource/src/mrs_natural/pos");

	playbacknet->updctrl("Vibrato/vib/mrs_real/mod_freq", 10.0);
	mrs_bool isEmpty;
	while (isEmpty = playbacknet->getctrl("mrs_bool/notEmpty")->toBool())
	{
		playbacknet->tick();
	}
	delete playbacknet;
}



