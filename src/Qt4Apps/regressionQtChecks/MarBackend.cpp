/*
  Reads a file and writes it to a file.  Really boring and simple.
  the MarSystemQtWrapper should stop() going when it reaches the
  end of the input file.

  waitUntilDone() forces the main program to... err... wait until
  the processing is finished.
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

	mrsWrapper->start();
//	mrsWrapper->tickForever();
	mrsWrapper->pause();

	mrsWrapper->trackctrl( isEmptyPtr );

	mrsWrapper->play();
	ctrlChanged(isEmptyPtr);

	cout<<"mrsWrapper running"<<endl;
}

MarBackend::~MarBackend()
{
	delete pnet;
}

void MarBackend::ctrlChanged(MarControlPtr changed)
{
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
	cout<<"stopping"<<endl;
	mrsWrapper->pause();
/*
	if (mrsWrapper != NULL) {
		delete mrsWrapper;
		mrsWrapper = NULL;
		delete pnet;
	}
*/

	// signal to waitUntilFinished() that it can stop waiting
}


void MarBackend::waitUntilFinished()
{
	//wait = new QWaitCondition(); // maybe?
	//   or maybe a qmutex?  or qsemaphore?

	sleep(1);
}




