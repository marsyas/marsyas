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
//	pnet->addMarSystem(mng.create("SoundFileSink", "dest"));
	// pnet->addMarSystem(mng.create("AudioSink", "dest"));

//	pnet->updctrl("SoundFileSink/dest/mrs_string/filename",
//	              outfile);
	// pnet->updctrl("AudioSink/dest/mrs_bool/initAudio", true);


	// wrap it up to make it pretend to be a Qt object:
	mrsWrapper = new MarSystemQtWrapper(pnet);
	connect(mrsWrapper, SIGNAL(ctrlChanged(MarControlPtr)),
	        this, SLOT(ctrlChanged(MarControlPtr)));

	isEmptyPtr = mrsWrapper->getctrl("SoundFileSource/src/mrs_bool/notEmpty");
	posPtr = mrsWrapper->getctrl("SoundFileSource/src/mrs_natural/pos");

	mrsWrapper->trackctrl( isEmptyPtr );

	mrsWrapper->start();
//	mrsWrapper->tickForever();
//	mrsWrapper->pause();

	isRunning = true;
	mrsWrapper->play();

//	ctrlChanged(isEmptyPtr);

//	wait = new QWaitCondition();
	cout<<"mrsWrapper running"<<endl;
}

MarBackend::~MarBackend()
{
}

void MarBackend::ctrlChanged(MarControlPtr changed)
{
	mutex.lock();
	if ( changed.isEqual( isEmptyPtr ) )
	{
		if (changed->to<mrs_bool>() == false)
		{
			if (isRunning == true) {
				stop();
				cout<<"unlocked"<<endl;
			}
		}
	}
	mutex.unlock();
}

void MarBackend::stop()
{
	cout<<"stopping"<<endl;
	isRunning = false;
	mrsWrapper->pause();
	delete mrsWrapper;
	mrsWrapper = NULL;
	delete pnet;
	cout<<"stopped"<<endl;

	// signal to waitUntilFinished() that it can stop waiting
	wait.wakeAll();
}


void MarBackend::waitUntilFinished()
{
	QMutex mutex2;
	mutex2.lock();
	wait.wait(&mutex2);
	cout<<"signaled"<<endl;
	mutex2.unlock();
}




