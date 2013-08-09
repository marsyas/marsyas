/**
	\class Easymar
	\ingroup Notmar
	\brief A collection of functions which greatly simplify working with
Marsyas (setting up a file source, destination, etc).

	Usage:
	mrs_real srate = addFileSource(myNet, infilename);
*/

#include "Easymar.h"
static MarSystemManager mng;

Easymar::Easymar()
{
}

Easymar::~Easymar()
{
}

// TODO: ask -devel about making this a general Marsyas function
mrs_real
Easymar::addFileSource(MarSystem* net, const string infile)
{
  if (infile == EMPTYSTRING)
  {
    MRSERR("Please specify a sound file.");
    return 0;
  }
  net->addMarSystem(mng.create("SoundFileSource", "src"));
  net->updctrl("SoundFileSource/src/mrs_string/filename", infile);
  net->linkControl("mrs_bool/hasData",
                   "SoundFileSource/src/mrs_bool/hasData");
  return net->getctrl("SoundFileSource/src/mrs_real/osrate")->to<mrs_real>();
}

void
Easymar::addDest(MarSystem* net, const string outfile)
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



