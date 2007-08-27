// Analysis checks
#include "common-reg.h"

#include "Transcriber.h"

void
analysis_pitch(string infile)
{
	MarSystem* pnet = mng.create("Series", "pnet");
	// sets up SoundFileSource, links notEmpty, and sets srate
	mrs_real srate = addSource( pnet, infile );
	pnet->addMarSystem(Transcriber::makePitchNet(srate, 100.0));
	while (pnet->getctrl("mrs_bool/notEmpty")->to<mrs_bool>())
	{
		pnet->tick();
		const realvec& processedData =
		    pnet->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();
		cout<<samples2hertz( processedData(1), srate)<<" ";
	}
	cout<<endl;
	delete pnet;
}



