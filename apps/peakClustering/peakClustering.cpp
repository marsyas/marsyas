#include <cstdio>
#include <iostream>
#include <iomanip>
#include <ctime>
#include <string>

#include "MarSystemManager.h"
#include "AudioSink.h"
#include "SoundFileSink.h"
#include "SoundFileSource.h"
#include "Conversions.h"
#include "CommandLineOptions.h"
#include "PeakFeatureSelect.h"
#include "SimilarityMatrix.h"

//[TODO]
#include "PeUtilities.h"

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
int nbClusters_ = 3;
// nbClusters
int nbSelectedClusters_ = 2;
// output buffer Size
int bopt_ = 128;
// output gain //[WTF]
mrs_real gopt_ = 1.0;
// number of accumulated frames
mrs_natural accSize_ = 10;
// number of seconds for analysing process
mrs_natural stopAnalyse_=0;

// type of similarity Metrics // test amplitude normamlise gtzan //[TODO]
string defaultSimilarityType_ = "hoabfb";//[TODO]
string similarityType_ = EMPTYSTRING;//[TODO]
// weight for similarity Metrics
realvec similarityWeight_;	//[TODO]
// store for clustered peaks 
realvec peakSet_;//[TODO]
// delay for noise insertion

mrs_real noiseDelay_=0;//[TODO]
// gain for noise insertion
mrs_real noiseGain_=.8;//[TODO]
// duration for noise insertion
mrs_real noiseDuration_=0;//[TODO]

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
mrs_natural synthetize_ = 0;
mrs_natural clusterSynthetize_ = -1;
bool peakStore_= true;
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
	cerr << "report bugs to marsyas" << endl;
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
// clusterExtract(peakSet_, *sfi, fileName, noiseName, mixName, intervalFrequency, panningInfo, noiseDelay_, similarityType_, ...
//                fftSize_, winSize_, hopSize_, nbSines_, nbClusters_, accSize_, synthetize_, &snr0);

void
clusterExtract(realvec &peakSet, string sfName, string outsfname, string noiseName, string mixName, string intervalFrequency, string panningInfo, mrs_real noiseDelay, string T, mrs_natural N, mrs_natural Nw, 
							 mrs_natural D, mrs_natural S, mrs_natural C,
							 mrs_natural accSize, mrs_natural synthetize, mrs_real *snr0)
{
	cout << "Extracting Peaks and Clusters" << endl;
	MarSystemManager mng;

	//**************************************************
	// create the peakClustering network
	//**************************************************
	MarSystem* mainNet = mng.create("Series", "mainNet");
	
	//**************************************************************************
	//create accumulator for the texture window and add it to the main network
	//**************************************************************************
	MarSystem* textWinNet = mng.create("Accumulator", "textWinNet");
	mainNet->addMarSystem(textWinNet);

	//************************************************************************
	//create Analysis Network and add it to the texture window accumulator
	//************************************************************************
	MarSystem* analysisNet = mng.create("Series", "analysisNet");
	textWinNet->addMarSystem(analysisNet);

	//************************************************************************
	//create FanInOut for mixing with a noise source and add to Analysis Net
	//************************************************************************
	MarSystem* mixer = mng.create("FanOutIn", "mixer");
	//---- create original series and add it to mixer
	MarSystem* oriNet = mng.create("Series", "oriNet");
	if (microphone_) 
		oriNet->addMarSystem(mng.create("AudioSource", "src"));
	else 
		oriNet->addMarSystem(mng.create("SoundFileSource", "src"));
	oriNet->addMarSystem(mng.create("Gain", "oriGain"));
	mixer->addMarSystem(oriNet);
	//---- create a series for the noiseSource
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
		mixer->addMarSystem(mixseries);
	}
	//add Mixer to analysis network
	analysisNet->addMarSystem(mixer);
	
	//********************************************************
	// create SoundFileSink and add it to the analysis net
	//********************************************************
	if(noiseName != EMPTYSTRING)
		analysisNet->addMarSystem(mng.create("SoundFileSink", "mixSink"));

	//********************************************************
	// create Spectrum Network and add it to the analysis net
	//********************************************************
	MarSystem* spectrumNet = mng.create("Series", "spectrumNet");
	spectrumNet->addMarSystem(mng.create("ShiftInput", "si"));
	spectrumNet->addMarSystem(mng.create("Shifter", "sh"));
	spectrumNet->addMarSystem(mng.create("Windowing", "wi"));
	MarSystem *parallel = mng.create("Parallel", "par");
	parallel->addMarSystem(mng.create("Spectrum", "spk1"));
	parallel->addMarSystem(mng.create("Spectrum", "spk2"));
	spectrumNet->addMarSystem(parallel);
	//add Spectrum net to analysis net 
	analysisNet->addMarSystem(spectrumNet);

	//***************************************************************
	//add PeakConvert to main SERIES for processing texture windows
	//***************************************************************
	mainNet->addMarSystem(mng.create("PeakConvert", "conv"));
	
	
	//***************************************************************
	//create a FlowThru for the Clustering Network and add to main net
	//***************************************************************
	MarSystem* clustNet = mng.create("FlowThru", "clustNet");
	mainNet->addMarSystem(clustNet);
		
	//***************************************************************
	// create Similarities Network and add it to ClustNet
	//***************************************************************
	MarSystem* simNet = mng.create("FanOutIn", "simNet");
	simNet->updctrl("mrs_string/combinator", "*");
	//
	//create Frequency similarity net and add it to simNet
	//
	MarSystem* freqSim = mng.create("Series","freqSim");
	//--------
	freqSim->addMarSystem(mng.create("PeakFeatureSelect","FREQfeatSelect"));
	freqSim->updctrl("PeakFeatureSelect/FREQfeatSelect/mrs_natural/selectedFeatures",
		PeakFeatureSelect::pkFrequency | PeakFeatureSelect::barkPkFreq);
	//--------
	MarSystem* fsimMat = mng.create("SimilarityMatrix","FREQsimMat");
	fsimMat->addMarSystem(mng.create("Metric","FreqL2Norm"));
	fsimMat->updctrl("Metric/FreqL2Norm/mrs_string/metric","euclideanDistance");
	//fsimMat->updctrl("mrs_natural/calcCovMatrix", SimilarityMatrix::diagCovMatrix);
	fsimMat->updctrl("mrs_string/normalize", "MinMax");
	fsimMat->linkctrl("Metric/FreqL2Norm/mrs_realvec/covMatrix", "mrs_realvec/covMatrix");
	freqSim->addMarSystem(fsimMat);	
	//--------
	freqSim->addMarSystem(mng.create("RBF","FREQrbf"));
	freqSim->updctrl("RBF/FREQrbf/mrs_string/RBFtype","Gaussian");
	freqSim->updctrl("RBF/FREQrbf/mrs_bool/symmetricIn",true);
	//--------
	simNet->addMarSystem(freqSim);
	//
	//create Amplitude similarity net and add it to simNet
	//
	MarSystem* ampSim = mng.create("Series","ampSim");
	//--------
	ampSim->addMarSystem(mng.create("PeakFeatureSelect","AMPfeatSelect"));
	ampSim->updctrl("PeakFeatureSelect/AMPfeatSelect/mrs_natural/selectedFeatures",
		PeakFeatureSelect::pkAmplitude | PeakFeatureSelect::dBPkAmp);
	//--------
	MarSystem* asimMat = mng.create("SimilarityMatrix","AMPsimMat");
	asimMat->addMarSystem(mng.create("Metric","AmpL2Norm"));
	asimMat->updctrl("Metric/AmpL2Norm/mrs_string/metric","euclideanDistance");
	//asimMat->updctrl("mrs_natural/calcCovMatrix", SimilarityMatrix::diagCovMatrix);
	asimMat->updctrl("mrs_string/normalize", "MinMax");
	asimMat->linkctrl("Metric/AmpL2Norm/mrs_realvec/covMatrix", "mrs_realvec/covMatrix");
	ampSim->addMarSystem(asimMat);	
	//--------
	ampSim->addMarSystem(mng.create("RBF","AMPrbf"));
	ampSim->updctrl("RBF/AMPrbf/mrs_string/RBFtype","Gaussian");
	ampSim->updctrl("RBF/AMPrbf/mrs_bool/symmetricIn",true);
	//--------
	simNet->addMarSystem(ampSim);
	//
	//create HWPS similarity net and add it to simNet
	//
	MarSystem* HWPSim = mng.create("Series","HWPSim");
	//--------
	HWPSim->addMarSystem(mng.create("PeakFeatureSelect","HWPSfeatSelect"));
	HWPSim->updctrl("PeakFeatureSelect/HWPSfeatSelect/mrs_natural/selectedFeatures",
		PeakFeatureSelect::pkFrequency | PeakFeatureSelect::pkSetFrequencies | PeakFeatureSelect::pkSetAmplitudes);
	//--------
	MarSystem* HWPSsimMat = mng.create("SimilarityMatrix","HWPSsimMat");
	HWPSsimMat->addMarSystem(mng.create("HWPS","hwps"));
	HWPSsimMat->updctrl("HWPS/hwps/mrs_bool/calcDistance", true);
	HWPSim->addMarSystem(HWPSsimMat);	
	//--------
	HWPSim->addMarSystem(mng.create("RBF","HWPSrbf"));
	HWPSim->updctrl("RBF/HWPSrbf/mrs_string/RBFtype","Gaussian");
	HWPSim->updctrl("RBF/HWPSrbf/mrs_bool/symmetricIn",true);
	//--------
	simNet->addMarSystem(HWPSim);
	//
	// LINK controls of PeakFeatureSelects in each similarity branch
	//
	simNet->linkctrl("Series/ampSim/PeakFeatureSelect/AMPfeatSelect/mrs_natural/totalNumPeaks",
		"Series/freqSim/PeakFeatureSelect/FREQfeatSelect/mrs_natural/totalNumPeaks");
	simNet->linkctrl("Series/HWPSim/PeakFeatureSelect/HWPSfeatSelect/mrs_natural/totalNumPeaks",
		"Series/freqSim/PeakFeatureSelect/FREQfeatSelect/mrs_natural/totalNumPeaks");
	//------
	simNet->linkctrl("Series/ampSim/PeakFeatureSelect/AMPfeatSelect/mrs_natural/frameMaxNumPeaks",
		"Series/freqSim/PeakFeatureSelect/FREQfeatSelect/mrs_natural/frameMaxNumPeaks");
	simNet->linkctrl("Series/HWPSim/PeakFeatureSelect/HWPSfeatSelect/mrs_natural/frameMaxNumPeaks",
		"Series/freqSim/PeakFeatureSelect/FREQfeatSelect/mrs_natural/frameMaxNumPeaks");
	//---- create an "alias" link for the above two controls in the simNet
	simNet->linkctrl("mrs_natural/totalNumPeaks",
		"Series/freqSim/PeakFeatureSelect/FREQfeatSelect/mrs_natural/totalNumPeaks");
	simNet->linkctrl("mrs_natural/frameMaxNumPeaks",
		"Series/freqSim/PeakFeatureSelect/FREQfeatSelect/mrs_natural/frameMaxNumPeaks");
	simNet->linkctrl("mrs_natural/totalNumPeaks",
		"Series/ampSim/PeakFeatureSelect/AMPfeatSelect/mrs_natural/totalNumPeaks");
	simNet->linkctrl("mrs_natural/frameMaxNumPeaks",
		"Series/ampSim/PeakFeatureSelect/AMPfeatSelect/mrs_natural/frameMaxNumPeaks");
	simNet->linkctrl("mrs_natural/totalNumPeaks",
		"Series/HWPSim/PeakFeatureSelect/HWPSfeatSelect/mrs_natural/totalNumPeaks");
	simNet->linkctrl("mrs_natural/frameMaxNumPeaks",
		"Series/HWPSim/PeakFeatureSelect/HWPSfeatSelect/mrs_natural/frameMaxNumPeaks");
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//add simNet to clustNet
	clustNet->addMarSystem(simNet);
	//
	// LINK controls related to variable number of peak from PeakConvert to simNet
	//
	mainNet->linkctrl("FlowThru/clustNet/FanOutIn/simNet/mrs_natural/totalNumPeaks",
		"PeakConvert/conv/mrs_natural/totalNumPeaks");
	mainNet->linkctrl("FlowThru/clustNet/FanOutIn/simNet/mrs_natural/frameMaxNumPeaks",
		"PeakConvert/conv/mrs_natural/frameMaxNumPeaks");

	//***************************************************************
	// create NCutNet MarSystem and add it to clustNet
	//***************************************************************
	MarSystem* NCutNet = mng.create("Series","NCutNet");
	clustNet->addMarSystem(NCutNet);
	//---add NCutNet components
	// add a stack to stack the 
	MarSystem* stack = mng.create("Fanout","stack");
	NCutNet->addMarSystem(stack);
	stack->addMarSystem(mng.create("NormCut","NCut"));
	stack->addMarSystem(mng.create("Gain", "ID"));
	// add the cluster selection module
	NCutNet->addMarSystem(mng.create("PeakClusterSelect","clusterSelect"));
	
	//***************************************************************
	// create PeakLabeler MarSystem and add it to mainNet
	//***************************************************************
	MarSystem* labeler = mng.create("PeakLabeler","labeler");
	mainNet->addMarSystem(labeler);
	//---- link labeler label control to the NCut output control
	mainNet->linkctrl("PeakLabeler/labeler/mrs_realvec/peakLabels", "FlowThru/clustNet/mrs_realvec/innerOut");

	//***************************************************************
	// create PeakLabeler MarSystem and add it to mainNet
	//***************************************************************
	if(peakStore_)
	{
 		mainNet->addMarSystem(mng.create("PeakViewSink", "peSink"));
		mainNet->updctrl("PeakViewSink/peSink/mrs_string/filename", filePeakName);
	}

	//****************************************************************
	// Create Synthesis Network
	//****************************************************************
	if(synthetize >-1) 
	{
		//create shredder
		synthNetCreate(&mng, outsfname, microphone_, synthetize, residual_);
		MarSystem *peSynth = mng.create("PeSynthetize", "synthNet");
		mainNet->addMarSystem(peSynth);
	}
	
	
	////////////////////////////////////////////////////////////////
	// update the controls
	////////////////////////////////////////////////////////////////
	mainNet->updctrl("Accumulator/textWinNet/mrs_natural/nTimes", accSize);

	if (microphone_) 
	{
		mainNet->updctrl("mrs_natural/inSamples", D);
		mainNet->updctrl("mrs_natural/inObservations", 1);
	}
	else
	{
		mainNet->updctrl("Accumulator/textWinNet/Series/analysisNet/FanOutIn/mixer/Series/oriNet/SoundFileSource/src/mrs_string/filename", sfName);
		mainNet->updctrl("mrs_natural/inSamples", D);
		mainNet->updctrl("mrs_natural/inObservations", 1);
		samplingFrequency_ = mainNet->getctrl("Accumulator/textWinNet/Series/analysisNet/FanOutIn/mixer/Series/oriNet/SoundFileSource/src/mrs_real/osrate")->to<mrs_real>();
	}

	if(noiseName != EMPTYSTRING)
	{
		mainNet->updctrl("Accumulator/textWinNet/Series/analysisNet/FanOutIn/mixer/Series/mixseries/SoundFileSource/noise/mrs_string/filename", noiseName);
		mainNet->updctrl("mrs_natural/inSamples", D);
		mainNet->updctrl("Accumulator/textWinNet/Series/analysisNet/FanOutIn/mixer/Series/mixseries/NoiseSource/noise/mrs_string/mode", "rand");
		mainNet->updctrl("Accumulator/textWinNet/Series/analysisNet/FanOutIn/mixer/Seriesmixseries/Delay/noiseDelay/mrs_real/delaySeconds",  noiseDelay);
		mainNet->updctrl("Accumulator/textWinNet/Series/analysisNet/FanOutIn/mixer/Series/mixseries/Gain/noiseGain/mrs_real/gain", noiseGain_);
	}


	mainNet->updctrl("Accumulator/textWinNet/Series/analysisNet/Series/spectrumNet/ShiftInput/si/mrs_natural/WindowSize", Nw+1);
	mainNet->updctrl("Accumulator/textWinNet/Series/analysisNet/Series/spectrumNet/Windowing/wi/mrs_natural/size", N);
	mainNet->updctrl("Accumulator/textWinNet/Series/analysisNet/Series/spectrumNet/Windowing/wi/mrs_string/type", "Hanning");
	mainNet->updctrl("Accumulator/textWinNet/Series/analysisNet/Series/spectrumNet/Windowing/wi/mrs_bool/zeroPhasing", true);
	mainNet->updctrl("Accumulator/textWinNet/Series/analysisNet/Series/spectrumNet/Shifter/sh/mrs_natural/shift", 1);
	
	mainNet->updctrl("PeakConvert/conv/mrs_natural/Decimation", D);
	if(unprecise_)
		mainNet->updctrl("PeakConvert/conv/mrs_bool/improvedPrecision", false);      
	else
		mainNet->updctrl("PeakConvert/conv/mrs_bool/improvedPrecision", true);  
	if(noPeakPicking_)
		mainNet->updctrl("PeakConvert/conv/mrs_bool/picking", false);      
	mainNet->updctrl("PeakConvert/conv/mrs_natural/frameMaxNumPeaks", S); 
	mainNet->updctrl("PeakConvert/conv/mrs_string/frequencyInterval", intervalFrequency);  
	// mainNet->updctrl("PeakConvert/conv/mrs_natural/nbFramesSkipped", (N/D));  
	
	mainNet->updctrl("FlowThru/clustNet/Series/NCutNet/Fanout/stack/NormCut/NCut/mrs_natural/numClusters", C); 
	mainNet->updctrl("FlowThru/clustNet/Series/NCutNet/PeakClusterSelect/clusterSelect/mrs_natural/numClustersToKeep", nbSelectedClusters_);
// 	//[TODO]
// 	mainNet->setctrl("PeClust/peClust/mrs_natural/selectedClusters", nbSelectedClusters_); 
// 	mainNet->setctrl("PeClust/peClust/mrs_natural/hopSize", D); 
// 	mainNet->setctrl("PeClust/peClust/mrs_natural/storePeaks", (mrs_natural) peakStore_); 
// 	mainNet->updctrl("PeClust/peClust/mrs_string/similarityType", T); 
//
// 	similarityWeight_.stretch(3);
// 	similarityWeight_(0) = 1;
// 	similarityWeight_(1) = 10;  //[WTF]
// 	similarityWeight_(2) = 1;
// 	mainNet->updctrl("PeClust/peClust/mrs_realvec/similarityWeight", similarityWeight_); 

	if(noiseName != EMPTYSTRING)
		mainNet->updctrl("Accumulator/textWinNet/Series/analysisNet/SoundFileSink/mixSink/mrs_string/filename", mixName);

	mainNet->update();

	if(synthetize>-1)
	{
		//[TODO]
		synthNetConfigure(mainNet, sfName, outsfname, fileResName, panningInfo, 1, Nw, D, S, accSize, microphone_, synthetize_, bopt_, Nw+1-D, residual_);
	}

	if(noiseDuration_) //[WTF]
	{
		ostringstream ossi;
		ossi << ((noiseDelay_+noiseDuration_)) << "s";
		cout << ossi.str() << endl;
		// touch the gain directly
		//	noiseGain->updctrl("0.1s", Repeat("0.1s", 1), new EvValUpd(noiseGain,"mrs_real/gain", 0.0));
	}
	
	//***************************************************************************************************************
	//									MAIN TICKING LOOP
	//***************************************************************************************************************
	//ofstream cfile("density.txt", ios::app); //[WTF] [TODO]
	mrs_real globalSnr = 0;
	mrs_natural frameCount = 0;
	//	mrs_real time=0;

	while(1)
	{	
		mainNet->tick();
		
		if(synthetize > -1 && residual_)
		{
			mrs_real snr = mainNet->getctrl("PeSynthetize/synthNet/Series/postNet/PeakResidual/res/mrs_real/SNR")->to<mrs_real>();
			globalSnr += snr;
			frameCount++;
			// cout << "Frame " << frameCount << " SNR : "<< snr << endl;
		}

		if (!microphone_)
		{
			bool temp = mainNet->getctrl("Accumulator/textWinNet/Series/analysisNet/FanOutIn/mixer/Series/oriNet/SoundFileSource/src/mrs_bool/notEmpty")->to<mrs_bool>();
			bool temp1 = textWinNet->getctrl("Series/analysisNet/FanOutIn/mixer/Series/oriNet/SoundFileSource/src/mrs_bool/notEmpty")->to<mrs_bool>();
			bool temp2 = analysisNet->getctrl("FanOutIn/mixer/Series/oriNet/SoundFileSource/src/mrs_bool/notEmpty")->to<mrs_bool>();

			mrs_real timeRead =  analysisNet->getctrl("FanOutIn/mixer/Series/oriNet/SoundFileSource/src/mrs_natural/pos")->to<mrs_natural>()/samplingFrequency_;
			mrs_real timeLeft;
			if(!stopAnalyse_)
				timeLeft =  analysisNet->getctrl("FanOutIn/mixer/Series/oriNet/SoundFileSource/src/mrs_natural/size")->to<mrs_natural>()/samplingFrequency_;
			else
				timeLeft = stopAnalyse_;
			// string fname = mainNet->getctrl("Accumulator/textWinNet/Series/analysisNet/FanOutIn/mixer/Series/oriNet/SoundFileSource/src/mrs_string/filename")->to<mrs_string>();

			printf("  %.2f / %.2f \r", timeRead, timeLeft);
			fflush(stdout);

			//cout << fixed << setprecision(2) << timeRead << "/" <<  setprecision(2) << timeLeft;
			///*bool*/ temp = mainNet->getctrl("Accumulator/textWinNet/Series/analysisNet/SoundFileSource/src/mrs_bool/notEmpty")->to<mrs_bool>();

			//[TODO]
// 			mrs_real density = mainNet->getctrl("PeClust/peClust/mrs_real/clusterDensity")->to<mrs_real>();
// 			cfile << density << " " << oriGain << endl;
// 			//cout << oriGain << endl;

			if (temp2 == false || (stopAnalyse_ !=0 && stopAnalyse_<timeRead))
				break;
		}
	}
	if(synthetize_ > -1 && residual_)
	{
		cout << "Global SNR : " << globalSnr/frameCount << endl;
		*snr0 = globalSnr/frameCount;
	}

	if(peakStore_)
	{
		mainNet->updctrl("PeakViewSink/peSink/mrs_real/fs", samplingFrequency_);
		mainNet->updctrl("PeakViewSink/peSink/mrs_natural/frameSize", D);
		mainNet->updctrl("PeakViewSink/peSink/mrs_string/filename", filePeakName); 
		mainNet->updctrl("PeakViewSink/peSink/mrs_bool/done", true);
	}

	//cfile.close(); [TODO]
}


void 
initOptions()
{
	cmd_options.addBoolOption("help", "h", false);
	cmd_options.addBoolOption("usage", "us", false);
	cmd_options.addNaturalOption("voices", "v", 1);
	cmd_options.addStringOption("noisename", "N", EMPTYSTRING);
	cmd_options.addStringOption("outputdirectoryname", "o", ".");
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
	MRSDIAG("peakClustering.cpp - main");

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
				analyse_ = false;
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
				cout << "PeakClustering " << Sfname.name() << endl; 
				clusterExtract(peakSet_, *sfi, fileName, noiseName, mixName, intervalFrequency, panningInfo, noiseDelay_, similarityType_, fftSize_, winSize_, hopSize_, nbSines_, nbClusters_, accSize_, synthetize_, &snr0);
			}	
			
			
			
			
			
			
			
// 			
// 			
// 			
// 			// if ! peak data read from file
// 			if(peakSet_.getSize() == 0)
// 				peakSet_.read(filePeakName);
// 			if(peakSet_.getSize() == 0)
// 			{
// 				cout << "unable to load peak file: " << filePeakName << endl;
// 				//	exit(1);
// 			}
// 
// 			//MATLAB_PUT(peakSet_, "peaks");
// 			//MATLAB_EVAL("figure(1); clf ; plotPeaks(peaks)");
// 
// 
// 			// create data for clusters
// 			PeClusters clusters(peakSet_);
// 			mrs_natural nbClusters=0;
// 			// computes the cluster attributes
// 			if(attributes_)
// 			{
// 				realvec vecs;
// 				realvec conv(2);
// 				string2parameters(intervalFrequency, conv, '_');
// 				clusters.attributes(peakSet_, conv(1));	
// 				clusters.getVecs(vecs);
// 
// 				// cout << vecs;
// 				//				MATLAB_PUT(getcwd(NULL, 0), "path");
// 				//				MATLAB_PUT(fileName, "fileName");
// 
// 				MATLAB_PUT(vecs, "clusters");
// 				ofstream clustFile;
// 				clustFile.open(fileClustName.c_str());
// 				if(!clustFile)
// 					cout << "Unable to open output Clusters File " << fileClustName << endl;
// 				//	clustFile << vecs;
// 				clustFile.close();
// 
// 				// store voicingLine
// 				clusters.voicingLine(fileVoicingName, hopSize_, accSize_);
// 				clusters.f0Line(fileF0Name, hopSize_, samplingFrequency_, accSize_);
// 			}
// 
// 			// compute ground truth 
// 			if(ground_)
// 			{
// 				realvec vecs;
// 				clusters.synthetize(peakSet_, *sfi, fileName, winSize_, hopSize_, nbSines_, bopt_, 1);
// 				clusters.getVecs(vecs);
// 				MATLAB_PUT(vecs, "clusters");
// 				clusters.selectGround();
// 				realvec ct;
// 
// 				clusters.getConversionTable(ct);
// 				updateLabels(peakSet_, ct);
// 				//	cout << ct;
// 			}
// 			if(ground_ || attributes_)
// 				MATLAB_EVAL("figure(2) ; plotClusters");
// 			/*	MATLAB_PUT(peakSet_, "peaksGp");
// 			MATLAB_EVAL("plotPeaks(peaksGp)");*/
// 
// 			if(clusterFilteringType_)
// 			{
// 				realvec ct;
// 				clusters.selectBefore(clusterFilteringType_);
// 				clusters.getConversionTable(ct);
// 				updateLabels(peakSet_, ct);
// 			}
// 
// 			if(clusterSynthetize_ > -1)
// 			{
// 				PeClusters sclusters(peakSet_);
// 				// synthetize remaining clusters
// 				snr0 = sclusters.synthetize(peakSet_, *sfi, fileName, winSize_, hopSize_, nbSines_, bopt_, clusterSynthetize_);
// 			}
// 			/*MATLAB_PUT(peakSet_, "peaks");
// 			MATLAB_EVAL("plotPeaks(peaks)");*/ 
// 			/*	MATLAB_PUT(peakSet_, "peaks");
// 			MATLAB_EVAL("figure(1); clf ; plotPeaks(peaks)");*/
// 
// 			// for SNR cimputation
// 			/*
// 			FileName oriFileName(*sfi);
// 			FileName noiseFileName(noiseName);
// 			ofstream resFile;
// 			string snrName(path + "/similaritySnrResults.txt");
// 			resFile.open( snrName.c_str(), ios::out | ios::app);
// 			cout << oriFileName.name() << " " << noiseFileName.name() << " " << similarityType_ << " " << snr0 << endl;
// 			resFile << oriFileName.name() << " " << noiseFileName.name() << " " << similarityType_ << " " << snr0 << endl;
// 			resFile.close();
// 			*/
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


