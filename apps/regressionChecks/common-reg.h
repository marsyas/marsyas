#ifndef COMMON_REG_H
#define COMMON_REG_H

#include <iostream>
#include "MarSystemManager.h"
#include "Collection.h"
#include "FileName.h"

// dunno what this is doing
#define EMPTYSTRING "MARSYAS_EMPTY"

#define CLOSE_ENOUGH 0.0001

using namespace std;
using namespace Marsyas;

MarSystemManager mng;
mrs_real srate;

// really useful global functions
void
addSource(MarSystem* net, string infile)
{
	if (infile == EMPTYSTRING) {
		cout << "Please specify a sound file." << endl;
		exit(1);
	}

	net->addMarSystem(mng.create("SoundFileSource", "src"));
	net->updctrl("SoundFileSource/src/mrs_string/filename", infile);
	srate = net->getctrl("SoundFileSource/src/mrs_real/osrate")->toReal();
	net->linkControl("mrs_bool/notEmpty",
	                 "SoundFileSource/src/mrs_bool/notEmpty");
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

#endif

