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

	//create accumulator series
	MarSystem* preNet = mng.create("Series", "preNet");
	//create fanout for mixing
	MarSystem* fanin = mng.create("Fanin", "fanin");
	// add original source in the fanout
	if (microphone_) 
		fanin->addMarSystem(mng.create("AudioSource", "src"));
	else 
		fanin->addMarSystem(mng.create("SoundFileSource", "src"));
	// create a series for the noiseSource
	MarSystem* mixseries = mng.create("Series", "mixseries");
	if(noiseName == "white")
mixseries->addMarSystem(mng.create("NoiseSource", "noise"));
	else
	mixseries->addMarSystem(mng.create("SoundFileSource", "noise"));

	mixseries->addMarSystem(mng.create("Delay", "noiseDelay"));
	mixseries->addMarSystem(mng.create("Gain", "noiseGain"));
	// add this series in the fanout
	fanin->addMarSystem(mixseries);

	preNet->addMarSystem(fanin);


	preNet->addMarSystem(mng.create("ShiftInput", "si"));
	preNet->addMarSystem(mng.create("Shifter", "sh"));
	preNet->addMarSystem(mng.create("Windowing", "wi"));

	MarSystem *parallel = mng.create("Parallel", "par");
	parallel->addMarSystem(mng.create("Spectrum", "spk1"));
	parallel->addMarSystem(mng.create("Spectrum", "spk2"));
	preNet->addMarSystem(parallel);

	preNet->addMarSystem(mng.create("PeConvert", "conv"));
	//create accumulator
	MarSystem* accumNet = mng.create("Accumulator", "accumNet");
	accumNet->addMarSystem(preNet);

	/*************************************************************/

	MarSystem* peClust = mng.create("PeClust", "peClust");

	/*************************************************************/

	//create the main network
	pvseries->addMarSystem(accumNet);
	pvseries->addMarSystem(peClust);

	/*************************************************************/

	if(synthetize) 
	{
		//create shredder
		synthNetCreate(&mng, outsfname, microphone_);
		MarSystem *peSynth = mng.create("PeSynthetize", "synthNet");
		pvseries->addMarSystem(peSynth);
	}

	////////////////////////////////////////////////////////////////
	// update the controls
	////////////////////////////////////////////////////////////////
	pvseries->updctrl("Accumulator/accumNet/mrs_natural/nTimes", accSize);

	if (microphone_) 
	{
		pvseries->updctrl("Accumulator/accumNet/Series/preNet/Fanin/fanin/AudioSource/src/mrs_natural/inSamples", D);
		pvseries->updctrl("Accumulator/accumNet/Series/preNet/Fanin/fanin/AudioSource/src/mrs_natural/inObservations", 1);
	}
	else
	{
		pvseries->updctrl("Accumulator/accumNet/Series/preNet/Fanin/fanin/SoundFileSource/src/mrs_string/filename", sfName);
		pvseries->updctrl("Accumulator/accumNet/Series/preNet/Fanin/fanin/SoundFileSource/src/mrs_natural/inSamples", D);
		pvseries->updctrl("Accumulator/accumNet/Series/preNet/Fanin/fanin/SoundFileSource/src/mrs_natural/inObservations", 1);
		samplingFrequency_ = pvseries->getctrl("Accumulator/accumNet/Series/preNet/Fanin/fanin/SoundFileSource/src/mrs_real/osrate")->toReal();
	}

	pvseries->updctrl("Accumulator/accumNet/Series/preNet/Fanin/fanin/Series/mixseries/SoundFileSource/noise/mrs_string/filename", noiseName);
	pvseries->updctrl("Accumulator/accumNet/Series/preNet/Fanin/fanin/Series/mixseries/SoundFileSource/noise/mrs_natural/inSamples", D);
	pvseries->updctrl("Accumulator/accumNet/Series/preNet/Fanin/fanin/Series/mixseries/NoiseSource/noise/mrs_string/mode", "rand");
	pvseries->updctrl("Accumulator/accumNet/Series/preNet/Fanin/fanin/Series/mixseries/Delay/noiseDelay/mrs_real/delay",  noiseDelay);
	pvseries->updctrl("Accumulator/accumNet/Series/preNet/Fanin/fanin/Series/mixseries/Gain/noiseGain/mrs_real/gain", noiseGain_);


	pvseries->updctrl("Accumulator/accumNet/Series/preNet/ShiftInput/si/mrs_natural/Decimation", D);
	pvseries->updctrl("Accumulator/accumNet/Series/preNet/ShiftInput/si/mrs_natural/WindowSize", Nw+1);
	pvseries->updctrl("Accumulator/accumNet/Series/preNet/Windowing/wi/mrs_natural/size", N);
	pvseries->updctrl("Accumulator/accumNet/Series/preNet/Windowing/wi/mrs_string/type", "Hanning");
	pvseries->updctrl("Accumulator/accumNet/Series/preNet/Windowing/wi/mrs_natural/zeroPhasing", 1);
	pvseries->updctrl("Accumulator/accumNet/Series/preNet/Shifter/sh/mrs_natural/shift", 1);
	pvseries->updctrl("Accumulator/accumNet/Series/preNet/PvFold/fo/mrs_natural/Decimation", D); // useless ?
	pvseries->updctrl("Accumulator/accumNet/Series/preNet/PeConvert/conv/mrs_natural/Decimation", D);      
	pvseries->updctrl("Accumulator/accumNet/Series/preNet/PeConvert/conv/mrs_natural/Sinusoids", S);  
pvseries->updctrl("Accumulator/accumNet/Series/preNet/PeConvert/conv/mrs_natural/nbFramesSkipped", (N/D));  

	pvseries->setctrl("PeClust/peClust/mrs_natural/Sinusoids", S);  
	pvseries->setctrl("PeClust/peClust/mrs_natural/Clusters", C); 
	pvseries->setctrl("PeClust/peClust/mrs_natural/hopSize", D); 
	pvseries->updctrl("PeClust/peClust/mrs_string/similarityType", T); 

	//pvseries->update();

	if(synthetize)
	{
		synthNetConfigure (pvseries, sfName, outsfname, fileResName, Nw, D, S, accSize, microphone_, bopt_, Nw+1-D);
	}

	if(noiseDuration_)
	{
	ostringstream ossi;
	ossi << ((noiseDelay_+noiseDuration_)*0.1) << "s";
	cout << ossi.str() << endl;
	// touch the gain directly
	// pvseries->updctrl(ossi.str(), Repeat(), new EvValUpd(pvseries,"Accumulator/accumNet/Series/preNet/Fanin/fanin/Series/mixseries/Gain/noiseGain/mrs_real/gain", 0.0));
	//	cout << *pvseries;
	}
	mrs_real globalSnr = 0;
	mrs_natural nb=0;
	//	mrs_real time=0;
	while(1)
	{
		pvseries->tick();

	/*	if(time > (noiseDelay_+noiseDuration_))
		{
	pvseries->updctrl("Accumulator/accumNet/Series/preNet/Fanin/fanin/Series/mixseries/Gain/noiseGain/mrs_real/gain", (mrs_real) 0);
		}
    time+=accSize_*hopSize_/samplingFrequency_;*/
// cout << time << " " << noiseDelay_+noiseDuration_ << endl;
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
			bool temp = pvseries->getctrl("Accumulator/accumNet/Series/preNet/Fanin/fanin/SoundFileSource/src/mrs_bool/notEmpty")->toBool();
			bool temp1 = accumNet->getctrl("Series/preNet/Fanin/fanin/SoundFileSource/src/mrs_bool/notEmpty")->toBool();
			bool temp2 = preNet->getctrl("Fanin/fanin/SoundFileSource/src/mrs_bool/notEmpty")->toBool();
			string fname = pvseries->getctrl("Accumulator/accumNet/Series/preNet/Fanin/fanin/SoundFileSource/src/mrs_string/filename")->toString();

			///*bool*/ temp = pvseries->getctrl("Accumulator/accumNet/Series/preNet/SoundFileSource/src/mrs_bool/notEmpty")->toBool();
			if (temp2 == false)
				break;
		}
	}
	if(synthetize_)
		cout << "Global SNR : " << globalSnr/nb << endl;

	// plot and save peak data
	peakSet = pvseries->getctrl("PeClust/peClust/mrs_realvec/peakSet")->toVec();


	ofstream peakFile;
	peakFile.open(filePeakName.c_str());
	if(!peakFile)
		cout << "Unable to open output Peaks File " << filePeakName << endl;
	peakFile << peakSet_;
	peakFile.close();
}





void clusterGroundThruth(realvec& peakSet, PeClusters& clusters, string fileName)
{

}

void clusterSynthetize(realvec& peakSet, string fileName)
{

}

void 
initOptions()
{
	cmd_options.addBoolOption("help", "h", false);
	cmd_options.addBoolOption("usage", "u", false);
	cmd_options.addNaturalOption("voices", "v", 1);
	cmd_options.addStringOption("filename", "f", EMPTYSTRING);
	cmd_options.addStringOption("noisename", "N", EMPTYSTRING);
	cmd_options.addStringOption("outputdirectoryname", "o", EMPTYSTRING);
	cmd_options.addStringOption("inputdirectoryname", "i", EMPTYSTRING);
	cmd_options.addNaturalOption("winsize", "w", winSize_);
	cmd_options.addNaturalOption("fftsize", "n", fftSize_);
	cmd_options.addNaturalOption("sinusoids", "s", nbSines_);
	cmd_options.addNaturalOption("bufferSize", "b", bopt_);

	cmd_options.addBoolOption("analyse", "a", analyse_);
	cmd_options.addBoolOption("attributes", "A", attributes_);
	cmd_options.addBoolOption("ground", "g", ground_);
	cmd_options.addBoolOption("synthetize", "s", synthetize_);
	cmd_options.addBoolOption("clusterSynthetize", "S", clusterSynthetize_);
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
	noiseName = cmd_options.getStringOption("noisename");
	winSize_ = cmd_options.getNaturalOption("winsize");
	fftSize_ = cmd_options.getNaturalOption("fftsize");
	nbSines_ = cmd_options.getNaturalOption("sinusoids");
	bopt_ = cmd_options.getNaturalOption("bufferSize");

	analyse_ = cmd_options.getBoolOption("analyse");
	attributes_ = cmd_options.getBoolOption("attributes");
	ground_ = cmd_options.getBoolOption("ground");
	synthetize_ = cmd_options.getBoolOption("synthetize");
	clusterSynthetize_ = cmd_options.getBoolOption("clusterSynthetize");

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
				updateLabels(peakSet_, clusters.getConversionTable());

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


