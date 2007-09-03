#include "common-reg.h"

// really useful global functions
mrs_real 
addSource(MarSystem* net, string infile)
{
	if (infile == EMPTYSTRING) {
		cout << "Please specify a sound file." << endl;
		exit(1);
	}

	net->addMarSystem(mng.create("SoundFileSource", "src"));
	net->updctrl("SoundFileSource/src/mrs_string/filename", infile);
	net->linkControl("mrs_bool/notEmpty",
	                 "SoundFileSource/src/mrs_bool/notEmpty");
	return net->getctrl("SoundFileSource/src/mrs_real/osrate")->to<mrs_real>();
}

void
addDest(MarSystem* net, string outfile)
{
    if (outfile == EMPTYSTRING)
    {
        net->addMarSystem(mng.create("AudioSink", "dest"));
        net->updctrl("AudioSink/dest/mrs_bool/initAudio", true);
    }
    else
    {
        net->addMarSystem(mng.create("SoundFileSink", "dest"));
        net->updctrl("SoundFileSink/dest/mrs_string/filename", outfile);
    }
}

