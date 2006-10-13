#include <cstdio>

#include "RtMidi.h"
#include "MarSystemManager.h"
#include "Messager.h"
#include "Conversions.h"
#include "CommandLineOptions.h"

#include <string>

using namespace std;
using namespace Marsyas;

#define EMPTYSTRING "MARSYAS_EMPTY"
string pluginName = EMPTYSTRING;
string inputDirectoryName = EMPTYSTRING;
string outputDirectoryName = EMPTYSTRING;
string fileName = EMPTYSTRING;

// Global variables for command-line options 
bool helpopt_ = 0;
bool usageopt_ =0;
int fftSize_ = 512;
int winSize_ = 512;
// if kept the same no time expansion
int dopt = 64;
int iopt = 64;
// nb Sines
int sopt = 6;
// output buffer Size
int bopt = 128;
mrs_real gopt_ = 1.0;
mrs_natural eopt_ = 0;

mrs_natural accSize = 20;

float popt = 1.0;
bool auto_ = false;

bool microphone_ = false;


CommandLineOptions cmd_options;



void 
printUsage(string progName)
{
  MRSDIAG("peakClustering.cpp - printUsage");
  cerr << "Usage : " << progName << " [file]" << endl;
  cerr << endl;
  cerr << "If no filename is given the default live audio input is used. " << endl;
}

void 
printHelp(string progName)
{
  MRSDIAG("peakClustering.cpp - printHelp");
  cerr << "peakClustering, MARSYAS, Copyright Mathieu Lagrange " << endl;
  cerr << "--------------------------------------------" << endl;
  cerr << "Usage : " << progName << " [file]" << endl;
  cerr << endl;
  cerr << "if no filename is given the default live audio input is used. " << endl;
  cerr << "Options:" << endl;
  cerr << "-n --fftsize         : size of fft " << endl;
  cerr << "-w --winsize         : size of window " << endl;
  cerr << "-s --sinusoids       : number of sinusoids" << endl;
  cerr << "-b --buffersize      : audio buffer size" << endl;
  cerr << "-g --gain            : gain (0.0-1.0) " << endl;
  cerr << "-f --filename        : output filename" << endl;
  cerr << "-o --outputdirectorypath   : output directory path" << endl;
  cerr << "-i --inputdirectorypath   : input directory path" << endl;
  cerr << "-u --usage           : display short usage info" << endl;
  cerr << "-h --help            : display this information " << endl;
  
  exit(1);
}



// original monophonic peakClustering 
void 
phasevocSeries(string sfName, mrs_natural N, mrs_natural Nw, 
							 mrs_natural D, mrs_natural I, mrs_real P, 
							 string outsfname, mrs_natural accSize)
{
	cout << "phasevocSeries" << endl;
	MarSystemManager mng;

	// create the peakClustering network
	MarSystem* pvseries = mng.create("Series", "pvseries");

	if (microphone_) 
		pvseries->addMarSystem(mng.create("AudioSource", "src"));
	else 
		pvseries->addMarSystem(mng.create("SoundFileSource", "src"));

	MarSystem* pvAccu = mng.create("Accumulator", "pvAccu");
  MarSystem* preSeries = mng.create("Series", "preSeries");
	preSeries->addMarSystem(mng.create("ShiftInput", "si"));
	preSeries->addMarSystem(mng.create("PvFold", "fo"));
	preSeries->addMarSystem(mng.create("Spectrum", "spk"));
	preSeries->addMarSystem(mng.create("PvConvert", "conv"));
	pvAccu->addMarSystem(preSeries);
	
	//pvAccu->addMarSystem(mng.create("ShiftInput", "si"));
	pvseries->addMarSystem(pvAccu);
	//pvAccu->addMarSystem(mng.create("ShiftInput", "si"));
	
	MarSystem* pvShred = mng.create("Shredder", "pvShred");
	MarSystem* postSeries = mng.create("Series", "postSeries");

	postSeries->addMarSystem(mng.create("PvOscBank", "ob"));
	postSeries->addMarSystem(mng.create("ShiftOutput", "so"));
	postSeries->addMarSystem(mng.create("Gain", "gain"));
  pvShred->addMarSystem(postSeries);

	pvseries->addMarSystem(pvShred);

	MarSystem *dest;
	if (outsfname == EMPTYSTRING) 
		dest = new AudioSink("dest");
	else
	{
		dest = new SoundFileSink("dest");
		dest->updctrl("mrs_string/filename", outsfname);
	}
	pvseries->addMarSystem(dest);


	if (outsfname == EMPTYSTRING) 
		pvseries->updctrl("AudioSink/dest/mrs_natural/bufferSize", bopt);


	// update the controls
	if (microphone_) 
	{
		pvseries->updctrl("AudioSource/src/mrs_natural/inSamples", D);
		pvseries->updctrl("AudioSource/src/mrs_natural/inObservations", 1);
	}
	else
	{
		pvseries->updctrl("SoundFileSource/src/mrs_string/filename", sfName);
		pvseries->updctrl("SoundFileSource/src/mrs_natural/inSamples", D);
		pvseries->updctrl("SoundFileSource/src/mrs_natural/inObservations", 1);
	}


	preSeries->updctrl("ShiftInput/si/mrs_natural/Decimation", D);
	preSeries->updctrl("ShiftInput/si/mrs_natural/WindowSize", Nw);
	preSeries->updctrl("PvFold/fo/mrs_natural/FFTSize", N);
	preSeries->updctrl("PvFold/fo/mrs_natural/WindowSize", Nw);
	preSeries->updctrl("PvFold/fo/mrs_natural/Decimation", D);
	preSeries->updctrl("PvConvert/conv/mrs_natural/Decimation",D);      
	preSeries->updctrl("PvConvert/conv/mrs_natural/Sinusoids", (mrs_natural) sopt);  
	
	pvseries->updctrl("Accumulator/pvAccu/mrs_natural/nTimes", accSize);
	pvseries->updctrl("Shredder/pvShred/mrs_natural/nTimes", accSize);

	postSeries->updctrl("PvOscBank/ob/mrs_natural/Interpolation", I);
	postSeries->updctrl("PvOscBank/ob/mrs_real/PitchShift", P);
	postSeries->updctrl("ShiftOutput/so/mrs_natural/Interpolation", I);
	postSeries->updctrl("ShiftOutput/so/mrs_natural/WindowSize", Nw);      
	postSeries->updctrl("ShiftOutput/so/mrs_natural/Decimation", D);
	postSeries->updctrl("Gain/gain/mrs_real/gain", gopt_);

	int i=0;
	while(1)
	{
		cout << i++ << " ";
		pvseries->tick();

		if (!microphone_) 
			if (pvseries->getctrl("SoundFileSource/src/mrs_bool/notEmpty").toBool() == false)
				break;
	}
}







void 
initOptions()
{
  cmd_options.addBoolOption("help", "h", false);
  cmd_options.addBoolOption("usage", "u", false);
  cmd_options.addNaturalOption("voices", "v", 1);
  cmd_options.addStringOption("filename", "f", EMPTYSTRING);
	cmd_options.addStringOption("outputdirectoryname", "o", EMPTYSTRING);
	cmd_options.addStringOption("inputdirectoryname", "i", EMPTYSTRING);
  cmd_options.addStringOption("plugin", "p", EMPTYSTRING);
  cmd_options.addNaturalOption("winsize", "w", winSize_);
  cmd_options.addNaturalOption("fftsize", "n", fftSize_);
  cmd_options.addNaturalOption("sinusoids", "s", sopt);
  cmd_options.addNaturalOption("bufferSize", "b", bopt);
  cmd_options.addRealOption("gain", "g", 1.0);
  cmd_options.addBoolOption("auto", "a", auto_);
}


void 
loadOptions()
{
  helpopt_ = cmd_options.getBoolOption("help");
  usageopt_ = cmd_options.getBoolOption("usage");
  pluginName = cmd_options.getStringOption("plugin");
  fileName   = cmd_options.getStringOption("filename");
  inputDirectoryName = cmd_options.getStringOption("inputdirectoryname");
  outputDirectoryName = cmd_options.getStringOption("outputdirectoryname");
  winSize_ = cmd_options.getNaturalOption("winsize");
  fftSize_ = cmd_options.getNaturalOption("fftsize");
  sopt = cmd_options.getNaturalOption("sinusoids");
  bopt = cmd_options.getNaturalOption("bufferSize");
  auto_ = cmd_options.getBoolOption("auto");
  gopt_ = cmd_options.getRealOption("gain");
}



int
main(int argc, const char **argv)
{
  MRSDIAG("sftransform.cpp - main");


  initOptions();
  cmd_options.readOptions(argc, argv);
  loadOptions();  

  vector<string> soundfiles = cmd_options.getRemaining();
  vector<string>::iterator sfi;


  string progName = argv[0];  
  
  
  if (helpopt_) 
    printHelp(progName);
  
  if (usageopt_)
    printUsage(progName);
  
  
  cerr << "peakClustering configuration (-h show the options): " << endl;
  cerr << "fft size (-n)      = " << fftSize_ << endl;
  cerr << "win size (-w)      = " << winSize_ << endl;
  cerr << "sinusoids (-s)     = " << sopt << endl;
  cerr << "outFile  (-f)      = " << fileName << endl;
	cerr << "outputDirectory  (-o) = " << outputDirectoryName << endl;
	cerr << "inputDirectory  (-i) = " << inputDirectoryName << endl;

  // soundfile input 
  string sfname;
	if (soundfiles.size() != 0)   
	{
		// process several soundFiles
		for (sfi=soundfiles.begin() ; sfi!=soundfiles.end() ; sfi++)
		{
				FileName Sfname(*sfi);
			if(outputDirectoryName != EMPTYSTRING)
			{
			
				fileName = outputDirectoryName + "/" + Sfname.name() ;
			}
			cout << "Phasevocoding " << Sfname.name() << endl; 
			phasevocSeries(*sfi, fftSize_, winSize_, dopt, iopt, popt, fileName, accSize);
		}
	}
	else
	{
		cout << "Using live microphone input" << endl;
		microphone_ = true;
		phasevocSeries("microphone", fftSize_, winSize_, dopt, iopt, popt, fileName, accSize);
	}

	exit(0);
  
}

	
