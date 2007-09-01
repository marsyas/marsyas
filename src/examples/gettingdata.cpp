#include "MarSystemManager.h"
using namespace std;
using namespace Marsyas;

void recognize(string sfName)
{
	MarSystemManager mng;
	MarSystem* pnet = mng.create("Series", "pnet");
// standard network
	pnet->addMarSystem(mng.create("SoundFileSource", "src"));
	pnet->updctrl("SoundFileSource/src/mrs_string/filename", sfName);
	pnet->addMarSystem(mng.create("Spectrum","spk"));
	pnet->addMarSystem(mng.create("Gain", "g2"));

	while ( pnet->getctrl("SoundFileSource/src/mrs_bool/notEmpty")->to<mrs_bool>() )
	{
		pnet->tick();
		// gets data from the Spectrum
		const realvec& processedData =
pnet->getctrl("Spectrum/spk/mrs_realvec/processedData")->to<mrs_realvec>();
		// get data from the end of the MarSystem
		// TODO: clarify these two points.
		//const realvec& processedData = pnet->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();
		cout << processedData << endl;
	}
	delete pnet;
}

int main(int argc, const char **argv)
{
	string fileName;
	if (argc<2)
	{
		cout<<"Please enter filename."<<endl;
		exit(1);
	}
	else
	{
		fileName = argv[1];
	}
	cout << "Processing file " << fileName << endl;

	recognize(fileName);
	exit(0);
}


