#include <cstdio>

#include "MarSystemManager.h"
#include "AudioSink.h"
#include "SoundFileSink.h"
#include "SoundFileSource.h"
#include "Gain.h"
#include "Messager.h"
#include "Conversions.h"
#include "CommandLineOptions.h"
#include "PeClusters.h"
#include "PeUtilities.h"

#include <string>

using namespace std;
using namespace Marsyas;


string pluginName = EMPTYSTRING;
string inputDirectoryName = EMPTYSTRING;
string outputDirectoryName = EMPTYSTRING;
string fileName = EMPTYSTRING;
string noiseName = EMPTYSTRING;
string fileResName = EMPTYSTRING;
string filePeakName = EMPTYSTRING;

// Global variables for command-line options 
bool helpopt_ = 0;
bool usageopt_ =0;
int fftSize_ = 2048;
int winSize_ = 2048;
// if kept the same no time expansion
int hopSize_ = 512;
// nb Sines
int nbSines_ = 15;
// nbClusters
int nbClusters_ = 3;
// output buffer Size
int bopt_ = 128;
// output gain
mrs_real gopt_ = 1.0;
// number of accumulated frames
mrs_natural accSize_ = 1;
// type of similarity Metrics
string similarityType_ = "ho";
// store for clustered peaks
realvec peakSet_;
// delay for noise insertion
mrs_real noiseDelay_=0;
// gain for noise insertion
mrs_real noiseGain_=.1;
// duration for noise insertion
mrs_real noiseDuration_=0;
// sampling frequency
mrs_real samplingFrequency_=1;

bool microphone_ = false;
bool analyse_ = false;
bool attributes_ = false;
bool ground_ = false;
bool synthetize_ = false;
bool clusterSynthetize_ = false;

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
clusterExtract(realvec &peakSet, string sfName, string outsfname, string noiseName, mrs_real noiseDelay, string T, mrs_natural N, mrs_natural Nw, 
							 mrs_natural D, mrs_natural S, mrs_natural C,
							 mrs_natural accSize, bool synthetize)
{
	cout << "Extracting Peaks and Clusters" << endl;
	MarSystemManager mng;

	// create the phasevocoder network
	MarSystem* pvseries = mng.create("Series", "pvseries");

	// add original source 
	if (microphone_) 
		pvseries->addMarSystem(mng.create("AudioSource", "src"));
	else 
		pvseries->addMarSystem(mng.create("SoundFileSource", "src"));
	// create analyser
	pvseries->addMarSystem(mng.create("PeAnalyse", "peA"));

	


	////////////////////////////////////////////////////////////////
	// update the controls
	////////////////////////////////////////////////////////////////
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
		samplingFrequency_ = pvseries->getctrl("SoundFileSource/src/mrs_real/osrate")->toReal();
	}

	
	pvseries->updctrl("PeAnalyse/peA/ShiftInput/si/mrs_natural/Decimation", D);
	pvseries->updctrl("PeAnalyse/peA/ShiftInput/si/mrs_natural/WindowSize", Nw+1);
	pvseries->updctrl("PeAnalyse/peA/Windowing/wi/mrs_natural/size", N);
	pvseries->updctrl("PeAnalyse/peA/Windowing/wi/mrs_string/type", "Hanning");
	pvseries->updctrl("PeAnalyse/peA/Windowing/wi/mrs_natural/zeroPhasing", 1);
	pvseries->updctrl("PeAnalyse/peA/Shifter/sh/mrs_natural/shift", 1);
	pvseries->updctrl("PeAnalyse/peA/PvFold/fo/mrs_natural/Decimation", D); // useless ?
	pvseries->updctrl("PeAnalyse/peA/PeConvert/conv/mrs_natural/Decimation", D);      
	pvseries->updctrl("PeAnalyse/peA/PeConvert/conv/mrs_natural/Sinusoids", S);  
  pvseries->updctrl("PeAnalyse/peA/PeConvert/conv/mrs_natural/nbFramesSkipped", (N/D));  

	//pvseries->update();

	if(synthetize)
	{
		synthNetConfigure (pvseries, sfName, outsfname, fileResName, Nw, D, S, accSize, microphone_, bopt_, Nw+1-D);
	}

	mrs_real globalSnr = 0;
	mrs_natural nb=0;
	//	mrs_real time=0;
	while(1)
	{
		pvseries->tick();
	// ouput the seg snr
		if(synthetize)
		{
			mrs_real snr = pvseries->getctrl("Shredder/synthNet/Series/postNet/PeResidual/res/mrs_real/snr")->toReal();
			globalSnr+=snr;
			nb++;
			cout << "Frame " << nb << " SNR : "<< snr << endl;
		}

		if (!microphone_)
		{
			bool temp = pvseries->getctrl("PeAnalyse/peA/Fanin/fanin/SoundFileSource/src/mrs_bool/notEmpty")->toBool();
			string fname = pvseries->getctrl("PeAnalyse/peA/Fanin/fanin/SoundFileSource/src/mrs_string/filename")->toString();

			///*bool*/ temp = pvseries->getctrl("PeAnalyse/peA/SoundFileSource/src/mrs_bool/notEmpty")->toBool();
			if (temp == false)
				break;
		}
	}
	if(synthetize_)
		cout << "Global SNR : " << globalSnr/nb << endl;
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
	cmd_options.addNaturalOption("winsize", "w", winSize_);
	cmd_options.addNaturalOption("fftsize", "n", fftSize_);
	cmd_options.addNaturalOption("sinusoids", "s", nbSines_);
	cmd_options.addNaturalOption("bufferSize", "b", bopt_);
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
	nbSines_ = cmd_options.getNaturalOption("sinusoids");
	bopt_ = cmd_options.getNaturalOption("bufferSize");
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
	cerr << "sinusoids (-s)     = " << nbSines_ << endl;
	cerr << "outFile  (-f)      = " << fileName << endl;
	cerr << "outputDirectory  (-o) = " << outputDirectoryName << endl;
	cerr << "inputDirectory  (-i) = " << inputDirectoryName << endl;

	// extract peaks and clusters
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
				fileResName = outputDirectoryName + "/" + Sfname.nameNoExt() + "Res." + Sfname.ext() ;
				filePeakName = outputDirectoryName + "/" + Sfname.nameNoExt() + "Peak.txt" ;
				cout << fileResName << endl;
			}
			if(analyse_)
			{
				cout << "Phasevocoding " << Sfname.name() << endl; 
				clusterExtract(peakSet_, *sfi, fileName, noiseName, noiseDelay_, similarityType_, fftSize_, winSize_, hopSize_, nbSines_, nbClusters_, accSize_, synthetize_);
			}	
			// if ! peak data read from file
			if(peakSet_.getSize() == 0)
				peakSet_.read(filePeakName);
			if(peakSet_.getSize() == 0)
			{
				cout << "unable to load " << filePeakName << endl;
				exit(1);
			}

			MATLAB_PUT(peakSet_, "peaks");
			MATLAB_EVAL("plotPeaks(peaks)");


			// computes the cluster attributes

			if(attributes_)
			{
				PeClusters clusters(peakSet_);
				mrs_natural nbClusters=0;

				// compute ground truth
				if(ground_)
					clusters.synthetize(peakSet_, *sfi, fileName, winSize_, hopSize_, nbSines_, bopt_, 1);


				clusters.selectGround();
				// commented out by gtzan because getConversionTable needs a realvec argument 
				// updateLabels(peakSet_, clusters.getConversionTable());

						MATLAB_PUT(peakSet_, "peaksGp");
			MATLAB_EVAL("plotPeaks(peaksGp)");
			}
	
			if(clusterSynthetize_)
			{
        PeClusters clusters(peakSet_);
				// synthetize remaining clusters
				clusters.synthetize(peakSet_, *sfi, fileName, winSize_, hopSize_, nbSines_, bopt_);
			}
			MATLAB_PUT(peakSet_, "peaks");
			MATLAB_EVAL("plotPeaks(peaks)");
		}
	}
	else
	{
		cout << "Using live microphone input" << endl;
		microphone_ = true;
		clusterExtract(peakSet_, "microphone", fileName, noiseName, noiseDelay_, similarityType_, fftSize_, winSize_, hopSize_, nbSines_, nbClusters_, accSize_, synthetize_);
	}



	exit(0);
}


