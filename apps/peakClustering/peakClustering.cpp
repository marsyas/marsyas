// #include <vld.h>

#include <cstdio>
#include<iostream>
#include<iomanip>

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
#include <time.h>
#include <string>

using namespace std;
using namespace Marsyas;

string pluginName = EMPTYSTRING;
string inputDirectoryName = EMPTYSTRING;
string outputDirectoryName = EMPTYSTRING;
string fileName = EMPTYSTRING;
string noiseName = EMPTYSTRING;
string fileResName = EMPTYSTRING;
string mixName = EMPTYSTRING;
string filePeakName = EMPTYSTRING;
string fileClustName = EMPTYSTRING;
string fileVoicingName = EMPTYSTRING;
string fileF0Name = EMPTYSTRING;
string panningInfo = EMPTYSTRING;
// set the seeking frequency interval for peaks
string intervalFrequency = "250_2500";

// Global variables for command-line options 
bool helpopt_ = 0;
bool usageopt_ =0;
int fftSize_ = 2048;
int winSize_ = 2048;
// if kept the same no time expansion
int hopSize_ = 512;
// nb Sines
int nbSines_ = 20;
// nbClusters
int nbClusters_ = 2;
// nbClusters
int nbSelectedClusters_ = 0;
// output buffer Size
int bopt_ = 128;
// output gain
mrs_real gopt_ = 1.0;
// number of accumulated frames
mrs_natural accSize_ = 10;
// number of seconds for analysing process
mrs_natural stopAnalyse_=0;
// type of similarity Metrics // test amplitude normamlise gtzan
string defaultSimilarityType_ = "hoabfb";
string similarityType_ = EMPTYSTRING;
// weight for similarity Metrics
realvec similarityWeight_;	
// store for clustered peaks 
realvec peakSet_;
// delay for noise insertion

mrs_real noiseDelay_=0;
// gain for noise insertion
mrs_real noiseGain_=.8;
// duration for noise insertion
mrs_real noiseDuration_=0;
// sampling frequency
mrs_real samplingFrequency_=1;
//
mrs_real timeElapsed;
//
mrs_natural nbTicks=0;
//
mrs_natural clusterFilteringType_ = 0;
//
mrs_natural fileInfo_=0;
//
mrs_natural unprecise_;
//
mrs_natural noPeakPicking_;

bool microphone_ = false;
bool analyse_ = true;
bool attributes_ = false;
bool ground_ = false;
mrs_natural synthetize_ = -1;
mrs_natural clusterSynthetize_ = -1;
bool peakStore_= false;
bool residual_ = 0;

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
	cerr << "report bugs to lagrange@uvic.ca" << endl;
	cerr << "--------------------------------------------" << endl;
	cerr << "Usage : " << progName << " [file]" << endl;
	cerr << endl;
	cerr << "if no filename is given the default live audio input is used. " << endl;
	cerr << "Options:" << endl;
	cerr << "-n --fftsize         : size of fft " << endl;
	cerr << "-w --winsize         : size of window " << endl;
	cerr << "-s --sinusoids       : number of sinusoids per frame" << endl;
	cerr << "-b --buffersize      : audio buffer size" << endl;
	cerr << "-o --outputdirectoryname   : output directory path" << endl;
	cerr << "-N --noisename : name of degrading audio file " << endl;
	cerr << "-p --panning : panning informations <foreground level (0..1)>-<foreground pan (-1..1)>-<background level>-<background pan> " << endl;
	cerr << "-t --typeSimilarity : similarity information a (amplitude) f (frequency) h (harmonicity)  " << endl;
	cerr << "-q -quitAnalyse : quit processing after specified number f seconds" << endl;
	cerr << "-T --textureSize: number of frames in a texture window" << endl;
	cerr << "-c -clustering : number of clusters in a texture window" << endl;
	cerr << "-k -keep : keep the specified number of clusters in the texture window " << endl;
	cerr << "-S --synthetise : synthetize using an oscillator bank (0), an IFFT mono (1), or an IFFT stereo (2)" << endl;
	cerr << "-r --residual : output the residual sound (if the synthesis stage is selected)" << endl;
	cerr << "-i --intervalFrequency : <minFrequency>_<maxFrequency> select peaks in this interval (default 250-2500 Hz)" << endl;
	cerr << "-f --fileInfo : provide clustering parameters in the output name (s20t10i250_2500c2k1uTabfbho means 20 sines per frames in the 250_2500 Hz frequency Interval, 1 cluster selected among 2 in one texture window of 10 frames, no precise parameter estimation and using a combination of similarities abfbho)" << endl;
	cerr << "-npp --noPeakPicking : do not perform peak picking in the spectrum" << endl;
	cerr << "-u --unprecise : do not perform precise estimation of sinusoidal parameters" << endl;
	cerr << "" << endl;
	cerr << "-h --help            : display this information " << endl;

	exit(1);
}


// original monophonic peakClustering 
void
clusterExtract(realvec &peakSet, string sfName, string outsfname, string noiseName, string mixName, string intervalFrequency, string panningInfo, mrs_real noiseDelay, string T, mrs_natural N, mrs_natural Nw, 
							 mrs_natural D, mrs_natural S, mrs_natural C,
							 mrs_natural accSize, mrs_natural synthetize, mrs_real *snr0)
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
	if(noiseName != EMPTYSTRING)
	{
	MarSystem* mixseries = mng.create("Series", "mixseries");

	if(noiseName == "white")
		mixseries->addMarSystem(mng.create("NoiseSource", "noise"));
	else
		mixseries->addMarSystem(mng.create("SoundFileSource", "noise"));

	mixseries->addMarSystem(mng.create("Delay", "noiseDelay"));
	MarSystem* noiseGain = mng.create("Gain", "noiseGain");
	mixseries->addMarSystem(noiseGain);
	// add this series in the fanout
	fanin->addMarSystem(mixseries);
	}
	preNet->addMarSystem(fanin);
	//////////////////////////////////////////////////////
	// should be removed for weird command line problems

		if(noiseName != EMPTYSTRING)
	preNet->addMarSystem(mng.create("SoundFileSink", "mixSink"));
	////////////////////////////////////////
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
	if(peakStore_)
		pvseries->addMarSystem(mng.create("RealvecSink", "peSink"));
	/*************************************************************/

	if(synthetize >-1) 
	{
		//create shredder
		synthNetCreate(&mng, outsfname, microphone_, synthetize, residual_);
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

if(noiseName != EMPTYSTRING)
{
	pvseries->updctrl("Accumulator/accumNet/Series/preNet/Fanin/fanin/Series/mixseries/SoundFileSource/noise/mrs_string/filename", noiseName);
	pvseries->updctrl("Accumulator/accumNet/Series/preNet/Fanin/fanin/Series/mixseries/SoundFileSource/noise/mrs_natural/inSamples", D);
	pvseries->updctrl("Accumulator/accumNet/Series/preNet/Fanin/fanin/Series/mixseries/NoiseSource/noise/mrs_string/mode", "rand");
	pvseries->updctrl("Accumulator/accumNet/Series/preNet/Fanin/fanin/Series/mixseries/Delay/noiseDelay/mrs_real/delay",  noiseDelay);
	pvseries->updctrl("Accumulator/accumNet/Series/preNet/Fanin/fanin/Series/mixseries/Gain/noiseGain/mrs_real/gain", noiseGain_);
}

	pvseries->updctrl("Accumulator/accumNet/Series/preNet/ShiftInput/si/mrs_natural/Decimation", D);
	pvseries->updctrl("Accumulator/accumNet/Series/preNet/ShiftInput/si/mrs_natural/WindowSize", Nw+1);
	pvseries->updctrl("Accumulator/accumNet/Series/preNet/Windowing/wi/mrs_natural/size", N);
	pvseries->updctrl("Accumulator/accumNet/Series/preNet/Windowing/wi/mrs_string/type", "Hanning");
	pvseries->updctrl("Accumulator/accumNet/Series/preNet/Windowing/wi/mrs_natural/zeroPhasing", 1);
	pvseries->updctrl("Accumulator/accumNet/Series/preNet/Shifter/sh/mrs_natural/shift", 1);
	//pvseries->updctrl("Accumulator/accumNet/Series/preNet/PvFold/fo/mrs_natural/Decimation", D); // useless ?
	pvseries->updctrl("Accumulator/accumNet/Series/preNet/PeConvert/conv/mrs_natural/Decimation", D);
	if(unprecise_)
	pvseries->updctrl("Accumulator/accumNet/Series/preNet/PeConvert/conv/mrs_natural/improvedPrecision", 0);      
	else
		pvseries->updctrl("Accumulator/accumNet/Series/preNet/PeConvert/conv/mrs_natural/improvedPrecision", 1);  
	if(noPeakPicking_)
	pvseries->updctrl("Accumulator/accumNet/Series/preNet/PeConvert/conv/mrs_natural/picking", 0);      
	pvseries->updctrl("Accumulator/accumNet/Series/preNet/PeConvert/conv/mrs_natural/Sinusoids", S); 
	pvseries->updctrl("Accumulator/accumNet/Series/preNet/PeConvert/conv/mrs_string/frequencyInterval", intervalFrequency);  
	// pvseries->updctrl("Accumulator/accumNet/Series/preNet/PeConvert/conv/mrs_natural/nbFramesSkipped", (N/D));  

	pvseries->setctrl("PeClust/peClust/mrs_natural/Sinusoids", S);  
	pvseries->setctrl("PeClust/peClust/mrs_natural/Clusters", C); 
	pvseries->setctrl("PeClust/peClust/mrs_natural/selectedClusters", nbSelectedClusters_); 
	pvseries->setctrl("PeClust/peClust/mrs_natural/hopSize", D); 
	pvseries->setctrl("PeClust/peClust/mrs_natural/storePeaks", (mrs_natural) peakStore_); 
	pvseries->updctrl("PeClust/peClust/mrs_string/similarityType", T); 


	similarityWeight_.stretch(3);
	similarityWeight_(0) = 1;
	similarityWeight_(1) = 10;
	similarityWeight_(2) = 1;

	pvseries->updctrl("PeClust/peClust/mrs_realvec/similarityWeight", similarityWeight_); 
	pvseries->updctrl("RealvecSink/peSink/mrs_string/fileName", filePeakName);

	if(noiseName != EMPTYSTRING)
	pvseries->updctrl("Accumulator/accumNet/Series/preNet/SoundFileSink/mixSink/mrs_string/filename", mixName);//[!]

	//pvseries->update();

	if(synthetize>-1)
	{
		synthNetConfigure (pvseries, sfName, outsfname, fileResName, panningInfo, 1, Nw, D, S, accSize, microphone_, synthetize_, bopt_, Nw+1-D, residual_);
	}

	if(noiseDuration_)
	{
		ostringstream ossi;
		ossi << ((noiseDelay_+noiseDuration_)) << "s";
		cout << ossi.str() << endl;
		// touch the gain directly
		//	noiseGain->updctrl("0.1s", Repeat("0.1s", 1), new EvValUpd(noiseGain,"mrs_real/gain", 0.0));

	}

	//	cout << *pvseries << endl;

	mrs_real globalSnr = 0;
	mrs_natural nb=0;
	//	mrs_real time=0;
	while(1)
	{	
		//		cout << "tick"<<endl;
		pvseries->tick();
		//	cout << "tick"<<endl;
		/*	if(time > (noiseDelay_+noiseDuration_))
		{
		pvseries->updctrl("Accumulator/accumNet/Series/preNet/Fanin/fanin/Series/mixseries/Gain/noiseGain/mrs_real/gain", (mrs_real) 0);
		}
		time+=accSize_*hopSize_/samplingFrequency_;*/
		// cout << time << " " << noiseDelay_+noiseDuration_ << endl;
		// ouput the seg snr
		if(synthetize > -1 && residual_)
		{
			mrs_real snr = pvseries->getctrl("PeSynthetize/synthNet/Series/postNet/PeResidual/res/mrs_real/snr")->toReal();
			globalSnr+=snr;
			nb++;
			// cout << "Frame " << nb << " SNR : "<< snr << endl;
		}

		if (!microphone_)
		{
			bool temp = pvseries->getctrl("Accumulator/accumNet/Series/preNet/Fanin/fanin/SoundFileSource/src/mrs_bool/notEmpty")->toBool();
			bool temp1 = accumNet->getctrl("Series/preNet/Fanin/fanin/SoundFileSource/src/mrs_bool/notEmpty")->toBool();
			bool temp2 = preNet->getctrl("Fanin/fanin/SoundFileSource/src/mrs_bool/notEmpty")->toBool();

			mrs_real timeRead =  preNet->getctrl("Fanin/fanin/SoundFileSource/src/mrs_natural/pos")->toNatural()/samplingFrequency_;
			mrs_real timeLeft;
			if(!stopAnalyse_)
				timeLeft =  preNet->getctrl("Fanin/fanin/SoundFileSource/src/mrs_natural/size")->toNatural()/samplingFrequency_;
			else
				timeLeft = stopAnalyse_;
			// string fname = pvseries->getctrl("Accumulator/accumNet/Series/preNet/Fanin/fanin/SoundFileSource/src/mrs_string/filename")->toString();
			printf("  %.2f / %.2f \r", timeRead, timeLeft);
			fflush(stdout);
			//cout << fixed << setprecision(2) << timeRead << "/" <<  setprecision(2) << timeLeft;
			///*bool*/ temp = pvseries->getctrl("Accumulator/accumNet/Series/preNet/SoundFileSource/src/mrs_bool/notEmpty")->toBool();
			if (temp2 == false || (stopAnalyse_ !=0 && stopAnalyse_<timeRead))
				break;
		}
	}
	if(synthetize_ > -1)
	{
		cout << "Global SNR : " << globalSnr/nb << endl;
		*snr0 = globalSnr/nb;
	}

	if(peakStore_)
	{
		realvec vec = pvseries->getctrl("RealvecSink/peSink/mrs_realvec/data")->toVec();
		pvseries->updctrl("RealvecSink/peSink/mrs_bool/done", true);
		peakStore(vec, filePeakName, samplingFrequency_, D); 
	}
}



void 
initOptions()
{
	cmd_options.addBoolOption("help", "h", false);
	cmd_options.addBoolOption("usage", "us", false);
	cmd_options.addNaturalOption("voices", "v", 1);
	cmd_options.addStringOption("noisename", "N", EMPTYSTRING);
	cmd_options.addStringOption("outputdirectoryname", "o", EMPTYSTRING);
	cmd_options.addStringOption("intervalFrequency", "i", intervalFrequency);
	cmd_options.addStringOption("panning", "p", EMPTYSTRING);
	cmd_options.addStringOption("typeSimilarity", "t", defaultSimilarityType_);
	cmd_options.addNaturalOption("winsize", "w", winSize_);
	cmd_options.addNaturalOption("fftsize", "n", fftSize_);
	cmd_options.addNaturalOption("sinusoids", "s", nbSines_);
	cmd_options.addNaturalOption("bufferSize", "b", bopt_);
	cmd_options.addNaturalOption("quitAnalyse", "q", stopAnalyse_);
	cmd_options.addNaturalOption("clustering", "c", nbClusters_);
	cmd_options.addNaturalOption("keep", "k", nbSelectedClusters_);
  cmd_options.addNaturalOption("textureSize", "T", accSize_);
	cmd_options.addNaturalOption("clusterFiltering", "F", clusterFilteringType_);
	cmd_options.addBoolOption("fileInfo", "f", 0);
  cmd_options.addBoolOption("residual", "r", 0);
 cmd_options.addBoolOption("unprecise", "u", 0);
	// cmd_options.addBoolOption("analyse", "a", analyse_);
	cmd_options.addBoolOption("attributes", "A", attributes_);
	cmd_options.addBoolOption("ground", "g", ground_);
	cmd_options.addNaturalOption("synthetize", "S", synthetize_);
		cmd_options.addBoolOption("noPeakPicking", "npp", 0);
	cmd_options.addNaturalOption("clusterSynthetize", "SC", clusterSynthetize_);
	cmd_options.addBoolOption("peakStore", "P", peakStore_);
}


void 
loadOptions()
{
	helpopt_ = cmd_options.getBoolOption("help");
	usageopt_ = cmd_options.getBoolOption("usage");
	pluginName = cmd_options.getStringOption("plugin");
	fileName   = cmd_options.getStringOption("filename");
	intervalFrequency = cmd_options.getStringOption("intervalFrequency");
	panningInfo = cmd_options.getStringOption("panning");
	outputDirectoryName = cmd_options.getStringOption("outputdirectoryname");
	similarityType_ = cmd_options.getStringOption("typeSimilarity");
	noiseName = cmd_options.getStringOption("noisename");
	winSize_ = cmd_options.getNaturalOption("winsize");
	fftSize_ = cmd_options.getNaturalOption("fftsize");
	nbSines_ = cmd_options.getNaturalOption("sinusoids");
	bopt_ = cmd_options.getNaturalOption("bufferSize");
	stopAnalyse_ = cmd_options.getNaturalOption("quitAnalyse");
	nbClusters_ = cmd_options.getNaturalOption("clustering");
	nbSelectedClusters_ = cmd_options.getNaturalOption("keep");
	accSize_ = cmd_options.getNaturalOption("textureSize");
	clusterFilteringType_ = cmd_options.getNaturalOption("clusterFiltering");
	fileInfo_ = cmd_options.getBoolOption("fileInfo");
	// analyse_ = cmd_options.getBoolOption("analyse");
	attributes_ = cmd_options.getBoolOption("attributes");
	ground_ = cmd_options.getBoolOption("ground");
	synthetize_ = cmd_options.getNaturalOption("synthetize");
	noPeakPicking_ = cmd_options.getBoolOption("noPeakPicking");
	clusterSynthetize_ = cmd_options.getNaturalOption("clusterSynthetize");
	peakStore_ = cmd_options.getBoolOption("peakStore"); 
	residual_ = cmd_options.getBoolOption("residual");
	unprecise_ = cmd_options.getBoolOption("unprecise");

}



int
main(int argc, const char **argv)
{
	MRSDIAG("sftransform.cpp - main");

	mrs_real snr0=0;
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

	nbTicks = clock();
	// extract peaks and clusters
	// soundfile input 
	string sfname;
	if (soundfiles.size() != 0)   
	{
		// process several soundFiles
		for (sfi=soundfiles.begin() ; sfi!=soundfiles.end() ; sfi++)
		{
			FileName Sfname(*sfi);
			string path, outputInfo;
			if(outputDirectoryName != EMPTYSTRING)
				path = outputDirectoryName;
			else
				path =Sfname.path();
			if(fileInfo_)
			{
				stringstream outputInf;
				outputInf << "_s" << nbSines_;
				outputInf << "i" << intervalFrequency;
				outputInf << "T" << accSize_; 
				outputInf << "c" << nbClusters_;
				outputInf <<"k" << nbSelectedClusters_;
				if(unprecise_)
                  outputInf << "u";
				outputInf << "t" << similarityType_;
					outputInf << "_";
				outputInfo = outputInf.str();
			}

			fileName = path + "/" + Sfname.nameNoExt()+outputInfo+"Sep"+"."+Sfname.ext() ;
			fileResName = path + "/" + Sfname.nameNoExt() +outputInfo+ "Res"+ "." + Sfname.ext() ;
			mixName = path + "/" + Sfname.nameNoExt() +outputInfo+ "Mix" + "."+ Sfname.ext() ;
			filePeakName = path + "/" + Sfname.nameNoExt() + outputInfo+".peak" ;
			fileClustName = path + "/" + Sfname.nameNoExt()+outputInfo+ "Clust" +".txt" ;
			fileVoicingName = path + "/" + Sfname.nameNoExt() +outputInfo+ "Voicing" +".txt" ;
			fileF0Name = path + "/" + Sfname.nameNoExt() +outputInfo+  "F0.txt" ;

			if(Sfname.ext() == "peak")
			{
				analyse_ = 0;
				if(synthetize_ > -1)
				clusterSynthetize_ = synthetize_;
				else
					clusterSynthetize_ = 0;
				fileName = path + "/" + Sfname.nameNoExt()+outputInfo+".wav" ;
	
			}
			if(Sfname.ext() == "wav")
			{
				analyse_ = 1;
			}


			if(noiseName == "music")
			{
				string tmp = Sfname.nameNoExt();
				tmp.replace(tmp.length()-1, 1, 1, 'M');
				noiseName = Sfname.path() +tmp + "." +  Sfname.ext();
			}
			cout << noiseName << endl;

			if(analyse_)
			{
				cout << "Phasevocoding " << Sfname.name() << endl; 
				clusterExtract(peakSet_, *sfi, fileName, noiseName, mixName, intervalFrequency, panningInfo, noiseDelay_, similarityType_, fftSize_, winSize_, hopSize_, nbSines_, nbClusters_, accSize_, synthetize_, &snr0);
			}	
			// if ! peak data read from file
			if(peakSet_.getSize() == 0)
				peakSet_.read(filePeakName);
			if(peakSet_.getSize() == 0)
			{
				cout << "unable to load peak file: " << filePeakName << endl;
				//	exit(1);
			}

			//MATLAB_PUT(peakSet_, "peaks");
			//MATLAB_EVAL("figure(1); clf ; plotPeaks(peaks)");


			// create data for clusters
			PeClusters clusters(peakSet_);
			mrs_natural nbClusters=0;
			// computes the cluster attributes
			if(attributes_)
			{
				realvec vecs;
				realvec conv(2);
				string2parameters(intervalFrequency, conv, '_');
				clusters.attributes(peakSet_, conv(1));	
				clusters.getVecs(vecs);

				// cout << vecs;
				//				MATLAB_PUT(getcwd(NULL, 0), "path");
				//				MATLAB_PUT(fileName, "fileName");

				MATLAB_PUT(vecs, "clusters");
				ofstream clustFile;
				clustFile.open(fileClustName.c_str());
				if(!clustFile)
					cout << "Unable to open output Clusters File " << fileClustName << endl;
				//	clustFile << vecs;
				clustFile.close();

				// store voicingLine
				clusters.voicingLine(fileVoicingName, hopSize_, accSize_);
				clusters.f0Line(fileF0Name, hopSize_, samplingFrequency_, accSize_);
			}

			// compute ground truth 
			if(ground_)
			{
				realvec vecs;
				clusters.synthetize(peakSet_, *sfi, fileName, winSize_, hopSize_, nbSines_, bopt_, 1);
				clusters.getVecs(vecs);
				MATLAB_PUT(vecs, "clusters");
				clusters.selectGround();
				realvec ct;



				clusters.getConversionTable(ct);
				updateLabels(peakSet_, ct);
				//	cout << ct;
			}
			if(ground_ || attributes_)
				MATLAB_EVAL("figure(2) ; plotClusters");
			/*	MATLAB_PUT(peakSet_, "peaksGp");
			MATLAB_EVAL("plotPeaks(peaksGp)");*/

			if(clusterFilteringType_)
			{
				realvec ct;
				clusters.selectBefore(clusterFilteringType_);
				clusters.getConversionTable(ct);
				updateLabels(peakSet_, ct);
			}

			if(clusterSynthetize_ > -1)
			{
				PeClusters sclusters(peakSet_);
				// synthetize remaining clusters
				snr0 = sclusters.synthetize(peakSet_, *sfi, fileName, winSize_, hopSize_, nbSines_, bopt_, clusterSynthetize_);
			}
			/*MATLAB_PUT(peakSet_, "peaks");
			MATLAB_EVAL("plotPeaks(peaks)");*/ 
			/*	MATLAB_PUT(peakSet_, "peaks");
			MATLAB_EVAL("figure(1); clf ; plotPeaks(peaks)");*/

			// for SNR cimputation
			/*
			FileName oriFileName(*sfi);
			FileName noiseFileName(noiseName);
			ofstream resFile;
			string snrName(path + "/similaritySnrResults.txt");
			resFile.open( snrName.c_str(), ios::out | ios::app);
			cout << oriFileName.name() << " " << noiseFileName.name() << " " << similarityType_ << " " << snr0 << endl;
			resFile << oriFileName.name() << " " << noiseFileName.name() << " " << similarityType_ << " " << snr0 << endl;
			resFile.close();
			*/
		}  
	}
	else
	{
		cout << "Using live microphone input" << endl;
		microphone_ = true;
		clusterExtract(peakSet_, "microphone", fileName, noiseName, mixName, intervalFrequency, panningInfo, noiseDelay_, similarityType_, fftSize_, winSize_, hopSize_, nbSines_, nbClusters_, accSize_, synthetize_, &snr0);
	}


	timeElapsed = (clock()-nbTicks)/((mrs_real) CLOCKS_PER_SEC );
	cout << "Time elapsed: " << timeElapsed << endl;

	exit(0);
}


