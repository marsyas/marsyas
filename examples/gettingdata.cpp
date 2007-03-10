#include "MarSystemManager.h"
using namespace std;
using namespace Marsyas;

void recognize(string sfName) {
	MarSystemManager mng;
	MarSystem* pnet = mng.create("Series", "pnet");
// standard network
	pnet->addMarSystem(mng.create("SoundFileSource", "src"));
	pnet->updctrl("SoundFileSource/src/mrs_string/filename", sfName);
	pnet->addMarSystem(mng.create("Spectrum","spk"));

// variables to get data from network
	realvec in(pnet->getctrl("mrs_natural/inObservations")->toNatural(), pnet->getctrl("mrs_natural/inSamples")->toNatural());
	realvec out(pnet->getctrl("mrs_natural/onObservations")->toNatural(), pnet->getctrl("mrs_natural/onSamples")->toNatural());

// counter variables
	int i,j;
	mrs_natural numberObservations = pnet->getctrl("mrs_natural/onObservations")->toNatural();
	mrs_natural numberSamples = pnet->getctrl("mrs_natural/onSamples")->toNatural();   // in this case, numbSamples will be 1, due to the Spectrum MarSystem.

	while ( pnet->getctrl("SoundFileSource/src/mrs_bool/notEmpty")->toBool() ) {
// don't tick because process() does that.
//		pnet->tick();
// get data from network
		pnet->process(in,out);

// display data
		for (i=0; i<numberObservations; i++) {
			for (j=0; j<numberSamples; j++) {
				cout<<out(i,j)<<" ";
			}
		}
		cout<<endl;

		const realvec& processedData = pnet->getctrl("Spectrum/spk/mrs_realvec/processedData")->to<mrs_realvec>();
		cout << processedData << endl;

		
	}
	delete pnet;
}

int main(int argc, const char **argv) {
	string fileName;
	if (argc<2) { cout<<"Please enter filename."<<endl; exit(1); } else 
		{ fileName = argv[1]; }
	cout << "Processing file " << fileName << endl;

	recognize(fileName);
	exit(0);
}


