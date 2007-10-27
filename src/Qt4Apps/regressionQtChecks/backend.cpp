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
	cout<<"going"<<endl;
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
			cout<<"stop!"<<endl;
			mrsWrapper->pause();
			delete mrsWrapper;
			delete pnet;
		}
	}
}


