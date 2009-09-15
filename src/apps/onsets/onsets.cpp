// mudbox is a container executable for various simple 
// functions/applications that typically test drive 
// a single MarSystem or type of processing. It can 
// either be viewed as repository of simple (but sometimes 
// broken) examples or as an incubator for more complicated 
// applications that deserve a separate executable. This 
// is the best place to experiment with Marsyas without 
// adding your own application and having to change 
// the build process.  

// #include <linux/soundcard.h>
// #include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
// #include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <string>
#include <iomanip> 
// #include <unistd.h>

#include "common.h"
#include "MarSystemManager.h"
#include "CommandLineOptions.h"
#include "FileName.h"

#include "MarSystemTemplateBasic.h"
#include "MarSystemTemplateAdvanced.h"
#include "EvValUpd.h"
#include "Collection.h"
#include "NumericLib.h"

#include "Spectrum2ACMChroma.h"
#include "time.h"

#ifdef MARSYAS_MIDIIO
#include "RtMidi.h"
#endif 

#ifdef MARSYAS_AUDIOIO
#include "RtAudio3.h"
#endif 

#ifdef MARSYAS_PNG
#include "pngwriter.h" 
#endif 


using namespace std;
using namespace Marsyas;

CommandLineOptions cmd_options;
string toy_withName;
int helpopt;
int usageopt;
int verboseopt;

void 
printUsage(string progName)
{
	MRSDIAG("onsets.cpp - printUsage");
	cerr << "Usage : " << progName << " fileName" << endl;
	cerr << endl;
	exit(1);
}

void 
printHelp(string progName)
{
	MRSDIAG("onsets.cpp - printHelp");
	cerr << "onsets, MARSYAS, Copyright George Tzanetakis " << endl;
	cerr << "--------------------------------------------" << endl;
	cerr << endl;
	cerr << "Usage : " << progName << "fileName" << endl;
	cerr << endl;
	
	exit(1);
}

void 
initOptions()
{
	cmd_options.addBoolOption("help", "h", false);
	cmd_options.addBoolOption("usage", "u", false);
	cmd_options.addBoolOption("verbose", "v", false);
}

void 
loadOptions()
{
	helpopt = cmd_options.getBoolOption("help");
	usageopt = cmd_options.getBoolOption("usage");
	verboseopt = cmd_options.getBoolOption("verbose");
}

void 
detect_onsets(string sfName) 
{
  // cout << "toying with onsets" << endl;
	MarSystemManager mng;

	// assemble the processing network 
	MarSystem* onsetnet = mng.create("Series", "onsetnet");
		MarSystem* onsetaccum = mng.create("Accumulator", "onsetaccum");
			MarSystem* onsetseries= mng.create("Series","onsetseries");
				onsetseries->addMarSystem(mng.create("SoundFileSource", "src"));
				onsetseries->addMarSystem(mng.create("Stereo2Mono", "src")); //replace by a "Monofier" MarSystem (to be created) [!]
				//onsetseries->addMarSystem(mng.create("ShiftInput", "si"));
				//onsetseries->addMarSystem(mng.create("Windowing", "win"));
				MarSystem* onsetdetector = mng.create("FlowThru", "onsetdetector");
					onsetdetector->addMarSystem(mng.create("ShiftInput", "si")); //<---
					onsetdetector->addMarSystem(mng.create("Windowing", "win")); //<---
					onsetdetector->addMarSystem(mng.create("Spectrum","spk"));
					onsetdetector->addMarSystem(mng.create("PowerSpectrum", "pspk"));
					onsetdetector->addMarSystem(mng.create("Flux", "flux")); 
					//onsetdetector->addMarSystem(mng.create("Memory","mem"));
					onsetdetector->addMarSystem(mng.create("ShiftInput","sif"));
					onsetdetector->addMarSystem(mng.create("Filter","filt1"));
					onsetdetector->addMarSystem(mng.create("Reverse","rev1"));
					onsetdetector->addMarSystem(mng.create("Filter","filt2"));
					onsetdetector->addMarSystem(mng.create("Reverse","rev2"));
					onsetdetector->addMarSystem(mng.create("PeakerOnset","peaker")); 
				onsetseries->addMarSystem(onsetdetector);
			onsetaccum->addMarSystem(onsetseries);
		onsetnet->addMarSystem(onsetaccum);
	//onsetnet->addMarSystem(mng.create("ShiftOutput","so"));
	MarSystem* onsetmix = mng.create("Fanout","onsetmix");
		onsetmix->addMarSystem(mng.create("Gain","gainaudio"));
			MarSystem* onsetsynth = mng.create("Series","onsetsynth");
				onsetsynth->addMarSystem(mng.create("NoiseSource","noisesrc"));
				onsetsynth->addMarSystem(mng.create("ADSR","env"));
				onsetsynth->addMarSystem(mng.create("Gain", "gainonsets"));
			onsetmix->addMarSystem(onsetsynth);
	onsetnet->addMarSystem(onsetmix);
	
	//onsetnet->addMarSystem(mng.create("AudioSink", "dest"));
	onsetnet->addMarSystem(mng.create("SoundFileSink", "fdest"));


	///////////////////////////////////////////////////////////////////////////////////////
	//link controls
	///////////////////////////////////////////////////////////////////////////////////////
	onsetnet->linkctrl("mrs_bool/notEmpty", 
		"Accumulator/onsetaccum/Series/onsetseries/SoundFileSource/src/mrs_bool/notEmpty");
	//onsetnet->linkctrl("ShiftOutput/so/mrs_natural/Interpolation","mrs_natural/inSamples");
	onsetnet->linkctrl("Accumulator/onsetaccum/mrs_bool/flush",
		"Accumulator/onsetaccum/Series/onsetseries/FlowThru/onsetdetector/PeakerOnset/peaker/mrs_bool/onsetDetected"); 
	//onsetnet->linkctrl("Fanout/onsetmix/Series/onsetsynth/Gain/gainonsets/mrs_real/gain",
	//	"Accumulator/onsetaccum/Series/onsetseries/FlowThru/onsetdetector/PeakerOnset/peaker/mrs_real/confidence");
	
	//onsetnet->linkctrl("Accumulator/onsetaccum/Series/onsetseries/FlowThru/onsetdetector/Memory/mem/mrs_bool/reset",
	//	"Accumulator/onsetaccum/Series/onsetseries/FlowThru/onsetdetector/PeakerOnset/peaker/mrs_bool/onsetDetected");

	//link FILTERS coeffs
	onsetnet->linkctrl("Accumulator/onsetaccum/Series/onsetseries/FlowThru/onsetdetector/Filter/filt2/mrs_realvec/ncoeffs",
		"Accumulator/onsetaccum/Series/onsetseries/FlowThru/onsetdetector/Filter/filt1/mrs_realvec/ncoeffs");
	onsetnet->linkctrl("Accumulator/onsetaccum/Series/onsetseries/FlowThru/onsetdetector/Filter/filt2/mrs_realvec/dcoeffs",
		"Accumulator/onsetaccum/Series/onsetseries/FlowThru/onsetdetector/Filter/filt1/mrs_realvec/dcoeffs");

	///////////////////////////////////////////////////////////////////////////////////////
	// update controls
	///////////////////////////////////////////////////////////////////////////////////////
	FileName outputFile(sfName);
	onsetnet->updctrl("Accumulator/onsetaccum/Series/onsetseries/SoundFileSource/src/mrs_string/filename", sfName);
	onsetnet->updctrl("SoundFileSink/fdest/mrs_string/filename", outputFile.nameNoExt() + "_onsets.wav");
	mrs_real fs = onsetnet->getctrl("mrs_real/osrate")->to<mrs_real>();

	mrs_natural winSize = 2048;//2048;
	mrs_natural hopSize = 512;//411;
	mrs_natural lookAheadSamples = 6;
	mrs_real thres = 1.75;

	mrs_real textureWinMinLen = 0.050; //secs
	mrs_natural minTimes = (mrs_natural) (textureWinMinLen*fs/hopSize); //12;//onsetWinSize+1;//15;
	// cout << "MinTimes = " << minTimes << " (i.e. " << textureWinMinLen << " secs)" << endl;
	mrs_real textureWinMaxLen = 3.000; //secs
	mrs_natural maxTimes = (mrs_natural) (textureWinMaxLen*fs/hopSize);//1000; //whatever... just a big number for now...
	// cout << "MaxTimes = " << maxTimes << " (i.e. " << textureWinMaxLen << " secs)" << endl;

	//best result till now are using dB power Spectrum!
	onsetnet->updctrl("Accumulator/onsetaccum/Series/onsetseries/FlowThru/onsetdetector/PowerSpectrum/pspk/mrs_string/spectrumType",
		"wrongdBonsets");

	onsetnet->updctrl("Accumulator/onsetaccum/Series/onsetseries/FlowThru/onsetdetector/Flux/flux/mrs_string/mode",
		"DixonDAFX06");
	
	//configure zero-phase Butterworth filter of Flux time series (from J.P.Bello TASLP paper)
	// Coefficients taken from MATLAB butter(2, 0.28)
	realvec bcoeffs(1,3);
	bcoeffs(0) = 0.1174;
	bcoeffs(1) = 0.2347;
	bcoeffs(2) = 0.1174;
	onsetnet->updctrl("Accumulator/onsetaccum/Series/onsetseries/FlowThru/onsetdetector/Filter/filt1/mrs_realvec/ncoeffs",
		bcoeffs);
	realvec acoeffs(1,3);
	acoeffs(0) = 1.0;
	acoeffs(1) = -0.8252;
	acoeffs(2) = 0.2946;
	onsetnet->updctrl("Accumulator/onsetaccum/Series/onsetseries/FlowThru/onsetdetector/Filter/filt1/mrs_realvec/dcoeffs",
		acoeffs);

	onsetnet->updctrl("mrs_natural/inSamples", hopSize);
	onsetnet->updctrl("Accumulator/onsetaccum/Series/onsetseries/FlowThru/onsetdetector/ShiftInput/si/mrs_natural/winSize", winSize);

	onsetnet->updctrl("Accumulator/onsetaccum/Series/onsetseries/FlowThru/onsetdetector/PeakerOnset/peaker/mrs_natural/lookAheadSamples", lookAheadSamples);
	onsetnet->updctrl("Accumulator/onsetaccum/Series/onsetseries/FlowThru/onsetdetector/PeakerOnset/peaker/mrs_real/threshold", thres); //!!!
	
	onsetnet->updctrl("Accumulator/onsetaccum/Series/onsetseries/FlowThru/onsetdetector/ShiftInput/sif/mrs_natural/winSize", 4*lookAheadSamples+1);
	
	mrs_natural winds = 1+lookAheadSamples+mrs_natural(ceil(mrs_real(winSize)/hopSize/2.0));
	// cout << "timesToKeep = " << winds << endl;
	onsetnet->updctrl("Accumulator/onsetaccum/mrs_natural/timesToKeep", winds);
	onsetnet->updctrl("Accumulator/onsetaccum/mrs_string/mode","explicitFlush");
	onsetnet->updctrl("Accumulator/onsetaccum/mrs_natural/maxTimes", maxTimes); 
	onsetnet->updctrl("Accumulator/onsetaccum/mrs_natural/minTimes", minTimes);

	//set audio/onset resynth balance and ADSR params for onset sound
	onsetnet->updctrl("Fanout/onsetmix/Gain/gainaudio/mrs_real/gain", 1.0);
	onsetnet->updctrl("Fanout/onsetmix/Series/onsetsynth/Gain/gainonsets/mrs_real/gain", 0.8);
	onsetnet->updctrl("Fanout/onsetmix/Series/onsetsynth/ADSR/env/mrs_real/aTarget", 1.0);
 	onsetnet->updctrl("Fanout/onsetmix/Series/onsetsynth/ADSR/env/mrs_real/aTime", winSize/80/fs); //!!!
 	onsetnet->updctrl("Fanout/onsetmix/Series/onsetsynth/ADSR/env/mrs_real/susLevel", 0.0);
 	onsetnet->updctrl("Fanout/onsetmix/Series/onsetsynth/ADSR/env/mrs_real/dTime", winSize/4/fs); //!!!
	
	//onsetnet->updctrl("AudioSink/dest/mrs_bool/initAudio", true);
	
	//MATLAB Engine inits
	//used for toy_with_onsets.m
	MATLAB_EVAL("clear;");
	MATLAB_PUT(winSize, "winSize");
	MATLAB_PUT(hopSize, "hopSize");
	MATLAB_PUT(lookAheadSamples, "lookAheadSamples");
	MATLAB_EVAL("srcAudio = [];");
	MATLAB_EVAL("onsetAudio = [];");
	MATLAB_EVAL("FluxTS = [];");
	MATLAB_EVAL("segmentData = [];");
	MATLAB_EVAL("onsetTS = [];");

	///////////////////////////////////////////////////////////////////////////////////////
	//process input file (till EOF)
	///////////////////////////////////////////////////////////////////////////////////////
	mrs_natural timestamps_samples = 0;
	mrs_real sampling_rate;
	sampling_rate = onsetnet->getctrl("mrs_real/osrate")->to<mrs_real>();
	// cout << "Sampling rate = " << sampling_rate << endl;
	
	while(onsetnet->getctrl("mrs_bool/notEmpty")->to<mrs_bool>())
	{
		onsetnet->updctrl("Fanout/onsetmix/Series/onsetsynth/ADSR/env/mrs_real/nton", 1.0); //note on
		onsetnet->tick();
		timestamps_samples += onsetnet->getctrl("mrs_natural/onSamples")->to<mrs_natural>();
		// cout << timestamps_samples / sampling_rate << endl;
		cout << timestamps_samples << endl;;
		onsetnet->updctrl("Fanout/onsetmix/Series/onsetsynth/ADSR/env/mrs_real/ntoff", 0.0); //note off
	}
}

int
main(int argc, const char **argv)
{
	string progName = argv[0];  
	if (argc == 1)
		printUsage(progName);

	// handling of command-line options 
	initOptions();
	cmd_options.readOptions(argc, argv);
	loadOptions();

	vector<string> soundfiles = cmd_options.getRemaining();

	string fname0 = EMPTYSTRING;
	string fname1 = EMPTYSTRING;

	if (soundfiles.size() > 0)
		fname0 = soundfiles[0];
	if (soundfiles.size() > 1)  
		fname1 = soundfiles[1];

	cout << "Marsyas onset detection" << endl;
	cout << "fname0 = " << fname0 << endl;
	cout << "fname1 = " << fname1 << endl;
	
	detect_onsets(fname0);
	
}
