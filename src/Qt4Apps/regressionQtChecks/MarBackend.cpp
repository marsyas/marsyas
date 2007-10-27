/*
  Reads a file and writes it to a file.  Really boring and simple.
  the MarSystemQtWrapper should stop() going when it reaches the
  end of the input file.

  Currently does not detect any SIGNAL(ctrlChanged(MarControlPtr))
  from mrsWrapper.  :(
*/

#include "MarBackend.h"

MarBackend::MarBackend(string infile, string outfile)
{
  pnet = mng.create("Series", "pnet");
  // input
  pnet->addMarSystem(mng.create("SoundFileSource", "src"));
  pnet->updctrl("SoundFileSource/src/mrs_string/filename",
		infile);
  pnet->linkControl("mrs_bool/notEmpty",
		    "SoundFileSource/src/mrs_bool/notEmpty");
  // output
  pnet->addMarSystem(mng.create("SoundFileSink", "dest"));
  // pnet->addMarSystem(mng.create("AudioSink", "dest"));
  
  pnet->updctrl("SoundFileSink/dest/mrs_string/filename",
		outfile);
  // pnet->updctrl("AudioSink/dest/mrs_bool/initAudio", true);
  
  // wrap it up to make it pretend to be a Qt object:
  mrsWrapper = new MarSystemQtWrapper(pnet);

  connect(mrsWrapper, SIGNAL(ctrlChanged(MarControlPtr)),
	  this, SLOT(ctrlChanged(MarControlPtr)));
  
  isEmptyPtr = mrsWrapper->getctrl("SoundFileSource/src/mrs_bool/notEmpty");
  posPtr = mrsWrapper->getctrl("SoundFileSource/src/mrs_natural/pos");
  




  mrsWrapper->tickForever();
  mrsWrapper->pause();
  
  mrsWrapper->trackctrl( isEmptyPtr );
  mrsWrapper->play();
  cout<<"mrsWrapper running"<<endl;
  ctrlChanged(isEmptyPtr);
}

MarBackend::~MarBackend()
{

}

void MarBackend::ctrlChanged(MarControlPtr changed)
{
  cout<<"ctrl changed"<<endl;
  if ( changed.isEqual( isEmptyPtr ) )
    {
      if (changed->to<mrs_bool>() == false)
	{
	  stop();
	  return;
	}
    }
}

void MarBackend::stop()
{
  cout<<"stop!"<<endl;
  delete mrsWrapper;
  // delete pnet;
}

