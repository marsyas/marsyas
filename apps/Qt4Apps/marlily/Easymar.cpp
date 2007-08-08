/**
	\class Easymar
	\ingroup Notmar
	\brief A collection of functions which simplify transcription (pitch
extraction, amplitude extraction, etc).

	Usage:
	- setPitchList() or getPitchesFromAudio()
	- calcOnsets()
	- calcNotes()
	- (optional)  calcRelativeDurations().  MUST BE AFTER THE FIRST TWO!
	- to see the results, use getOnsets() and getNotes(), and/or
	  getDurations()
*/

#include "Easymar.h"
static MarSystemManager mng;

Easymar::Easymar() {
}

Easymar::~Easymar() {
}

#define EMPTYSTRING "MARSYAS_EMPTY"

// TODO: ask -devel about making this a general Marsyas function
mrs_real
Easymar::addFileSource(MarSystem* net, string const infile)
{
    if (infile == EMPTYSTRING) {
		MRSERR("Please specify a sound file.");
		return 0;
    }
    net->addMarSystem(mng.create("SoundFileSource", "src"));
    net->updctrl("SoundFileSource/src/mrs_string/filename", infile);
    net->linkControl("mrs_bool/notEmpty",
                     "SoundFileSource/src/mrs_bool/notEmpty");
	return net->getctrl("SoundFileSource/src/mrs_real/osrate")->toReal();
}

void
Easymar::addDest(MarSystem* net, string outfile)
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



