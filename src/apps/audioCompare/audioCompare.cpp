#include "MarSystemManager.h"
using namespace Marsyas;
using namespace std;

#define CLOSE_ENOUGH 0.0001

void
printHelp(string progName)
{
	MRSDIAG("audioCompare.cpp - printUsage");
	cerr << "audioCompare, MARSYAS" << endl;
	cerr << "-------------------------" << endl;
	cerr << "Usage: " <<endl;
	cerr << progName<<" file1 file2"<<endl;
	exit(1);
}

void
isClose(string infile1, string infile2)
{
	MarSystemManager mng;
	MarSystem* pnet = mng.create("Series", "pnet");

	MarSystem* invnet = mng.create("Series", "invnet");
	invnet->addMarSystem(mng.create("SoundFileSource", "src2"));
	invnet->updctrl("SoundFileSource/src2/mrs_string/filename", infile2);
	invnet->addMarSystem(mng.create("Negative", "neg"));

	MarSystem* fanout = mng.create("Fanout", "fanout");
	fanout->addMarSystem(mng.create("SoundFileSource", "src1"));
	fanout->updctrl("SoundFileSource/src1/mrs_string/filename", infile1);
	fanout->addMarSystem(invnet);

	pnet->addMarSystem(fanout);
	pnet->addMarSystem(mng.create("Sum", "sum"));
	pnet->linkControl("mrs_bool/notEmpty",
	                  "Fanout/fanout/SoundFileSource/src1/mrs_bool/notEmpty");

	mrs_natural i;
	mrs_natural samples =
	    pnet->getctrl("mrs_natural/inSamples")->to<mrs_natural>();
	while ( pnet->getctrl("mrs_bool/notEmpty")->to<mrs_bool>() )
	{
		pnet->tick();
		const realvec& processedData =
		    pnet->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();
		for (i=0; i<samples; i++)
		{
			//  useful for tweaking CLOSE_ENOUGH
			//cout<<processedData(i)<<" ";
			if ( processedData(i) > CLOSE_ENOUGH )
			{
				exit(1);
			}
		}
	}
}


int
main(int argc, const char **argv)
{
	string progName = argv[0];
	if (argc != 3)
	{
		printHelp(progName);
	}

	string file1 = argv[1];
	string file2 = argv[2];

	isClose(file1, file2);
	exit(0);
}

