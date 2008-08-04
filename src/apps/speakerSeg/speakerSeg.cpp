#include <cstdio>
#include "Collection.h"
#include "MarSystemManager.h"
#include "CommandLineOptions.h"

#include <vector> 

using namespace std;
using namespace Marsyas;

/* global variables for various commandline options */ 
int helpopt;
int usageopt;
int verboseopt;
string fileName;
string pluginName;
CommandLineOptions cmd_options;

void 
printUsage(string progName)
{
	MRSDIAG("speakerSeg.cpp - printUsage");
	cerr << "Usage : " << progName << " [-f outputfile] [-p pluginName] file1 file2 file3" << endl;
	cerr << endl;
	cerr << "where file1, ..., fileN are sound files in a MARSYAS supported format or collections " << endl;
	exit(1);
}

void 
printHelp(string progName)
{
	MRSDIAG("speakerSeg.cpp - printHelp");
	cerr << "speakerSeg, MARSYAS, Copyright George Tzanetakis " << endl;
	cerr << "--------------------------------------------" << endl;
	cerr << "Play the sound files provided as arguments " << endl;
	cerr << endl;
	cerr << "Usage : " << progName << " file1 file2 file3" << endl;
	cerr << endl;
	cerr << "where file1, ..., fileN are sound files in a Marsyas supported format" << endl;
	cerr << "Help Options:" << endl;
	cerr << "-u --usage      : display short usage info" << endl;
	cerr << "-h --help       : display this information " << endl;
	cerr << "-f --file       : output to file " << endl;
	cerr << "-p --plugin     : output plugin name " << endl;
	exit(1);
}

// Play a collection l of soundfiles
void speakerSeg(vector<string> soundfiles)
{
	MRSDIAG("speakerSeg.cpp - speakerSeg");

	MarSystemManager mng;
	string sfName;

	//create main processing network
	MarSystem* pnet = mng.create("Series", "pnet");

// 	// Output destination is either audio or soundfile 
// 	MarSystem* dest;
// 	if (fileName == EMPTYSTRING)	// audio output
// 		dest = mng.create("AudioSink", "dest");
// 	else 				// filename output
// 	{
// 		dest = mng.create("SoundFileSink", "dest");
// 	}

	//create feature extraction network for calculating LSP-10
	MarSystem* featExtractor = mng.create("Series", "featExtractor");
	featExtractor->addMarSystem(mng.create("SoundFileSource", "src"));
	featExtractor->addMarSystem(mng.create("LPCnet", "lpc"));
	featExtractor->addMarSystem(mng.create("LSP", "lsp"));
	featExtractor->updctrl("mrs_natural/inSamples", 125); //hardcoded for fs=8khz [!]
	featExtractor->updctrl("LPCnet/lpc/mrs_natural/order", 10);	//hardcoded [!]

	// based on the nr of features (in this case, the LSP order = 10),
	// calculate the minimum number of frames each speech segment should have
	// in order to avoid ill calculation of the corresponding covariance matrices.
	// To have a good estimation of the covariance matrices
	// the number of data points (i.e. feature vectors) should be at least
	// equal or bigger than d(d+1)/2, where d = cov matrix dimension.
	mrs_real d = (mrs_real)featExtractor->getctrl("LPCnet/lpc/mrs_natural/order")->to<mrs_natural>();
	mrs_natural minSegFrames = (mrs_natural)ceil(0.5*d*(d+1.0)); //0.5*d*(d+1.0) or 0.5*d*(d-1.0) [?]

	//speakerSeg processes data at each tick as depicted bellow, 
	//which includes 4 speech sub-segments (C1, C2, C3, C4) that will be used for detecting 
	//speaker changes:
	//
	// data processed at each tick
	// |------------------------|
	//      C1        C2
	// |----+----|----+----|					
	//      |----+----|----+----|
	//           C3        C4
	// |--->|
	//   hop
	//
	// So, the speech segments C1, C2, C3 and C4 should have a minimum of minSegFrames in order
	// to avoid cov ill-calculation. Consequently, at each tick, at least 5/2*minSegFrames should
	// be processed, with a hop set to 1/2*minSegFrames (which could be changed, but for now is fixed).

	//create an accumulator for creating hopsize new feature vectors at each tick
	MarSystem* accum = mng.create("Accumulator", "accum");
	accum->addMarSystem(featExtractor);
	accum->updctrl("mrs_natural/nTimes", minSegFrames/2);

	//add accumuated feature extraction to main processing network
	pnet->addMarSystem(accum);

	//create a circular buffer for storing most recent LSP10 speech data
	pnet->addMarSystem(mng.create("Memory", "mem"));
	pnet->updctrl("Memory/mem/mrs_natural/memSize", 5); //see above for an explanation why we use memSize = 5

	//add a BIC change detector
	pnet->addMarSystem(mng.create("BICchangeDetector", "BICdet"));
	
	
	// link top-level controls 
	pnet->linkctrl("mrs_string/filename","Accumulator/accum/Series/featExtractor/SoundFileSource/src/mrs_string/filename");
	pnet->linkctrl("mrs_natural/nChannels","Accumulator/accum/Series/featExtractor/SoundFileSource/src/mrs_natural/nChannels");
	pnet->linkctrl("mrs_real/israte", "Accumulator/accum/Series/featExtractor/SoundFileSource/src/mrs_real/israte");
	pnet->linkctrl("mrs_natural/pos", "Accumulator/accum/Series/featExtractor/SoundFileSource/src/mrs_natural/pos");
	pnet->linkctrl("mrs_bool/notEmpty", "Accumulator/accum/Series/featExtractor/SoundFileSource/src/mrs_bool/notEmpty");

	// play each collection or soundfile 
	vector<string>::iterator sfi;  
	for (sfi = soundfiles.begin(); sfi != soundfiles.end(); ++sfi) 
	{
		string fname = *sfi;
		//clear any memory data and any stored models
		pnet->updctrl("Memory/mem/mrs_bool/reset", true);
		pnet->updctrl("BICchangeDetector/BICdet/mrs_bool/reset", true);
		//set new file name
		pnet->updctrl("mrs_string/filename", fname);

// 		if (fileName != EMPTYSTRING) // soundfile output instead of audio output
// 			pnet->updctrl("SoundFileSink/dest/mrs_string/filename", fileName);
// 
// 		if (fileName == EMPTYSTRING)	// audio output
// 		{
// 			pnet->updctrl("AudioSink/dest/mrs_natural/bufferSize", 256); 
// 			pnet->updctrl("AudioSink/dest/mrs_bool/initAudio", true);
// 		}

		MarControlPtr notEmptyPtr_ = 
			pnet->getctrl("mrs_bool/notEmpty");

		while (notEmptyPtr_->isTrue())	
		{
			pnet->tick();
		}
	}

	// output network description to cout  
	if ((pluginName == EMPTYSTRING) && (verboseopt)) // output to stdout 
	{
		cout << (*pnet) << endl;      
	}
	else if (pluginName != EMPTYSTRING)             // output to plugin
	{
		ofstream oss(pluginName.c_str());
		oss << (*pnet) << endl;
	}
	delete pnet;
}


void 
initOptions()
{
	cmd_options.addBoolOption("help", "h", false);
	cmd_options.addBoolOption("usage", "u", false);
	cmd_options.addBoolOption("verbose", "v", false);
	cmd_options.addStringOption("filename", "f", EMPTYSTRING);
	cmd_options.addStringOption("plugin", "p", EMPTYSTRING);
}

void 
loadOptions()
{
	helpopt = cmd_options.getBoolOption("help");
	usageopt = cmd_options.getBoolOption("usage");
	verboseopt = cmd_options.getBoolOption("verbose");
	pluginName = cmd_options.getStringOption("plugin");
	fileName   = cmd_options.getStringOption("filename");
}

int
main(int argc, const char **argv)
{
	MRSDIAG("speakerSeg.cpp - main");

	string progName = argv[0];  
	if (argc == 1)
		printUsage(progName);

	// handling of command-line options 
	initOptions();
	cmd_options.readOptions(argc, argv);
	loadOptions();

	vector<string> soundfiles = cmd_options.getRemaining();
	if (helpopt) 
		printHelp(progName);

	if (usageopt)
		printUsage(progName);

	// play the soundfiles/collections 
	speakerSeg(soundfiles);

	exit(0);
}






