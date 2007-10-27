/*
  Reads a file and writes it to a file.  Really boring and simple.
  the MarSystemQtWrapper should stop() going when it reaches the
  end of the input file.

  Currently does not detect any SIGNAL(ctrlChanged(MarControlPtr))
  from mrsWrapper.  :(
*/

#include "backend.h"

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
	pnet->updctrl("SoundFileSink/dest/mrs_string/filename",
	              outfile);

	// wrap it up to make it pretend to be a Qt object:
	mrsWrapper = new MarSystemQtWrapper(pnet);

	isEmptyPtr = mrsWrapper->getctrl("mrs_bool/notEmpty");
	mrsWrapper->trackctrl( isEmptyPtr );
	connect(mrsWrapper, SIGNAL(ctrlChanged(MarControlPtr)),
	        this, SLOT(ctrlChanged(MarControlPtr)));

	mrsWrapper->start();
	mrsWrapper->play();
	cout<<"mrsWrapper running"<<endl;
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
		}
	}
}

void MarBackend::stop()
{
	cout<<"stop!"<<endl;
	mrsWrapper->pause();
	delete mrsWrapper;
	delete pnet;
}

