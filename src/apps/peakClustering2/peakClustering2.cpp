/*
** Copyright (C) 2000-2010 George Tzanetakis <gtzan@cs.uvic.ca>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/


#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <ctime>
#include <string>

#include "common.h"
#include "common_header.h"
#include "MarSystemManager.h"
#include "Conversions.h"
#include "CommandLineOptions.h"
#include "FileName.h"
#include "PeakFeatureSelect.h"
#include "SelfSimilarityMatrix.h"

//[TODO]
//#include "PeUtilities.h"


using namespace std;
using namespace Marsyas;


#define WITH_CLUSTERING

//#define ORIGINAL

#ifdef ORIGINAL
#define USE_FANOUTIN
#else
#define NEWBOOLS
#endif

#ifdef NEWBOOLS
enum SimMeasures_t
{
	kFreq,
	kTime,
	kAmp,
	kDeltaFreq,
	kDeltaAmp,
	kConn,
	kHwps,
	kPan,
	kRandom,

	kNumSimMeasures
};

struct SimMeasureProperties_t
{
	mrs_string	name; 
	mrs_string	desc;
	mrs_string  distance;
	mrs_string  clLong,
				clShort;
	mrs_bool	isIgnored;
	mrs_natural	isHorizontal;
	mrs_natural features;
};
static SimMeasureProperties_t simMeasureProps[kNumSimMeasures] =
{
	{"FREQ",	"Frequency",			"euclideanDistance",	"ignoreFrequency",		"if",	false,	1, {PeakFeatureSelect::pkFrequency | PeakFeatureSelect::barkPkFreq}},
	{"TIME",	"Time",					"euclideanDistance",	"ignoreTime",			"it",	false,	2, {PeakFeatureSelect::pkFrame}},
	{"AMP",		"Amplitude",			"euclideanDistance",	"ignoreAmplitude",		"ia",	false,	1, {PeakFeatureSelect::pkAmplitude | PeakFeatureSelect::dBPkAmp}},
	{"DFREQ",	"Frequency Modulation",	"euclideanDistance",	"ignoreDeltaFrequency", "idf",	false,	0, {PeakFeatureSelect::pkDeltaFrequency | PeakFeatureSelect::barkPkFreq}},
	{"DAMP",	"Amplitude Modulation",	"euclideanDistance",	"ignoreDeltaAmplitude", "ida",	false,	0, {PeakFeatureSelect::pkDeltaAmplitude /*| PeakFeatureSelect::dBPkAmp*/}},
	{"CONN",	"Connectivity",			"doesntmatter",			"ignoreConnectivity",	"ic",	false,	1, {PeakFeatureSelect::pkFrequency | PeakFeatureSelect::barkPkFreq | PeakFeatureSelect::pkFrame}},
	{"HWPS",	"HPWS",					"doesntmatter",			"ignoreHWPS",			"ih",	false,	2, {PeakFeatureSelect::pkFrequency | PeakFeatureSelect::pkSetFrequencies | PeakFeatureSelect::pkSetAmplitudes}},
	{"PAN",		"Panning",				"euclideanDistance",	"ignorePan",			"ip",	false,	2, {PeakFeatureSelect::pkPan}},
	{"RAND",	"Random",				"randomDistance",		"ignoreRand",			"ir",	false,	2, {PeakFeatureSelect::pkPan}},
};
//static mrs_natural simFeatures[kNumSimMeasures] = 
//{
//	{PeakFeatureSelect::pkFrequency | PeakFeatureSelect::barkPkFreq},
//	{PeakFeatureSelect::pkFrame},
//	{PeakFeatureSelect::pkAmplitude | PeakFeatureSelect::dBPkAmp},
//	{PeakFeatureSelect::pkDeltaFrequency | PeakFeatureSelect::barkPkFreq},
//	{PeakFeatureSelect::pkDeltaAmplitude /*| PeakFeatureSelect::dBPkAmp*/},
//	{PeakFeatureSelect::pkFrequency | PeakFeatureSelect::barkPkFreq | PeakFeatureSelect::pkFrame},
//	{PeakFeatureSelect::pkFrequency | PeakFeatureSelect::pkSetFrequencies | PeakFeatureSelect::pkSetAmplitudes},
//	{PeakFeatureSelect::pkPan}
//};

static mrs_natural GetNumSimMeasures ()
{
	mrs_natural count = 0;

	for (mrs_natural i = 0; i < kNumSimMeasures; i++)
		if (!simMeasureProps[i].isIgnored)
			count++;

	return count;
};

static mrs_realvec GetHorizontality ()
{
	mrs_natural numSimMeasures = GetNumSimMeasures ();
	mrs_realvec result (numSimMeasures);
	mrs_natural count = 0;

	for (mrs_natural i = 0; i < kNumSimMeasures; i++)
	{
		if (!simMeasureProps[i].isIgnored)
		{
			result (count)	= (mrs_real)simMeasureProps[i].isHorizontal;
			count++;
		}
	}
	return result;
}

#else
static const int kNumSimMeasures = 8;
bool ignoreFrequency = false;
bool ignoreTime = true;
bool ignoreAmplitude = false;
bool ignoreDeltaFrequency = true;
bool ignoreDeltaAmplitude = true;
bool ignoreConnectivity = true;
bool ignoreHWPS = false;
bool ignorePan = false;
#endif

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
#ifdef ORIGINAL
string intervalFrequency = "250_2500";
#else
string intervalFrequency = "100_4000";
#endif

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
bool residual_ = false;

bool useOnsets = false;
bool horizWeight = false;

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
	cerr << "peakClustering, MARSYAS" << endl;
	cerr << "report bugs to marsyas-users@lists.sourceforge.net" << endl;
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
	cerr << "-v  --voices: number of voices" << endl;
	cerr << "-F --clusterFiltering : cluster filtering" << endl;
	cerr << "-A --attributes : set attributes" << endl;
	cerr << "-g --ground : set ground" << endl;
	cerr << "-SC --clusterSynthetize : cluster synthetize" << endl;
	cerr << "-P --peakStore : set peak store" << endl;
	cerr << "-T --textureSize: number of frames in a texture window" << endl;
	cerr << "-c -clustering : number of clusters in a texture window" << endl;
	cerr << "-k -keep : keep the specified number of clusters in the texture window " << endl;
	cerr << "-S --synthetise : synthetize using an oscillator bank (0), an IFFT mono (1), or an IFFT stereo (2)" << endl;
	cerr << "-r --residual : output the residual sound (if the synthesis stage is selected)" << endl;
	cerr << "-i --intervalFrequency : <minFrequency>_<maxFrequency> select peaks in this interval (default 250-2500 Hz)" << endl;
	cerr << "-f --fileInfo : provide clustering parameters in the output name (s20t10i250_2500c2k1uTabfbho means 20 sines per frames in the 250_2500 Hz frequency Interval, 1 cluster selected among 2 in one texture window of 10 frames, no precise parameter estimation and using a combination of similarities abfbho)" << endl;
	cerr << "-npp --noPeakPicking : do not perform peak picking in the spectrum" << endl;
	cerr << "-u --unprecise : do not perform precise estimation of sinusoidal parameters" << endl;
	
	cerr << "-if --ignoreFrequency: ignore frequency similarity between peaks" << endl;
	cerr << "-it --ignoreTime: ignore time proximity between peaks" << endl;
	cerr << "-ia --ignoreAmplitude: ignore amplitude similarity between peaks" << endl;
	cerr << "-idf --ignoreDeltaFrequency: ignore frequency modulation similarity between peaks" << endl;
	cerr << "-ida --ignoreDeltaAmplitude: ignore amplitude modulation similarity between peaks" << endl;
	cerr << "-ida --ignoreConnectivity: ignore connectivity measure between peaks" << endl;
	cerr << "-ih --ignoreHWPS: ignore harmonicity (HWPS) similarity between peaks" << endl;
	cerr << "-ip --ignorePan: ignore panning similarity between peaks" << endl;
	
	cerr << "-uo --useOnsets: use onset detector for dynamically adjusting the length of texture windows" << endl;
	cerr << "-hw --horizWeight: apply horizontality weighting to features" << endl;

	cerr << "" << endl;
	cerr << "-h --help            : display this information " << endl;
	cerr << "-us --usage : print usage" << endl;

	exit(1);
}

MarSystem* createSimilarityNet (MarSystemManager *mng, mrs_string seriesName = "simNet")
{
	//***************************************************************
	// create Similarities Network and add it to ClustNet
	//***************************************************************
	ostringstream	name,
	tmp;
	MarSystem* simNet = mng->create("Series", seriesName);
	MarSystem* flThru = mng->create("FlowThru", "weightCalc");
	MarSystem* simFan = mng->create("Fanout", "simFan");
	simNet->addMarSystem (flThru);
	simNet->addMarSystem (simFan);

	flThru->addMarSystem (mng->create("PeakFeatureSelect","FREQfeatSel4Weight"));
	flThru->updctrl("PeakFeatureSelect/FREQfeatSel4Weight/mrs_natural/selectedFeatures",
		PeakFeatureSelect::pkFrequency | PeakFeatureSelect::barkPkFreq);
	flThru->addMarSystem (mng->create("PeakDistanceHorizontality","horizontality"));
	
	//
	//create similarity measure calculation and add it to simNet
	//
	for (mrs_natural i = 0; i < kNumSimMeasures; i++)
	{    
		MarSystem	*newSimMeasure	= 0,
					*newSimMat		= 0;

		// create series
		name.str("");
		name << simMeasureProps[i].name << "_" << "Sim";
		newSimMeasure = mng->create("Series", name.str ());

		// create feature selection and set params
		tmp.str("");
		tmp << simMeasureProps[i].name << "_" << "FeatSelect";
		newSimMeasure->addMarSystem(mng->create("PeakFeatureSelect", tmp.str ()));
		tmp.str("");
		tmp << "PeakFeatureSelect/" << simMeasureProps[i].name << "_" << "FeatSelect" <<"/mrs_natural/selectedFeatures";
		newSimMeasure->updctrl(tmp.str (), simMeasureProps[i].features);

		// create similarity matrix and set metric and parameters
		tmp.str("");
		tmp << simMeasureProps[i].name << "_" << "SimMat";
		switch (i)
		{
		default:
			{
				newSimMat = mng->create("SelfSimilarityMatrix", tmp.str ());
				tmp.str("");
				tmp << simMeasureProps[i].name << "_" << "L2Norm";
				newSimMat->addMarSystem(mng->create("Metric", tmp.str ()));
				tmp.str("");
				tmp << "Metric/" << simMeasureProps[i].name << "_" << "L2Norm" << "/mrs_string/metric","euclideanDistance";
				newSimMat->updctrl(tmp.str (),simMeasureProps[i].distance);
				newSimMat->updctrl("mrs_natural/calcCovMatrix", SelfSimilarityMatrix::diagCovMatrix);
				break;
			}
		case kConn:
			{
				newSimMat = mng->create("TimeFreqPeakConnectivity", tmp.str ());
				newSimMat->updctrl("mrs_string/frequencyInterval", intervalFrequency);
				break;
			}
		case kHwps:
			{
				newSimMat = mng->create("SelfSimilarityMatrix", tmp.str ());
				tmp.str("");
				tmp << simMeasureProps[i].name << "_" << "Norm";
				newSimMat->addMarSystem(mng->create("HWPS", tmp.str ()));
				tmp.str("");
				tmp << "HWPS/" << simMeasureProps[i].name << "_" << "Norm" << "/mrs_bool/calcDistance";
				newSimMat->updctrl(tmp.str (), true);
				break;
			}
		}
		// add created system to hierarchy
		newSimMeasure->addMarSystem(newSimMat);	
		simFan->addMarSystem(newSimMeasure);
	}

	// weighting for different similarity measures
	simNet->addMarSystem(mng->create("ParallelMatrixWeight","SimWeight"));

	//
	//	map each distance matrix with an RBF in a parallel
	//
	MarSystem* prbfParallel	= mng->create("Parallel","RBFParallel");

	// create RBF weighting and set RBF parameters
	for (mrs_natural i = 0; i < kNumSimMeasures; i++)
	{
		name.str("");
		name << simMeasureProps[i].name << "_" << "Rbf";
		prbfParallel->addMarSystem(mng->create("RBF", name.str ()));

		tmp.str("");
		tmp << "RBF/" << name.str () << "/mrs_string/RBFtype";
		prbfParallel->updctrl(tmp.str (), "Gaussian");
		tmp.str("");
		tmp << "RBF/" << name.str () << "/mrs_bool/symmetricIn";
		prbfParallel->updctrl(tmp.str (), true);
	}


	simNet->addMarSystem(prbfParallel);

	simNet->addMarSystem(mng->create("Combinator","CombSims"));
	simNet->updctrl("Combinator/CombSims/mrs_string/combinator", "*");


	simNet->linkctrl("ParallelMatrixWeight/SimWeight/mrs_realvec/weights",
		"FlowThru/weightCalc/PeakDistanceHorizontality/horizontality/mrs_realvec/weights");

	//
	// LINK controls of PeakFeatureSelects in each similarity branch
	//
	simNet->linkctrl("Parallel/RBFParallel/mrs_natural/inObservations",
		"Fanout/simFan/mrs_natural/onObservations");

	name.str("");
	name   << "Fanout/simFan/Series/" << simMeasureProps[0].name << "_" << "Sim/SelfSimilarityMatrix/" << simMeasureProps[0].name << "_SimMat/mrs_natural/onObservations";	 // master
	simNet->linkctrl("Parallel/RBFParallel/mrs_natural/inSamples",
		name.str ());
	//------ 
	name.str("");
	name   << "Fanout/simFan/Series/" << simMeasureProps[0].name << "_" << "Sim/PeakFeatureSelect/" << simMeasureProps[0].name << "_FeatSelect/mrs_natural/totalNumPeaks";	 // master
	simNet->linkctrl("FlowThru/weightCalc/PeakFeatureSelect/FREQfeatSel4Weight/mrs_natural/totalNumPeaks",
		name.str ());
	//------
	name.str("");
	name   << "Fanout/simFan/Series/" << simMeasureProps[0].name << "_" << "Sim/PeakFeatureSelect/" << simMeasureProps[0].name << "_FeatSelect/mrs_natural/frameMaxNumPeaks";	 // master
	simNet->linkctrl("FlowThru/weightCalc/PeakFeatureSelect/FREQfeatSel4Weight/mrs_natural/frameMaxNumPeaks", name.str ());

	// link all similarity measure controls to the first one
	for (mrs_natural i = 1; i < kNumSimMeasures; i++)
	{
		name.str("");
		name   << "Series/" << simMeasureProps[0].name << "_" << "Sim/PeakFeatureSelect/" << simMeasureProps[0].name << "_" << "FeatSelect/mrs_natural/totalNumPeaks";	 // master
		tmp.str("");
		tmp   << "Series/" << simMeasureProps[i].name << "_" << "Sim/PeakFeatureSelect/" << simMeasureProps[i].name << "_" << "FeatSelect/mrs_natural/totalNumPeaks";	// slave
		simFan->linkctrl(tmp.str (), name.str ());

		name.str("");
		name   << "Series/" << simMeasureProps[0].name << "_" << "Sim/PeakFeatureSelect/" << simMeasureProps[0].name << "_" << "FeatSelect/mrs_natural/frameMaxNumPeaks";	 // master
		//name   << "Series/freqSim/PeakFeatureSelect/" << simMeasureProps[0].name << "_" << "FeatSelect" << "/mrs_natural/frameMaxNumPeaks";	 // master
		tmp.str("");
		tmp   << "Series/" << simMeasureProps[i].name << "_" << "Sim/PeakFeatureSelect/" << simMeasureProps[i].name << "_" << "FeatSelect/mrs_natural/frameMaxNumPeaks";	// slave
		simFan->linkctrl(tmp.str (), name.str ());
	}

	return simNet;
}
// original monophonic peakClustering 
// peakClustering(peakSet_, *sfi, fileName, noiseName, mixName, intervalFrequency, panningInfo, noiseDelay_, similarityType_, ...
//                fftSize_, winSize_, hopSize_, nbSines_, nbClusters_, accSize_, synthetize_, &snr0);

void
peakClustering(realvec &peakSet, string sfName, string outsfname, string noiseName, string mixName, string intervalFrequency, string panningInfo, mrs_real noiseDelay, string T, mrs_natural N, mrs_natural Nw, 
			   mrs_natural D, mrs_natural S, mrs_natural C,
			   mrs_natural accSize, mrs_natural synthetize, mrs_real *snr0)
{
	MarSystemManager mng;

	cout << "Extracting Peaks and Computing Clusters..." << endl;
	cout << "Nw = " << Nw << endl;
	cout << "N = " << N << endl;
	cout << "winSize_ = " << winSize_ << endl;
	cout << "D = " << D << endl;
	cout << "hopSize_ = " << hopSize_ << endl;
	
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

	
	//***********************************************************
	// create peakExtract network and add it to the analysis net
	//***********************************************************
	MarSystem* peakExtract = mng.create("Series","peakExtract");
	peakExtract->addMarSystem(mng.create("ShiftInput", "si"));

	MarSystem* stereoFo = mng.create("Fanout","stereoFo");
	// create Spectrum Network and add it to the analysis net
	MarSystem* spectrumNet = mng.create("Series", "spectrumNet");
	spectrumNet->addMarSystem(mng.create("Stereo2Mono","s2m"));

	//onset detector
	MarSystem* onsetdetector = mng.create("FlowThru", "onsetdetector");
	//onsetdetector->addMarSystem(mng.create("ShiftInput", "si"));
	onsetdetector->addMarSystem(mng.create("Windowing", "win")); 
	onsetdetector->addMarSystem(mng.create("Spectrum","spk"));
	onsetdetector->addMarSystem(mng.create("PowerSpectrum", "pspk"));
	onsetdetector->addMarSystem(mng.create("Flux", "flux")); 
	onsetdetector->addMarSystem(mng.create("ShiftInput","sif"));
	onsetdetector->addMarSystem(mng.create("Filter","filt1"));
	onsetdetector->addMarSystem(mng.create("Reverse","rev1"));
	onsetdetector->addMarSystem(mng.create("Filter","filt2"));
	onsetdetector->addMarSystem(mng.create("Reverse","rev2"));
	onsetdetector->addMarSystem(mng.create("PeakerOnset","peaker")); 
	spectrumNet->addMarSystem(onsetdetector);
		
	spectrumNet->addMarSystem(mng.create("Shifter", "sh"));
	spectrumNet->addMarSystem(mng.create("Windowing", "wi"));
	MarSystem* parallel = mng.create("Parallel", "par");
	parallel->addMarSystem(mng.create("Spectrum", "spk1"));
	parallel->addMarSystem(mng.create("Spectrum", "spk2"));
	spectrumNet->addMarSystem(parallel);
	// add spectrumNet to stereo fanout
	stereoFo->addMarSystem(spectrumNet);
	//
	//create stereo spectrum net
	MarSystem* stereoSpkNet = mng.create("Series","stereoSpkNet");
	MarSystem* LRnet = mng.create("Parallel","LRnet");
	//
	MarSystem* spkL = mng.create("Series","spkL");
	spkL->addMarSystem(mng.create("Windowing","win"));
	spkL->addMarSystem(mng.create("Spectrum","spk"));
	LRnet->addMarSystem(spkL);
	//
	MarSystem* spkR = mng.create("Series","spkR");
	spkR->addMarSystem(mng.create("Windowing","win"));
	spkR->addMarSystem(mng.create("Spectrum","spk"));
	LRnet->addMarSystem(spkR);
	//
	//add it to the stereo spectrum net series
	stereoSpkNet->addMarSystem(LRnet);
	//
	//add stereo spectrum object to stereo spectrum net
	//stereoSpkNet->addMarSystem(mng.create("StereoSpectrum","stereoSpk")); //AVENDANO
	stereoSpkNet->addMarSystem(mng.create("EnhADRess","ADRess"));//enhADRess_1
	stereoSpkNet->addMarSystem(mng.create("EnhADRessStereoSpectrum","stereoSpk")); //enhADRess_2
	//
	// add the stereo Spectrum net to the Fanout
	stereoFo->addMarSystem(stereoSpkNet);
	//
	// add the fanout to the peakExtract net
	peakExtract->addMarSystem(stereoFo);
	//
	//add peakExtract net to analysis net 
	analysisNet->addMarSystem(peakExtract);

	//***************************************************************
	//add PeakConvert to main SERIES for processing texture windows
	//***************************************************************
	mainNet->addMarSystem(mng.create("PeakConvert2", "conv"));

	//***************************************************************
	//create a FlowThru for the Clustering Network and add to main net
	//***************************************************************
	MarSystem* clustNet = mng.create("FlowThru", "clustNet");
#ifdef WITH_CLUSTERING
	mainNet->addMarSystem(clustNet);
#endif

	//***************************************************************
	// create Similarities Network and add it to ClustNet
	//***************************************************************
	MarSystem* simNet;
	clustNet->addMarSystem(simNet = createSimilarityNet (&mng));
	//
	// LINK controls related to variable number of peak from PeakConvert to simNet
	//
	ostringstream ctrl;
	ctrl.str("");
	ctrl << "FlowThru/clustNet/Series/simNet/Fanout/simFan/Series/" << simMeasureProps[0].name << "_" << "Sim/PeakFeatureSelect/" << simMeasureProps[0].name << "_FeatSelect/mrs_natural/totalNumPeaks";
	mainNet->linkctrl(ctrl.str (), "PeakConvert2/conv/mrs_natural/totalNumPeaks");
	ctrl.str("");
	ctrl << "FlowThru/clustNet/Series/simNet/Fanout/simFan/Series/" << simMeasureProps[0].name << "_" << "Sim/PeakFeatureSelect/" << simMeasureProps[0].name << "_FeatSelect/mrs_natural/frameMaxNumPeaks";
	mainNet->linkctrl(ctrl.str (), "PeakConvert2/conv/mrs_natural/frameMaxNumPeaks");


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
	// create PeakViewSink MarSystem and add it to mainNet
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
		//create Shredder series
		MarSystem* postNet = mng.create("Series", "postNet");
		//	postNet->addMarSystem(mng->create("PeOverlapadd", "ob"));
		if (synthetize < 3)
		{
			if(synthetize == 0)
			{
				postNet->addMarSystem(mng.create("PeakSynthOsc", "pso"));
				postNet->addMarSystem(mng.create("Windowing", "wiSyn"));
			}
			else
			{
				// put a fake object for probing the series
				postNet->addMarSystem(mng.create("Gain", "fakeGain"));
				postNet->addMarSystem(mng.create("FlowCutSource", "fcs"));
				// put the original source
				if (microphone_) 
					postNet->addMarSystem(mng.create("AudioSource", "srcSyn"));
				else 
					postNet->addMarSystem(mng.create("SoundFileSource", "srcSyn"));
				// set the correct buffer size
				postNet->addMarSystem(mng.create("ShiftInput", "siSyn"));
				// perform an FFT
				postNet->addMarSystem(mng.create("Spectrum", "specSyn"));
				// convert to polar
				postNet->addMarSystem(mng.create("Cartesian2Polar", "c2p"));
				// perform amplitude and panning change
				postNet->addMarSystem(mng.create("PeakSynthFFT", "psf"));
				// convert back to cartesian
				postNet->addMarSystem(mng.create("Polar2Cartesian", "p2c"));	
				// perform an IFFT
				//	 postNet->addMarSystem(mng.create("PlotSink", "plot"));
				postNet->addMarSystem(mng.create("InvSpectrum", "invSpecSyn"));
				// postNet->addMarSystem(mng.create("PlotSink", "plot2"));
				postNet->addMarSystem(mng.create("Windowing", "wiSyn"));
			}
			postNet->addMarSystem(mng.create("OverlapAdd", "ov"));
		}
		else
		{
			postNet->addMarSystem(mng.create("PeakSynthOscBank", "pso"));
			// postNet->addMarSystem(mng.create("ShiftOutput", "so"));
		}

		postNet->addMarSystem(mng.create("Gain", "outGain"));

		MarSystem *dest;
		if (outsfname == "MARSYAS_EMPTY") 
			dest = mng.create("AudioSink/dest");
		else
		{
			dest = mng.create("SoundFileSink/dest");
			//dest->updctrl("mrs_string/filename", outsfname);
		}

		if(residual_)
		{
			MarSystem* fanout = mng.create("Fanout", "fano");
			fanout->addMarSystem(dest);
			MarSystem* fanSeries = mng.create("Series", "fanSeries");

			if (microphone_) 
				fanSeries->addMarSystem(mng.create("AudioSource", "src2"));
			else 
				fanSeries->addMarSystem(mng.create("SoundFileSource", "src2"));

			fanSeries->addMarSystem(mng.create("Delay", "delay"));
			fanout->addMarSystem(fanSeries);

			postNet->addMarSystem(fanout);
			postNet->addMarSystem(mng.create("PeakResidual", "res"));

			MarSystem *destRes;
			if (outsfname == "MARSYAS_EMPTY") 
				destRes = mng.create("AudioSink/destRes");
			else
			{
				destRes = mng.create("SoundFileSink/destRes");
				//dest->updctrl("mrs_string/filename", outsfname);
			}
			postNet->addMarSystem(destRes);
		}
		else
			postNet->addMarSystem(dest);

		MarSystem* synthNet = mng.create("Shredder", "synthNet");
		synthNet->addMarSystem(postNet);
		
		mainNet->addMarSystem(synthNet);

		//link Shredder nTimes to Accumulator nTimes
		mainNet->linkctrl("Shredder/synthNet/mrs_natural/nTimes",
						  "Accumulator/textWinNet/mrs_natural/nTimes");
	}

	
	//****************************************************************

	
	////////////////////////////////////////////////////////////////
	// update the controls
	////////////////////////////////////////////////////////////////
	//mainNet->updctrl("Accumulator/textWinNet/mrs_natural/nTimes", accSize);

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

	mainNet->updctrl("Accumulator/textWinNet/Series/analysisNet/Series/peakExtract/ShiftInput/si/mrs_natural/winSize", Nw+1);

	mainNet->updctrl("Accumulator/textWinNet/Series/analysisNet/Series/peakExtract/Fanout/stereoFo/Series/spectrumNet/Shifter/sh/mrs_natural/shift", 1);

	mainNet->updctrl("Accumulator/textWinNet/Series/analysisNet/Series/peakExtract/Fanout/stereoFo/Series/spectrumNet/Windowing/wi/mrs_natural/size", N);
	mainNet->updctrl("Accumulator/textWinNet/Series/analysisNet/Series/peakExtract/Fanout/stereoFo/Series/spectrumNet/Windowing/wi/mrs_string/type", "Hanning");
	mainNet->updctrl("Accumulator/textWinNet/Series/analysisNet/Series/peakExtract/Fanout/stereoFo/Series/spectrumNet/Windowing/wi/mrs_bool/zeroPhasing", true);

	mainNet->updctrl("Accumulator/textWinNet/Series/analysisNet/Series/peakExtract/Fanout/stereoFo/Series/stereoSpkNet/Parallel/LRnet/Series/spkL/Windowing/win/mrs_natural/size", N);
	mainNet->updctrl("Accumulator/textWinNet/Series/analysisNet/Series/peakExtract/Fanout/stereoFo/Series/stereoSpkNet/Parallel/LRnet/Series/spkL/Windowing/win/mrs_string/type", "Hanning");
	mainNet->updctrl("Accumulator/textWinNet/Series/analysisNet/Series/peakExtract/Fanout/stereoFo/Series/stereoSpkNet/Parallel/LRnet/Series/spkL/Windowing/win/mrs_bool/zeroPhasing", true);
	//
	mainNet->updctrl("Accumulator/textWinNet/Series/analysisNet/Series/peakExtract/Fanout/stereoFo/Series/stereoSpkNet/Parallel/LRnet/Series/spkR/Windowing/win/mrs_natural/size", N);
	mainNet->updctrl("Accumulator/textWinNet/Series/analysisNet/Series/peakExtract/Fanout/stereoFo/Series/stereoSpkNet/Parallel/LRnet/Series/spkR/Windowing/win/mrs_string/type", "Hanning");
	mainNet->updctrl("Accumulator/textWinNet/Series/analysisNet/Series/peakExtract/Fanout/stereoFo/Series/stereoSpkNet/Parallel/LRnet/Series/spkR/Windowing/win/mrs_bool/zeroPhasing", true);

	if(unprecise_)
		mainNet->updctrl("PeakConvert2/conv/mrs_bool/improvedPrecision", false);      
	else
		mainNet->updctrl("PeakConvert2/conv/mrs_bool/improvedPrecision", true);  
	
	if(noPeakPicking_)
		mainNet->updctrl("PeakConvert2/conv/mrs_bool/picking", false);      

	mainNet->updctrl("PeakConvert2/conv/mrs_natural/hopSize", D);

	mainNet->updctrl("PeakConvert2/conv/mrs_natural/frameMaxNumPeaks", S); 
	mainNet->updctrl("PeakConvert2/conv/mrs_string/frequencyInterval", intervalFrequency);  
	mainNet->updctrl("PeakConvert2/conv/mrs_natural/nbFramesSkipped", 0);//(N/D));  

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

	//check if input is a stereo signal
	if(mainNet->getctrl("Accumulator/textWinNet/Series/analysisNet/FanOutIn/mixer/mrs_natural/onObservations")->to<mrs_natural>() == 1)
	{
		//if a not a stereo signal, we must set the Stereo2Mono weight to 1.0 (i.e. do no mixing)!
		mainNet->updctrl("Accumulator/textWinNet/Series/analysisNet/Series/peakExtract/Fanout/stereoFo/Series/spectrumNet/Stereo2Mono/s2m/mrs_real/weight", 1.0);
	}

	//------------------------------------------------------------------------
	//check which similarity computations should be disabled (if any)
	//------------------------------------------------------------------------
	// Frequency Similarity

	simMeasureProps[kPan].isIgnored	= simMeasureProps[kPan].isIgnored || !(mainNet->getctrl("Accumulator/textWinNet/Series/analysisNet/FanOutIn/mixer/mrs_natural/onObservations")->to<mrs_natural>() == 2);
	for (mrs_natural i = 0; i < kNumSimMeasures; i++)
	{
		ostringstream string;
		if (simMeasureProps[i].isIgnored)
		{
			cout << simMeasureProps[i].desc << " Similarity Computation disabled!" << endl;

			string.str ("");
			string << "Series/" << simMeasureProps[i].name << "_Sim";
			mainNet->updctrl("FlowThru/clustNet/Series/simNet/Fanout/simFan/mrs_string/disableChild",
				string.str());
			string.str ("");
			string << "Parallel/RBFParallel/RBF/" << simMeasureProps[i].name << "_Rbf/mrs_natural/inObservations";
			simNet->updctrl(string.str(), 0);

			if (i == kPan)
			{
				mainNet->updctrl("Accumulator/textWinNet/Series/analysisNet/Series/peakExtract/Fanout/stereoFo/mrs_string/disableChild",
					"Series/stereoSpkNet");
			}
		}
		else
		{
			ostringstream tmp;
			cout << simMeasureProps[i].desc << " Similarity Computation enabled!" << endl;

			string.str ("");
			string << "Series/" << simMeasureProps[i].name << "_Sim";
			mainNet->updctrl("FlowThru/clustNet/Series/simNet/Fanout/simFan/mrs_string/enableChild",
				string.str());

			string.str ("");
			string << "Parallel/RBFParallel/RBF/" << simMeasureProps[i].name << "_Rbf/mrs_natural/inObservations";
			tmp.str("");
#ifndef GEORGE_DBGVERSION
			tmp << "Fanout/simFan/Series/" << simMeasureProps[0].name << "_Sim/SelfSimilarityMatrix/" << simMeasureProps[0].name  << "_SimMat/mrs_natural/onObservations";
#else
			tmp << "Fanout/simFan/Series/" << simMeasureProps[i].name << "_Sim/";
			if (i != kConn)
				tmp << "SelfSimilarityMatrix/" << simMeasureProps[i].name;
			else
				tmp << "TimeFreqPeakConnectivity/" << simMeasureProps[i].name;
			tmp << "_SimMat/mrs_natural/onObservations";

#endif
			simNet->linkctrl(string.str(), tmp.str());

			if (i == kPan)
			{
				mainNet->updctrl("Accumulator/textWinNet/Series/analysisNet/Series/peakExtract/Fanout/stereoFo/mrs_string/enableChild",
					"Series/stereoSpkNet");
			}
		}
	}
	simNet->updctrl("Combinator/CombSims/mrs_natural/numInputs", GetNumSimMeasures ());
	simNet->updctrl("FlowThru/weightCalc/PeakDistanceHorizontality/horizontality/mrs_natural/numInputs", GetNumSimMeasures ());
	simNet->updctrl("FlowThru/weightCalc/PeakDistanceHorizontality/horizontality/mrs_realvec/inpIsHorizontal", GetHorizontality ());


	simNet->updctrl("FlowThru/weightCalc/PeakDistanceHorizontality/horizontality/mrs_bool/bypass", !horizWeight);
	//
	mainNet->update(); //probably not necessary... [!]
	//------------------------------------------------------------------------

	if(noiseDuration_) //[WTF]
	{
		ostringstream ossi;
		ossi << ((noiseDelay_+noiseDuration_)) << "s";
		cout << ossi.str() << endl;
		// touch the gain directly
		//	noiseGain->updctrl("0.1s", Repeat("0.1s", 1), new EvValUpd(noiseGain,"mrs_real/gain", 0.0));
	}

	//ONSET DETECTION CONFIGURATION (if enabled)
	if(useOnsets)
	{
		cout << "** Onset detector enabled -> using dynamically adjusted texture windows!" << endl;
		cout << "WinSize = " << winSize_ << endl;
		cout << "N = " << N << endl;
		cout << "Nw = " << Nw << endl;
		cout << "hopSize = " << hopSize_ << endl;
		cout << "D = " << D << endl;
		cout << "fs = " << samplingFrequency_ << endl;

		//link controls for onset detector
		onsetdetector->linkctrl("Filter/filt2/mrs_realvec/ncoeffs",
								"Filter/filt1/mrs_realvec/ncoeffs");
		onsetdetector->linkctrl("Filter/filt2/mrs_realvec/dcoeffs",
								"Filter/filt1/mrs_realvec/dcoeffs");
		//link onset detector to accumulator and if onsets enabled, set "explicitFlush" mode
		textWinNet->linkControl("mrs_bool/flush",
								"Series/analysisNet/Series/peakExtract/Fanout/stereoFo/Series/spectrumNet/FlowThru/onsetdetector/PeakerOnset/peaker/mrs_bool/onsetDetected");

		//update onset detector controls
		onsetdetector->updctrl("PowerSpectrum/pspk/mrs_string/spectrumType", "wrongdBonsets");
		onsetdetector->updctrl("Flux/flux/mrs_string/mode", "DixonDAFX06");
		realvec bcoeffs(1,3);//configure zero-phase Butterworth filter of Flux time series -> butter(2, 0.28)
		bcoeffs(0) = 0.1174;
		bcoeffs(1) = 0.2347;
		bcoeffs(2) = 0.1174;
		realvec acoeffs(1,3);
		acoeffs(0) = 1.0;
		acoeffs(1) = -0.8252;
		acoeffs(2) = 0.2946;
		onsetdetector->updctrl("Filter/filt1/mrs_realvec/ncoeffs", bcoeffs);
		onsetdetector->updctrl("Filter/filt1/mrs_realvec/dcoeffs", acoeffs);
		mrs_natural lookAheadSamples = 6;
		onsetdetector->updctrl("PeakerOnset/peaker/mrs_natural/lookAheadSamples", lookAheadSamples); //!!
		onsetdetector->updctrl("PeakerOnset/peaker/mrs_real/threshold", 1.5); //!!!
		onsetdetector->updctrl("ShiftInput/sif/mrs_natural/winSize", 4*lookAheadSamples+1);

		//set Accumulator controls for explicit flush mode
		mrs_natural winds = 1+lookAheadSamples+mrs_natural(ceil(mrs_real(winSize_)/hopSize_/2.0));
		cout << "Accumulator/textWinNet timesToKeep = " << winds << endl;
		mrs_real textureWinMinLen = 0.050; //secs
		mrs_real textureWinMaxLen = 1.0; //secs
		mrs_natural minTimes = (mrs_natural)(textureWinMinLen*samplingFrequency_/hopSize_); 
		mrs_natural maxTimes = (mrs_natural)(textureWinMaxLen*samplingFrequency_/hopSize_); 
		cout << "Accumulator/textWinNet MinTimes = " << minTimes << " (i.e. " << textureWinMinLen << " secs)" << endl;
		cout << "Accumulator/textWinNet MaxTimes = " << maxTimes << " (i.e. " << textureWinMaxLen << " secs)" <<endl;
		textWinNet->updctrl("mrs_string/mode", "explicitFlush");
		textWinNet->updctrl("mrs_natural/timesToKeep", winds);
		textWinNet->updctrl("mrs_string/mode","explicitFlush");
		textWinNet->updctrl("mrs_natural/maxTimes", maxTimes); 
		textWinNet->updctrl("mrs_natural/minTimes", minTimes);
	}
	else
	{
		cout << "** Onset detector disabled -> using fixed length texture windows" << endl;
		cout << "Accumulator/textWinNet nTimes = " << accSize << " (i.e. " << accSize*hopSize_/samplingFrequency_ << " secs)" << endl;
		mainNet->updctrl("Accumulator/textWinNet/mrs_natural/nTimes", accSize);
	}

	if(synthetize>-1)
	{
		mrs_natural delay = -(winSize_/2 - hopSize_); 
		cout << "Delay = " << delay << endl;
		if (synthetize < 3)
		{
			if(synthetize==0)
			{
				mainNet->updctrl("Shredder/synthNet/Series/postNet/PeakSynthOsc/pso/mrs_real/samplingFreq", samplingFrequency_);
				mainNet->updctrl("Shredder/synthNet/Series/postNet/PeakSynthOsc/pso/mrs_natural/delay", delay); // Nw/2+1 
				mainNet->updctrl("Shredder/synthNet/Series/postNet/PeakSynthOsc/pso/mrs_natural/synSize", hopSize_*2);//D*2);
				mainNet->updctrl("Shredder/synthNet/Series/postNet/Windowing/wiSyn/mrs_string/type", "Hanning");
			}
			else 
			{
				// linking between the first slice and the psf
				mainNet->linkControl("Shredder/synthNet/Series/postNet/mrs_realvec/input0", "PeSynthetize/synthNet/Series/postNet/PeakSynthFFT/psf/mrs_realvec/peaks");
				//
				mainNet->updctrl("Shredder/synthNet/Series/postNet/Windowing/wiSyn/mrs_string/type", "Hanning");
				mainNet->updctrl("Shredder/synthNet/Series/postNet/FlowCutSource/fcs/mrs_natural/setSamples", D);	
				mainNet->updctrl("Shredder/synthNet/Series/postNet/FlowCutSource/fcs/mrs_natural/setObservations", 1);	
				// setting the panning mode mono/stereo
				mainNet->updctrl("Shredder/synthNet/Series/postNet/PeakSynthFFT/psf/mrs_natural/nbChannels", synthetize_);
				mainNet->updctrl("Shredder/synthNet/Series/postNet/PeakSynthFFT/psf/mrs_string/panning", panningInfo);
				// setting the FFT size
				mainNet->updctrl("Shredder/synthNet/Series/postNet/ShiftInput/siSyn/mrs_natural/winSize", D*2);
				// setting the name of the original file
				if (microphone_) 
				{
					mainNet->updctrl("Shredder/synthNet/Series/postNet/AudioSource/srcSyn/mrs_natural/inSamples", D);
					mainNet->updctrl("Shredder/synthNet/Series/postNet/AudioSource/srcSyn/mrs_natural/inObservations", 1);
				}
				else
				{
					mainNet->updctrl("Shredder/synthNet/Series/postNet/SoundFileSource/srcSyn/mrs_string/filename", sfName);
					// pvseries->updctrl("Shredder/synthNet/Series/postNet/SoundFileSource/srcSyn/mrs_natural/pos", 0);
					mainNet->updctrl("Shredder/synthNet/Series/postNet/SoundFileSource/srcSyn/mrs_natural/onSamples", D);
					mainNet->updctrl("Shredder/synthNet/Series/postNet/SoundFileSource/srcSyn/mrs_natural/onObservations", 1);
				}
				// setting the synthesis starting time (default 0)
			}
		}
		//else
		//	mainNet->updctrl("Shredder/synthNet/Series/postNet/PeakSynthOscBank/pso/mrs_natural/Interpolation", D); //this control exists?!? [WTF]

		//mainNet->updctrl("Shredder/synthNet/Series/postNet/ShiftOutput/so/mrs_natural/Interpolation", D); //[WTF]

		if (outsfname == "MARSYAS_EMPTY") 
			mainNet->updctrl("Shredder/synthNet/Series/postNet/AudioSink/dest/mrs_natural/bufferSize", bopt_);

		if(residual_)
		{
			mainNet->updctrl("Shredder/synthNet/Series/postNet/Fanout/fano/Series/fanSeries/Delay/delay/mrs_natural/delay", delay); // Nw+1-D

			if (microphone_) 
			{
				mainNet->updctrl("PeSynthetize/synthNet/Series/postNet/Fanout/fano/Series/fanSeries/AudioSource/src2/mrs_natural/inSamples", D);
				mainNet->updctrl("PeSynthetize/synthNet/Series/postNet/Fanout/fano/Series/fanSeries/AudioSource/src2/mrs_natural/inObservations", 1);
			}
			else
			{
				mainNet->updctrl("Shredder/synthNet/Series/postNet/Fanout/fano/Series/fanSeries/SoundFileSource/src2/mrs_string/filename", sfName);
				mainNet->updctrl("Shredder/synthNet/Series/postNet/Fanout/fano/Series/fanSeries/SoundFileSource/src2/mrs_natural/pos", 0);
				mainNet->updctrl("Shredder/synthNet/Series/postNet/Fanout/fano/Series/fanSeries/SoundFileSource/src2/mrs_natural/inSamples", D);
				mainNet->updctrl("Shredder/synthNet/Series/postNet/Fanout/fano/Series/fanSeries/SoundFileSource/src2/mrs_natural/inObservations", 1);
			}

			mainNet->updctrl("Shredder/synthNet/Series/postNet/Fanout/fano/SoundFileSink/dest/mrs_string/filename", outsfname);//[!]
			mainNet->updctrl("Shredder/synthNet/Series/postNet/SoundFileSink/destRes/mrs_string/filename", fileResName);//[!]
		}
		else
			mainNet->updctrl("Shredder/synthNet/Series/postNet/SoundFileSink/dest/mrs_string/filename", outsfname);//[!]
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
			bool temp = mainNet->getctrl("Accumulator/textWinNet/Series/analysisNet/FanOutIn/mixer/Series/oriNet/SoundFileSource/src/mrs_bool/hasData")->to<mrs_bool>();
			bool temp1 = textWinNet->getctrl("Series/analysisNet/FanOutIn/mixer/Series/oriNet/SoundFileSource/src/mrs_bool/hasData")->to<mrs_bool>();
			bool temp2 = analysisNet->getctrl("FanOutIn/mixer/Series/oriNet/SoundFileSource/src/mrs_bool/hasData")->to<mrs_bool>();

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
			///*bool*/ temp = mainNet->getctrl("Accumulator/textWinNet/Series/analysisNet/SoundFileSource/src/mrs_bool/hasData")->to<mrs_bool>();

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

	for (mrs_natural i = 0; i < kNumSimMeasures; i++)
	{
		cmd_options.addBoolOption(simMeasureProps[i].clLong, simMeasureProps[i].clShort, simMeasureProps[i].isIgnored);
	}
	//cmd_options.addBoolOption("ignoreFrequency", "if", simMeasureProps[kFreq].isIgnored);
	//cmd_options.addBoolOption("ignoreTime", "it", simMeasureProps[kTime].isIgnored);
	//cmd_options.addBoolOption("ignoreAmplitude", "ia", simMeasureProps[kAmp].isIgnored);
	//cmd_options.addBoolOption("ignoreDeltaFrequency", "idf", simMeasureProps[kDeltaFreq].isIgnored);
	//cmd_options.addBoolOption("ignoreDeltaAmplitude", "ida", simMeasureProps[kDeltaAmp].isIgnored);
	//cmd_options.addBoolOption("ignoreConnectivity", "ic", simMeasureProps[kConn].isIgnored);
	//cmd_options.addBoolOption("ignoreHWPS", "ih", simMeasureProps[kHwps].isIgnored);
	//cmd_options.addBoolOption("ignorePan", "ip", simMeasureProps[kPan].isIgnored);

	cmd_options.addBoolOption("horizWeight", "hw", useOnsets);

	cmd_options.addBoolOption("useOnsets", "uo", useOnsets);
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

#ifdef NEWBOOLS
	for (mrs_natural i = 0; i < kNumSimMeasures; i++)
	{
		simMeasureProps[i].isIgnored	= cmd_options.getBoolOption(simMeasureProps[i].clLong);
	}
	//ignoreSimMeasure[kFreq] = cmd_options.getBoolOption("ignoreFrequency");
	//ignoreSimMeasure[kTime] = cmd_options.getBoolOption("ignoreTime");
	//ignoreSimMeasure[kAmp] = cmd_options.getBoolOption("ignoreAmplitude");
	//ignoreSimMeasure[kDeltaFreq] = cmd_options.getBoolOption("ignoreDeltaFrequency");
	//ignoreSimMeasure[kDeltaAmp] = cmd_options.getBoolOption("ignoreDeltaAmplitude");
	//ignoreSimMeasure[kConn] = cmd_options.getBoolOption("ignoreConnectivity");
	//ignoreSimMeasure[kHwps] = cmd_options.getBoolOption("ignoreHWPS");
	//ignoreSimMeasure[kPan] = cmd_options.getBoolOption("ignorePan");
#else
	ignoreFrequency = cmd_options.getBoolOption("ignoreFrequency");
	ignoreTime = cmd_options.getBoolOption("ignoreTime");
	ignoreAmplitude = cmd_options.getBoolOption("ignoreAmplitude");
	ignoreDeltaFrequency = cmd_options.getBoolOption("ignoreDeltaFrequency");
	ignoreDeltaAmplitude = cmd_options.getBoolOption("ignoreDeltaAmplitude");
	ignoreConnectivity = cmd_options.getBoolOption("ignoreConnectivity");
	ignoreHWPS = cmd_options.getBoolOption("ignoreHWPS");
	ignorePan = cmd_options.getBoolOption("ignorePan");
#endif
	horizWeight = cmd_options.getBoolOption("horizWeight");
	useOnsets = cmd_options.getBoolOption("useOnsets");
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
				outputInf << "k" << nbSelectedClusters_;
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
				peakClustering(peakSet_, *sfi, fileName, noiseName, mixName, intervalFrequency, panningInfo, noiseDelay_, similarityType_, fftSize_, winSize_, hopSize_, nbSines_, nbClusters_, accSize_, synthetize_, &snr0);
			}	

		}

		timeElapsed = (clock()-nbTicks)/((mrs_real) CLOCKS_PER_SEC );
		cout << "Time elapsed: " << timeElapsed << endl;

		exit(0);
	}
}


