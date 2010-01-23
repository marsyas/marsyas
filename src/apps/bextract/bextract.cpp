/*
** Copyright (C) 2000-2008 George Tzanetakis <gtzan@cs.princeton.edu>
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


/**
bextract: batch feature extraction
*/

#include <cstdio>
#include <cstdlib>
#include "Collection.h"
#include "MarSystemManager.h"
#include "Accumulator.h"
#include "Fanout.h"
#include "CommandLineOptions.h"
#include "TimeLine.h"
#include "FileName.h"

#include <string>
using namespace std;
using namespace Marsyas;

int helpopt;
int usageopt;
int normopt;
bool tline;

mrs_natural offset = 0;
mrs_real duration = 30.0f;
mrs_natural memSize = 1;
mrs_natural winSize = 512;
mrs_natural hopSize = 512;
mrs_real samplingRate_ = 22050.0;
mrs_natural accSize_ = 1000;
mrs_real start = 0.0;
mrs_real length = -1.0;
mrs_real gain = 1.0;
mrs_bool pluginMute = 0.0;
mrs_bool playback = false;
mrs_bool stereo_ = false;
mrs_bool featExtract_ = false;

mrs_bool spsf_ = false;
mrs_bool mfcc_ = false;
mrs_bool chroma_ = false;
mrs_bool sfm_ = false;
mrs_bool scf_ = false;
mrs_natural downSample = 1;

mrs_bool ctd_ = false;
mrs_bool rlf_ = false;
mrs_bool flx_ = false;
mrs_bool lsp_ = false;
mrs_bool lpcc_ = false;

mrs_bool single_vector_ = false;

mrs_bool spectralFeatures_ = false;
mrs_bool zcrs_ = false;
mrs_bool timbralFeatures_ = false;
mrs_bool shuffle_;
mrs_bool mic_;


#define DEFAULT_EXTRACTOR "STFT"
#define DEFAULT_CLASSIFIER  "SVM"

string workspaceDir = EMPTYSTRING;
string pluginName = EMPTYSTRING;
string wekafname = EMPTYSTRING;
string extractorName = "REFACTORED";
string classifierName = EMPTYSTRING;
string collectionName = EMPTYSTRING;
string predictCollection = EMPTYSTRING;
string testCollection = EMPTYSTRING;

CommandLineOptions cmd_options;

// map table with pointers to the functions that create
// each specific and supported feature extraction MarSystem
typedef MarSystem* (*FeatureExtractorFactory)();
map<string,FeatureExtractorFactory >  featExtractors;
map<string, string> featExtractorDesc;

//////////////////////////////////////////////////////////////////////////
// Feature Extractors creation Functions
//////////////////////////////////////////////////////////////////////////
MarSystem* createExtractorFromFile()
{
	MarSystemManager mng;

	//this opens a .mpl file and creates in run-time
	//the MarSystem network for feature extraction.
	//NOTE:
	//the network should not contain any source MarSystem
	//since that is added by bextract
	ifstream mplFile(extractorName.c_str());

	return mng.getMarSystem(mplFile);
}

MarSystem* createBEATextrator()
{
	MarSystemManager mng;

	MarSystem* extractor = mng.create("Series", "beatExtrator");
	extractor->addMarSystem(mng.create("SoundFileSource", "src1"));
	extractor->addMarSystem(mng.create("Stereo2Mono", "s2m"));
	extractor->addMarSystem(mng.create("ShiftInput", "si"));
	extractor->addMarSystem(mng.create("DownSampler", "initds"));
	extractor->addMarSystem(mng.create("WaveletPyramid", "wvpt"));
	extractor->addMarSystem(mng.create("WaveletBands", "wvbnds"));
	extractor->addMarSystem(mng.create("FullWaveRectifier", "fwr"));
	extractor->addMarSystem(mng.create("OnePole", "lpf"));
	extractor->addMarSystem(mng.create("Norm", "norm"));
	{
	  // Extra gain added for compensating the cleanup of the Norm Marsystem,
	  // which used a 0.05 internal gain for some unknown reason.
	  // \todo is this weird gain factor actually required?
	  extractor->addMarSystem(mng.create("Gain", "normGain"));
	  extractor->updctrl("Gain/normGain/mrs_real/gain", 0.05);
	}
	extractor->addMarSystem(mng.create("Sum", "sum"));
	extractor->addMarSystem(mng.create("DownSampler", "ds"));
	extractor->addMarSystem(mng.create("AutoCorrelation", "acr"));
	extractor->addMarSystem(mng.create("Peaker", "pkr"));
	extractor->addMarSystem(mng.create("MaxArgMax", "mxr"));
	extractor->addMarSystem(mng.create("PeakPeriods2BPM", "p2bpm"));
	extractor->addMarSystem(mng.create("Histogram", "histo"));
	// extractor->addMarSystem(mng.create("PlotSink", "psink"));
	// extractor->addMarSystem(mng.create("Reassign", "reassign"));
	extractor->addMarSystem(mng.create("BeatHistoFeatures", "bhf"));
	extractor->linkctrl("mrs_natural/winSize", "ShiftInput/si/mrs_natural/winSize");
	return extractor;
}

MarSystem* createSTFTextractor()
{
	MarSystemManager mng;

	MarSystem* extractor = mng.create("Fanout", "STFTextractor");

	// Time domain features
	extractor->addMarSystem(mng.create("ZeroCrossings", "zcrs"));

	// Frequency domain features
	MarSystem* spectralShape = mng.create("Series", "spectralShape");
	spectralShape->addMarSystem(mng.create("PowerSpectrumNet","powerSpect"));

	MarSystem* spectrumFeatures = mng.create("STFT_features", "spectrumFeatures");
	spectralShape->addMarSystem(spectrumFeatures);
	extractor->addMarSystem(spectralShape);

	extractor->linkctrl("mrs_natural/winSize", "Series/spectralShape/PowerSpectrumNet/powerSpect/mrs_natural/winSize");
	extractor->linkctrl("mrs_string/enableChild", "Series/spectralShape/STFT_features/spectrumFeatures/mrs_string/enableChild");
	extractor->linkctrl("mrs_string/disableChild", "Series/spectralShape/STFT_features/spectrumFeatures/mrs_string/disableChild");

	return extractor;
}

MarSystem* createMFCCextractor()
{
	MarSystemManager mng;

	MarSystem* extractor = mng.create("Series", "MFCCextractor");
	extractor->addMarSystem(mng.create("PowerSpectrumNet","powerSpect"));
	extractor->addMarSystem(mng.create("MFCC", "mfcc"));

	extractor->linkctrl("mrs_natural/winSize", "PowerSpectrumNet/powerSpect/mrs_natural/winSize");

	return extractor;
}

MarSystem* createSTFTMFCCextractor()
{
	MarSystemManager mng;

	MarSystem* extractor = mng.create("Series", "STFTMFCCextractor");
	extractor->addMarSystem(mng.create("PowerSpectrumNet","powerSpect"));
	// Spectrum Shape descriptors
	MarSystem* spectrumFeatures = mng.create("Fanout", "spectrumFeatures");
	spectrumFeatures->addMarSystem(mng.create("Centroid", "cntrd"));
	spectrumFeatures->addMarSystem(mng.create("Rolloff", "rlf"));
	spectrumFeatures->addMarSystem(mng.create("Flux", "flux"));
	spectrumFeatures->addMarSystem(mng.create("MFCC", "mfcc"));
	extractor->addMarSystem(spectrumFeatures);
	extractor->linkctrl("mrs_natural/winSize", "PowerSpectrumNet/powerSpect/mrs_natural/winSize");

	return extractor;
}

MarSystem* createSCFextractor()
{
	MarSystemManager mng;

	MarSystem* extractor = mng.create("Series", "SCFextractor");
	extractor->addMarSystem(mng.create("PowerSpectrumNet","powerSpect"));
	extractor->addMarSystem(mng.create("SCF", "scf"));

	extractor->linkctrl("mrs_natural/winSize", "PowerSpectrumNet/powerSpect/mrs_natural/winSize");

	return extractor;
}

MarSystem* createSFMextractor()
{
	MarSystemManager mng;

	MarSystem* extractor = mng.create("Series", "SFMextractor");
	extractor->addMarSystem(mng.create("PowerSpectrumNet","powerSpect"));
	extractor->addMarSystem(mng.create("SFM", "sfm"));

	extractor->linkctrl("mrs_natural/winSize", "PowerSpectrumNet/powerSpect/mrs_natural/winSize");

	return extractor;
}

MarSystem* createSFMSCFextractor()
{
	MarSystemManager mng;

	MarSystem* extractor = mng.create("Series", "SFMSCFextractor");

	extractor->addMarSystem(mng.create("PowerSpectrumNet","powerSpect"));
	// Spectrum Shape descriptors
	MarSystem* spectrumFeatures = mng.create("Fanout", "spectrumFeatures");
	spectrumFeatures->addMarSystem(mng.create("SFM", "sfm"));
	spectrumFeatures->addMarSystem(mng.create("SCF", "scf"));

	extractor->addMarSystem(spectrumFeatures);

	extractor->linkctrl("mrs_natural/winSize", "PowerSpectrumNet/powerSpect/mrs_natural/winSize");

	return extractor;
}

MarSystem* createLSPextractor()
{
	MarSystemManager mng;

	mrs_natural order = 18;
	cout << "LSP order = " << order << endl;

	MarSystem* extractor = mng.create("Series","LSPextractor");

	//first add a LPC calculation net
	extractor->addMarSystem(mng.create("LPCnet", "lpcNet"));
	//add the LSP calculation
	extractor->addMarSystem(mng.create("LSP", "lsp"));

	//must create a link otherwise the outmost composite will know nothing
	//about this order change (which affects output nr of observations) and
	//consequently will not update the network accordingly!
	extractor->linkctrl("mrs_natural/order", "LPCnet/lpcNet/mrs_natural/order");
	extractor->updctrl("mrs_natural/order", order);

	extractor->linkctrl("mrs_natural/winSize", "LPCnet/lpcNet/mrs_natural/winSize");

	return extractor;
}

MarSystem* createLPCCextractor()
{
	MarSystemManager mng;

	mrs_natural order = 12; //12 order LPCC
	cout << "LPCC order = " << order << endl;

	MarSystem* extractor = mng.create("Series","LPCCextractor");

	//first add a LPC calculation net
	extractor->addMarSystem(mng.create("LPCnet", "lpcNet"));
	//add the LPCC calculation
	extractor->addMarSystem(mng.create("LPCC", "lpcc"));

	//must create a link otherwise the outmost composite will know nothing
	//about this order change (which affects output nr of observations) and
	//consequently will not update the network accordingly!
	extractor->linkctrl("mrs_natural/order", "LPCnet/lpcNet/mrs_natural/order");
	extractor->updctrl("mrs_natural/order", order);

	extractor->linkctrl("mrs_natural/winSize", "LPCnet/lpcNet/mrs_natural/winSize");

	return extractor;
}

void
printUsage(string progName)
{
	MRSDIAG("bextract.cpp - printUsage");
	cerr << "Usage : " << progName << " [-e extractor] [-h help] [-s start(seconds)] [-l length(seconds)] [-m memory]  [-u usage] collection1 collection2 ... collectionN" << endl;
	cerr << endl;
	exit(0);
}

void
printHelp(string progName)
{
	MRSDIAG("bextract.cpp - printHelp");
	cerr << "bextract, MARSYAS, Copyright George Tzanetakis " << endl;
	cerr << "--------------------------------------------" << endl;
	cerr << "Prints information about the sound files provided as arguments " << endl;
	cerr << endl;
	cerr << "Usage : " << progName << "[-c collection] file1 file2 file3" << endl;
	cerr << endl;
	cerr << "where file1, ..., fileN are sound files in a Marsyas supported format" << endl;
	cerr << "Help Options:" << endl;
	cerr << "-u  --usage        : display short usage info" << endl;
	cerr << "-v  --verbose      : verbose output" << endl;
	cerr << "-c  --collection   : use files in this collection" << endl;
	cerr << "-n  --normalize    : enable normalization" << endl;
	cerr << "-as --accSize      : accumulator size" << endl;
	cerr << "-cl --classifier   : classifier name" << endl;
	cerr << "-pr --predict      : predict class for files in collection" << endl;
	cerr << "-fe --featExtract  : only extract features" << endl;
	cerr << "-tc --test         : test collection" << endl;
	cerr << "-st --stereo       : use stereo feature extraction" << endl;
	cerr << "-ds --downsample   : downsampling factor" << endl;
	cerr << "-h  --help         : display this information " << endl;
	cerr << "-e  --extractor    : extractor " << endl;
	cerr << "-p  --plugin       : output plugin name " << endl;
	cerr << "-pm --pluginmute   : mute the plugin " << endl;
	cerr << "-pb --playback     : playback during feature extraction " << endl;
	cerr << "-s  --start        : playback start offset in seconds " << endl;
	cerr << "-sh --shuffle      : shuffle collection file before processing" << endl;

	cerr << "-l  --length       : playback length in seconds " << endl;
	cerr << "-m  --memory       : memory size " << endl;
	cerr << "-w  --weka         : weka .arff filename " << endl;
	cerr << "-od --outputdir    : output directory for output of files" << endl;
	cerr << "-ws --winsamples   : analysis window size in samples " << endl;
	cerr << "-hp --hopsamples   : analysis hop size in samples " << endl;
	cerr << "-t  --timeline     : flag 2nd input collection as timelines for the 1st collection";
	cerr << endl;

	cerr << "Available extractors: " << endl;
	cerr << "---------------------" << endl;
	map<string, FeatureExtractorFactory >::const_iterator it;
	for(it = featExtractors.begin(); it != featExtractors.end(); ++it)
	{
		cerr << it->first << ": " << featExtractorDesc[it->first] << endl;
	}
	cerr << endl;
	cerr << "NOTE:" << endl;
	cerr << "All extractors calculate means and variances over a memory size window" << endl;
	cerr << "SV can be appended in front of any extractor to extract a single vector (mean, variances) over a 30-second clip (for example SVSTFT) " << endl;

	exit(0);
}

void
tempo_histoSumBands(MarSystem* total1, string sfName, realvec& beatfeatures,
										realvec& iwin, realvec& estimate)
{
	estimate.setval(0.0);

	mrs_real srate;

	// prepare network

	// update the controls
	// input filename with hopSize/winSize

	total1->updctrl("SoundFileSource/src1/mrs_string/filename", sfName);
	srate = total1->getctrl("SoundFileSource/src1/mrs_real/osrate")->to<mrs_real>();

	mrs_natural ifactor = 8;
	total1->updctrl("DownSampler/initds/mrs_natural/factor", ifactor);

	mrs_natural winSize = (mrs_natural) ((srate / 22050.0) * 2 * 65536);
	mrs_natural hopSize = winSize / 16;

	offset = (mrs_natural) (start * srate);

	// only do 30 seconds
	duration = (mrs_natural) (30.0 * srate);

	total1->updctrl("mrs_natural/inSamples", hopSize);
	total1->updctrl("SoundFileSource/src1/mrs_natural/pos", offset);
	total1->updctrl("SoundFileSource/src1/mrs_natural/inSamples", hopSize);
	total1->updctrl("ShiftInput/si/mrs_natural/winSize", winSize);
	total1->updctrl("ShiftInput/si/mrs_bool/reset", true);
	total1->updctrl("MaxArgMax/mxr/mrs_natural/nMaximums", 3);

	// wavelet filterbank envelope extraction controls
	total1->updctrl("WaveletPyramid/wvpt/mrs_bool/forward", true);
	total1->updctrl("OnePole/lpf/mrs_real/alpha", 0.99f);
	mrs_natural factor = 32;
	total1->updctrl("DownSampler/ds/mrs_natural/factor", factor);

	srate = total1->getctrl("DownSampler/initds/mrs_real/osrate")->to<mrs_real>();

	// Peak picker 4BPMs at 60BPM resolution from 50 BPM to 250 BPM
	mrs_natural pkinS = total1->getctrl("Peaker/pkr/mrs_natural/onSamples")->to<mrs_natural>();
	mrs_real peakSpacing = ((mrs_natural)(srate * 60.0 / (factor *60.0)) -
		(mrs_natural)(srate * 60.0 / (factor *62.0))) / (pkinS * 1.0);
	mrs_natural peakStart = (mrs_natural)(srate * 60.0 / (factor * 200.0));
	mrs_natural peakEnd   = (mrs_natural)(srate * 60.0 / (factor * 50.0));
	total1->updctrl("Peaker/pkr/mrs_real/peakSpacing", peakSpacing);
	total1->updctrl("Peaker/pkr/mrs_real/peakStrength", 0.5);
	total1->updctrl("Peaker/pkr/mrs_natural/peakStart", peakStart);
	total1->updctrl("Peaker/pkr/mrs_natural/peakEnd", peakEnd);
	total1->updctrl("Peaker/pkr/mrs_real/peakGain", 2.0);
	total1->updctrl("Histogram/histo/mrs_natural/startBin", 0);
	total1->updctrl("Histogram/histo/mrs_natural/endBin", 250);
	total1->updctrl("Histogram/histo/mrs_bool/reset", true);

	// prepare vectors for processing
	/* realvec iwin(total->getctrl("mrs_natural/inObservations")->to<mrs_natural>(),
	total->getctrl("mrs_natural/inSamples")->to<mrs_natural>());
	realvec estimate(total->getctrl("mrs_natural/onObservations")->to<mrs_natural>(),
	total->getctrl("mrs_natural/onSamples")->to<mrs_natural>());
	*/

	mrs_natural onSamples;

	int numPlayed =0;
	mrs_natural wc=0;
	mrs_natural samplesPlayed = 0;

	// vector of bpm estimate used to calculate median
	onSamples = total1->getctrl("ShiftInput/si/mrs_natural/onSamples")->to<mrs_natural>();

	total1->updctrl("SoundFileSource/src1/mrs_natural/pos", 0);

	while (total1->getctrl("SoundFileSource/src1/mrs_bool/notEmpty")->to<mrs_bool>())
	{
		total1->process(iwin, estimate);

		numPlayed++;
		wc ++;
		samplesPlayed += onSamples;
		// no duration specified so use all of source input
	}

	// cout << "FINAL = " << bpms[bpms.size()-1] << endl;

	beatfeatures = estimate;
}

void
bextract_trainStereoSPS(vector<Collection> cls, string classNames,
												string wekafname, mrs_natural memSize)
{
	cout << "STEREO SPS" << endl;
	cout << "classNames = "  << classNames << endl;
	cout << "wekafname = "  << wekafname << endl;

	MarSystemManager mng;

	MarSystem* playbacknet = mng.create("Series", "playbacknet");
	playbacknet->addMarSystem(mng.create("SoundFileSource", "src"));
	// playbacknet->addMarSystem(mng.create("AudioSink", "dest"));

	MarSystem* stereobranches = mng.create("Parallel", "stereobranches");
	MarSystem* left = mng.create("Series", "left");
	MarSystem* right = mng.create("Series", "right");

	left->addMarSystem(mng.create("Windowing", "hamleft"));
	left->addMarSystem(mng.create("Spectrum", "spkleft"));
	right->addMarSystem(mng.create("Windowing", "hamright"));
	right->addMarSystem(mng.create("Spectrum", "spkright"));

	stereobranches->addMarSystem(left);
	stereobranches->addMarSystem(right);

	playbacknet->addMarSystem(stereobranches);
	playbacknet->addMarSystem(mng.create("StereoSpectrum", "sspk"));
	playbacknet->addMarSystem(mng.create("StereoSpectrumFeatures", "sspkf"));
	playbacknet->addMarSystem(mng.create("TextureStats", "texturests"));

	MarSystem* acc = mng.create("Accumulator", "acc");
	acc->addMarSystem(playbacknet);

	MarSystem* statistics2 = mng.create("Fanout", "statistics2");
	statistics2->addMarSystem(mng.create("Mean", "mn"));
	statistics2->addMarSystem(mng.create("StandardDeviation", "std"));

	MarSystem* total = mng.create("Series", "total");
	total->addMarSystem(acc);
	total->updctrl("Accumulator/acc/mrs_natural/nTimes", 1000);
	total->addMarSystem(statistics2);

	total->addMarSystem(mng.create("Annotator", "ann"));
	total->addMarSystem(mng.create("WekaSink", "wsink"));
	total->addMarSystem(mng.create("SVMClassifier", "svmcl"));

	total->updctrl("mrs_natural/inSamples", 1024);

	mrs_bool collection_has_labels = false;

	if ((cls.size() == 1)&&(cls[0].hasLabels()))
	{
		collection_has_labels = true;
	}

	// cout << *total << endl;
	Collection l;

	if (!collection_has_labels)
	{
		total->updctrl("WekaSink/wsink/mrs_natural/nLabels", (mrs_natural)cls.size());
		total->updctrl("WekaSink/wsink/mrs_natural/downsample", 1);
		total->updctrl("WekaSink/wsink/mrs_string/labelNames", classNames);
		total->updctrl("WekaSink/wsink/mrs_string/filename", wekafname);
		unsigned int cj;
		int i;
		for (cj=0; cj < cls.size(); cj++)
		{
			Collection l = cls[cj];

			total->updctrl("Annotator/ann/mrs_natural/label", (mrs_natural)cj);
			for (i=0; i < l.size(); i++)
			{
				total->updctrl("Accumulator/acc/Series/playbacknet/SoundFileSource/src/mrs_string/filename", l.entry(i));
				cout << "Processing" << l.entry(i) << endl;
				total->tick();
			}
		}
	}
	else
	{
		int i;
		l = cls[0];

		total->updctrl("WekaSink/wsink/mrs_natural/nLabels", (mrs_natural)l.getNumLabels());
		total->updctrl("WekaSink/wsink/mrs_natural/downsample", 1);
		total->updctrl("WekaSink/wsink/mrs_string/labelNames", l.getLabelNames());
		total->updctrl("WekaSink/wsink/mrs_string/filename", wekafname);
		total->updctrl("SVMClassifier/svmcl/mrs_string/mode", "train");

		for (i=0; i < l.size(); i++)
		{
			total->updctrl("Accumulator/acc/Series/playbacknet/SoundFileSource/src/mrs_string/filename", l.entry(i));
			total->updctrl("Annotator/ann/mrs_natural/label", l.labelNum(l.labelEntry(i)));
			cout << "Processing" << l.entry(i) << endl;
			total->tick();
		}
	}

	int i;
	if (testCollection != EMPTYSTRING)
	{
		Collection m;
		m.read(testCollection);
		if (wekafname != EMPTYSTRING)
			total->updctrl("WekaSink/wsink/mrs_string/filename", "predict.arff");
		total->updctrl("SVMClassifier/svmcl/mrs_string/mode", "predict");

		ofstream prout;
		prout.open(predictCollection.c_str());

		for (i=0; i < m.size(); i++)//iterate over collection files
		{
			total->updctrl("Accumulator/acc/Series/playbacknet/SoundFileSource/src/mrs_string/filename", m.entry(i));
			total->tick();
			mrs_realvec pr = total->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();
			cout << "Predicting " << m.entry(i) << "\t" << l.labelName((mrs_natural)pr(0,0)) << endl;
			prout << m.entry(i) << "\t" << l.labelName((mrs_natural)pr(0,0)) << endl;
		}
	}
}

void
bextract_trainStereoSPSMFCC(vector<Collection> cls, string classNames,
														string wekafname, mrs_natural memSize)
{
	cout << "STEREO SPS+MFCC" << endl;
	cout << "classNames = "  << classNames << endl;
	cout << "wekafname = "  << wekafname << endl;

	MarSystemManager mng;

	MarSystem* playbacknet = mng.create("Series", "playbacknet");
	playbacknet->addMarSystem(mng.create("SoundFileSource", "src"));

	MarSystem* ffanout = mng.create("Fanout", "ffanout");

	MarSystem* stereobranches = mng.create("Parallel", "stereobranches");
	MarSystem* left = mng.create("Series", "left");
	MarSystem* right = mng.create("Series", "right");

	left->addMarSystem(mng.create("Windowing", "hamleft"));
	left->addMarSystem(mng.create("Spectrum", "spkleft"));
	left->addMarSystem(mng.create("PowerSpectrum", "leftpspk"));
	left->addMarSystem(mng.create("MFCC", "leftMFCC"));
	left->addMarSystem(mng.create("TextureStats", "leftTextureStats"));

	right->addMarSystem(mng.create("Windowing", "hamright"));
	right->addMarSystem(mng.create("Spectrum", "spkright"));
	right->addMarSystem(mng.create("PowerSpectrum", "rightpspk"));
	right->addMarSystem(mng.create("MFCC", "rightMFCC"));
	right->addMarSystem(mng.create("TextureStats", "rightTextureStats"));

	stereobranches->addMarSystem(left);
	stereobranches->addMarSystem(right);

	MarSystem* secondbranch = mng.create("Series", "secondbranch");
	MarSystem* stereobranches1 = mng.create("Parallel", "stereobranches1");
	MarSystem* left1 = mng.create("Series", "left1");
	MarSystem* right1 = mng.create("Series", "right1");

	left1->addMarSystem(mng.create("Windowing", "hamleft1"));
	left1->addMarSystem(mng.create("Spectrum", "spkleft1"));
	right1->addMarSystem(mng.create("Windowing", "hamright1"));
	right1->addMarSystem(mng.create("Spectrum", "spkright1"));

	stereobranches1->addMarSystem(left1);
	stereobranches1->addMarSystem(right1);

	secondbranch->addMarSystem(stereobranches1);
	secondbranch->addMarSystem(mng.create("StereoSpectrum", "sspk"));
	secondbranch->addMarSystem(mng.create("StereoSpectrumFeatures", "sspkf"));
	secondbranch->addMarSystem(mng.create("TextureStats", "texturests"));

	playbacknet->addMarSystem(ffanout);
	ffanout->addMarSystem(stereobranches);
	ffanout->addMarSystem(secondbranch);

	MarSystem* acc = mng.create("Accumulator", "acc");
	acc->addMarSystem(playbacknet);

	MarSystem* statistics2 = mng.create("Fanout", "statistics2");
	statistics2->addMarSystem(mng.create("Mean", "mn"));
	statistics2->addMarSystem(mng.create("StandardDeviation", "std"));

	MarSystem* total = mng.create("Series", "total");
	total->addMarSystem(acc);
	total->updctrl("Accumulator/acc/mrs_natural/nTimes", 1000);
	total->addMarSystem(statistics2);

	total->addMarSystem(mng.create("Annotator", "ann"));
	total->addMarSystem(mng.create("WekaSink", "wsink"));
	total->addMarSystem(mng.create("SVMClassifier", "svmcl"));

	total->updctrl("mrs_natural/inSamples", 1024);

	mrs_bool collection_has_labels = false;

	if ((cls.size() == 1)&&(cls[0].hasLabels()))
	{
		collection_has_labels = true;
	}

	Collection l;

	if (!collection_has_labels)
	{
		// cout << *total << endl;
		unsigned int cj;
		int i;

		total->updctrl("WekaSink/wsink/mrs_natural/nLabels", (mrs_natural)cls.size());
		total->updctrl("WekaSink/wsink/mrs_natural/downsample", 1);
		total->updctrl("WekaSink/wsink/mrs_string/labelNames", classNames);
		total->updctrl("WekaSink/wsink/mrs_string/filename", wekafname);

		for (cj=0; cj < cls.size(); cj++)
		{
			Collection l = cls[cj];
			total->updctrl("Annotator/ann/mrs_natural/label", (mrs_natural)cj);
			for (i=0; i < l.size(); i++)
			{
				total->updctrl("Accumulator/acc/Series/playbacknet/SoundFileSource/src/mrs_string/filename", l.entry(i));
				cout << "Processing" << l.entry(i) << endl;
				total->tick();
			}
		}
	}
	else
	{
		int i;
		l = cls[0];

		total->updctrl("WekaSink/wsink/mrs_natural/nLabels", (mrs_natural)l.getNumLabels());
		total->updctrl("WekaSink/wsink/mrs_natural/downsample", 1);
		total->updctrl("WekaSink/wsink/mrs_string/labelNames", l.getLabelNames());
		total->updctrl("WekaSink/wsink/mrs_string/filename", wekafname);
		total->updctrl("SVMClassifier/svmcl/mrs_string/mode", "train");

		for (i=0; i < l.size(); i++)
		{
			total->updctrl("Accumulator/acc/Series/playbacknet/SoundFileSource/src/mrs_string/filename", l.entry(i));
			total->updctrl("Annotator/ann/mrs_natural/label", l.labelNum(l.labelEntry(i)));
			cout << "Processing" << l.entry(i) << endl;
			total->tick();
		}
	}

	int i;
	if (testCollection != EMPTYSTRING)
	{
		Collection m;
		m.read(testCollection);
		if (wekafname != EMPTYSTRING)
			total->updctrl("WekaSink/wsink/mrs_string/filename", "predict.arff");
		total->updctrl("SVMClassifier/svmcl/mrs_string/mode", "predict");

		ofstream prout;
		prout.open(predictCollection.c_str());

		for (i=0; i < m.size(); i++)//iterate over collection files
		{
			total->updctrl("Accumulator/acc/Series/playbacknet/SoundFileSource/src/mrs_string/filename", m.entry(i));
			total->tick();
			mrs_realvec pr = total->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();
			cout << "Predicting " << m.entry(i) << "\t" << l.labelName((mrs_natural)pr(0,0)) << endl;
			prout << m.entry(i) << "\t" << l.labelName((mrs_natural)pr(0,0)) << endl;
		}
	}
}

void
bextract_trainStereoMFCC(vector<Collection> cls, string classNames,
												 string wekafname, mrs_natural memSize)
{
	cout << "STEREO MFCC" << endl;
	cout << "classNames = "  << classNames << endl;
	cout << "wekafname = "  << wekafname << endl;

	MarSystemManager mng;

	MarSystem* playbacknet = mng.create("Series", "playbacknet");
	playbacknet->addMarSystem(mng.create("SoundFileSource", "src"));

	MarSystem* stereobranches = mng.create("Parallel", "stereobranches");
	MarSystem* left = mng.create("Series", "left");
	MarSystem* right = mng.create("Series", "right");

	left->addMarSystem(mng.create("Windowing", "hamleft"));
	left->addMarSystem(mng.create("Spectrum", "spkleft"));
	left->addMarSystem(mng.create("PowerSpectrum", "leftpspk"));
	left->addMarSystem(mng.create("MFCC", "leftMFCC"));
	left->addMarSystem(mng.create("TextureStats", "leftTextureStats"));

	right->addMarSystem(mng.create("Windowing", "hamright"));
	right->addMarSystem(mng.create("Spectrum", "spkright"));
	right->addMarSystem(mng.create("PowerSpectrum", "rightpspk"));
	right->addMarSystem(mng.create("MFCC", "rightMFCC"));
	right->addMarSystem(mng.create("TextureStats", "rightTextureStats"));

	stereobranches->addMarSystem(left);
	stereobranches->addMarSystem(right);

	playbacknet->addMarSystem(stereobranches);

	MarSystem* acc = mng.create("Accumulator", "acc");
	acc->addMarSystem(playbacknet);

	MarSystem* statistics2 = mng.create("Fanout", "statistics2");
	statistics2->addMarSystem(mng.create("Mean", "mn"));
	statistics2->addMarSystem(mng.create("StandardDeviation", "std"));

	MarSystem* total = mng.create("Series", "total");
	total->addMarSystem(acc);
	total->updctrl("Accumulator/acc/mrs_natural/nTimes", 1000);
	total->addMarSystem(statistics2);

	total->addMarSystem(mng.create("Annotator", "ann"));
	total->addMarSystem(mng.create("WekaSink", "wsink"));
	total->addMarSystem(mng.create("SVMClassifier", "svmcl"));
	playbacknet->linkControl("mrs_bool/notEmpty", "SoundFileSource/src/mrs_bool/notEmpty");

	total->updctrl("mrs_natural/inSamples", 1024);

	mrs_bool collection_has_labels = false;

	if ((cls.size() == 1)&&(cls[0].hasLabels()))
	{
		collection_has_labels = true;
	}

	// cout << *total << endl;
	Collection l;

	if (!collection_has_labels)
	{
		unsigned int cj;
		int i;

		total->updctrl("WekaSink/wsink/mrs_natural/nLabels", (mrs_natural)cls.size());
		total->updctrl("WekaSink/wsink/mrs_natural/downsample", 1);
		total->updctrl("WekaSink/wsink/mrs_string/labelNames", classNames);
		total->updctrl("WekaSink/wsink/mrs_string/filename", wekafname);

		for (cj=0; cj < cls.size(); cj++)
		{
			Collection l = cls[cj];
			total->updctrl("Annotator/ann/mrs_natural/label", (mrs_natural)cj);
			for (i=0; i < l.size(); i++)
			{
				total->updctrl("Accumulator/acc/Series/playbacknet/SoundFileSource/src/mrs_string/filename", l.entry(i));
				cout << "Processing" << l.entry(i) << endl;
				total->tick();
			}
		}
	}
	else
	{

		int i;
		l = cls[0];

		total->updctrl("WekaSink/wsink/mrs_natural/nLabels", (mrs_natural)l.getNumLabels());
		total->updctrl("WekaSink/wsink/mrs_natural/downsample", 1);
		total->updctrl("WekaSink/wsink/mrs_string/labelNames", l.getLabelNames());
		total->updctrl("WekaSink/wsink/mrs_string/filename", wekafname);
		total->updctrl("SVMClassifier/svmcl/mrs_string/mode", "train");

		for (i=0; i < l.size(); i++)
		{
			total->updctrl("Accumulator/acc/Series/playbacknet/SoundFileSource/src/mrs_string/filename", l.entry(i));
			total->updctrl("Annotator/ann/mrs_natural/label", l.labelNum(l.labelEntry(i)));
			cout << "Processing" << l.entry(i) << endl;
			total->tick();
		}
	}


	int i;
	if (testCollection != EMPTYSTRING)
	{
		Collection m;
		m.read(testCollection);
		if (wekafname != EMPTYSTRING)
			total->updctrl("WekaSink/wsink/mrs_string/filename", "predict.arff");
		total->updctrl("SVMClassifier/svmcl/mrs_string/mode", "predict");

		ofstream prout;
		prout.open(predictCollection.c_str());

		for (i=0; i < m.size(); i++)//iterate over collection files
		{
			total->updctrl("Accumulator/acc/Series/playbacknet/SoundFileSource/src/mrs_string/filename", m.entry(i));
			total->tick();
			mrs_realvec pr = total->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();
			cout << "Predicting " << m.entry(i) << "\t" << l.labelName((mrs_natural)pr(0,0)) << endl;
			prout << m.entry(i) << "\t" << l.labelName((mrs_natural)pr(0,0)) << endl;
		}
	}



}
//---------------------------------------------------------------------------------------------------------
void
bextract_trainADRessStereoSPS(vector<Collection> cls, string classNames,
															string wekafname, mrs_natural memSize)
{
	cout << "ADRess STEREO SPS" << endl;
	cout << "classNames = "  << classNames << endl;
	cout << "wekafname = "  << wekafname << endl;

	MarSystemManager mng;

	MarSystem* playbacknet = mng.create("Series", "playbacknet");
	playbacknet->addMarSystem(mng.create("SoundFileSource", "src"));
	// playbacknet->addMarSystem(mng.create("AudioSink", "dest"));

	MarSystem* stereobranches = mng.create("Parallel", "stereobranches");
	MarSystem* left = mng.create("Series", "left");
	MarSystem* right = mng.create("Series", "right");

	left->addMarSystem(mng.create("Windowing", "hamleft"));
	left->addMarSystem(mng.create("Spectrum", "spkleft"));
	right->addMarSystem(mng.create("Windowing", "hamright"));
	right->addMarSystem(mng.create("Spectrum", "spkright"));

	stereobranches->addMarSystem(left);
	stereobranches->addMarSystem(right);

	playbacknet->addMarSystem(stereobranches);
	playbacknet->addMarSystem(mng.create("ADRess", "aspk"));
	playbacknet->addMarSystem(mng.create("ADRessStereoSpectrum", "asspk"));
	playbacknet->addMarSystem(mng.create("StereoSpectrumFeatures", "sspkf"));
	playbacknet->addMarSystem(mng.create("TextureStats", "texturests"));

	MarSystem* acc = mng.create("Accumulator", "acc");
	acc->addMarSystem(playbacknet);

	MarSystem* statistics2 = mng.create("Fanout", "statistics2");
	statistics2->addMarSystem(mng.create("Mean", "mn"));
	statistics2->addMarSystem(mng.create("StandardDeviation", "std"));

	MarSystem* total = mng.create("Series", "total");
	total->addMarSystem(acc);
	total->updctrl("Accumulator/acc/mrs_natural/nTimes", 1000);
	total->addMarSystem(statistics2);

	total->addMarSystem(mng.create("Annotator", "ann"));
	total->addMarSystem(mng.create("WekaSink", "wsink"));

	total->updctrl("mrs_natural/inSamples", 1024);

	mrs_bool collection_has_labels = false;

	if ((cls.size() == 1)&&(cls[0].hasLabels()))
	{
		collection_has_labels = true;
	}

	// cout << *total << endl;

	if (!collection_has_labels)
	{
		total->updctrl("WekaSink/wsink/mrs_natural/nLabels", (mrs_natural)cls.size());
		total->updctrl("WekaSink/wsink/mrs_natural/downsample", 1);
		total->updctrl("WekaSink/wsink/mrs_string/labelNames", classNames);
		total->updctrl("WekaSink/wsink/mrs_string/filename", wekafname);
		unsigned int cj;
		int i;
		for (cj=0; cj < cls.size(); cj++)
		{
			Collection l = cls[cj];

			total->updctrl("Annotator/ann/mrs_natural/label", (mrs_natural)cj);
			for (i=0; i < l.size(); i++)
			{
				total->updctrl("Accumulator/acc/Series/playbacknet/SoundFileSource/src/mrs_string/filename", l.entry(i));
				cout << "Processing" << l.entry(i) << endl;
				total->tick();
			}
		}
	}
	else
	{
		Collection l;
		int i;
		l = cls[0];

		total->updctrl("WekaSink/wsink/mrs_natural/nLabels", (mrs_natural)l.getNumLabels());
		total->updctrl("WekaSink/wsink/mrs_natural/downsample", 1);
		total->updctrl("WekaSink/wsink/mrs_string/labelNames", l.getLabelNames());
		total->updctrl("WekaSink/wsink/mrs_string/filename", wekafname);

		for (i=0; i < l.size(); i++)
		{
			total->updctrl("Accumulator/acc/Series/playbacknet/SoundFileSource/src/mrs_string/filename", l.entry(i));
			total->updctrl("Annotator/ann/mrs_natural/label", l.labelNum(l.labelEntry(i)));
			cout << "Processing" << l.entry(i) << endl;
			total->tick();
		}
	}
}

void
bextract_trainADRessStereoSPSMFCC(vector<Collection> cls, string classNames,
																	string wekafname, mrs_natural memSize)
{
	cout << "ADRess STEREO SPS+MFCC" << endl;
	cout << "classNames = "  << classNames << endl;
	cout << "wekafname = "  << wekafname << endl;

	MarSystemManager mng;

	MarSystem* playbacknet = mng.create("Series", "playbacknet");
	playbacknet->addMarSystem(mng.create("SoundFileSource", "src"));

	MarSystem* ffanout = mng.create("Fanout", "ffanout");

	MarSystem* stereobranches = mng.create("Parallel", "stereobranches");
	MarSystem* left = mng.create("Series", "left");
	MarSystem* right = mng.create("Series", "right");

	left->addMarSystem(mng.create("Windowing", "hamleft"));
	left->addMarSystem(mng.create("Spectrum", "spkleft"));
	left->addMarSystem(mng.create("PowerSpectrum", "leftpspk"));
	left->addMarSystem(mng.create("MFCC", "leftMFCC"));
	left->addMarSystem(mng.create("TextureStats", "leftTextureStats"));

	right->addMarSystem(mng.create("Windowing", "hamright"));
	right->addMarSystem(mng.create("Spectrum", "spkright"));
	right->addMarSystem(mng.create("PowerSpectrum", "rightpspk"));
	right->addMarSystem(mng.create("MFCC", "rightMFCC"));
	right->addMarSystem(mng.create("TextureStats", "rightTextureStats"));

	stereobranches->addMarSystem(left);
	stereobranches->addMarSystem(right);

	MarSystem* secondbranch = mng.create("Series", "secondbranch");
	MarSystem* stereobranches1 = mng.create("Parallel", "stereobranches1");
	MarSystem* left1 = mng.create("Series", "left1");
	MarSystem* right1 = mng.create("Series", "right1");

	left1->addMarSystem(mng.create("Windowing", "hamleft1"));
	left1->addMarSystem(mng.create("Spectrum", "spkleft1"));
	right1->addMarSystem(mng.create("Windowing", "hamright1"));
	right1->addMarSystem(mng.create("Spectrum", "spkright1"));

	stereobranches1->addMarSystem(left1);
	stereobranches1->addMarSystem(right1);

	secondbranch->addMarSystem(stereobranches1);
	secondbranch->addMarSystem(mng.create("ADRess", "aspk"));
	secondbranch->addMarSystem(mng.create("ADRessStereoSpectrum", "asspk"));
	secondbranch->addMarSystem(mng.create("StereoSpectrumFeatures", "sspkf"));
	secondbranch->addMarSystem(mng.create("TextureStats", "texturests"));

	playbacknet->addMarSystem(ffanout);
	ffanout->addMarSystem(stereobranches);
	ffanout->addMarSystem(secondbranch);

	MarSystem* acc = mng.create("Accumulator", "acc");
	acc->addMarSystem(playbacknet);

	MarSystem* statistics2 = mng.create("Fanout", "statistics2");
	statistics2->addMarSystem(mng.create("Mean", "mn"));
	statistics2->addMarSystem(mng.create("StandardDeviation", "std"));

	MarSystem* total = mng.create("Series", "total");
	total->addMarSystem(acc);
	total->updctrl("Accumulator/acc/mrs_natural/nTimes", 1000);
	total->addMarSystem(statistics2);

	total->addMarSystem(mng.create("Annotator", "ann"));
	total->addMarSystem(mng.create("WekaSink", "wsink"));

	total->updctrl("mrs_natural/inSamples", 1024);

	mrs_bool collection_has_labels = false;

	if ((cls.size() == 1)&&(cls[0].hasLabels()))
	{
		collection_has_labels = true;
	}

	if (!collection_has_labels)
	{
		// cout << *total << endl;
		unsigned int cj;
		int i;

		total->updctrl("WekaSink/wsink/mrs_natural/nLabels", (mrs_natural)cls.size());
		total->updctrl("WekaSink/wsink/mrs_natural/downsample", 1);
		total->updctrl("WekaSink/wsink/mrs_string/labelNames", classNames);
		total->updctrl("WekaSink/wsink/mrs_string/filename", wekafname);

		for (cj=0; cj < cls.size(); cj++)
		{
			Collection l = cls[cj];
			total->updctrl("Annotator/ann/mrs_natural/label", (mrs_natural)cj);
			for (i=0; i < l.size(); i++)
			{
				total->updctrl("Accumulator/acc/Series/playbacknet/SoundFileSource/src/mrs_string/filename", l.entry(i));
				cout << "Processing" << l.entry(i) << endl;
				total->tick();
			}
		}
	}
	else
	{
		Collection l;
		int i;
		l = cls[0];

		total->updctrl("WekaSink/wsink/mrs_natural/nLabels", (mrs_natural)l.getNumLabels());
		total->updctrl("WekaSink/wsink/mrs_natural/downsample", 1);
		total->updctrl("WekaSink/wsink/mrs_string/labelNames", l.getLabelNames());
		total->updctrl("WekaSink/wsink/mrs_string/filename", wekafname);

		for (i=0; i < l.size(); i++)
		{
			total->updctrl("Accumulator/acc/Series/playbacknet/SoundFileSource/src/mrs_string/filename", l.entry(i));
			total->updctrl("Annotator/ann/mrs_natural/label", l.labelNum(l.labelEntry(i)));
			cout << "Processing" << l.entry(i) << endl;
			total->tick();
		}
	}
}

void bextract_trainAccumulator(vector<Collection> cls, mrs_natural label,
															 string pluginName, string classNames,
															 string wekafname,
															 mrs_natural memSize, string extractorStr,
															 bool withBeatFeatures)
{
	if (withBeatFeatures)
		cout << "with beat features" << endl;


	mrs_bool collection_has_labels = false;

	if ((cls.size() == 1)&&(cls[0].hasLabels()))
	{
		collection_has_labels = true;
	}

	MRSDIAG("bextract.cpp - bextract_trainAccumulator");
	mrs_natural i;
	mrs_natural cj;

	Collection linitial = cls[0];
	string sfName = linitial.entry(0);

	if (normopt)
		cout << "NORMALIZE ENABLED" << endl;

	MarSystemManager mng;


	////////////////////////////////////////////////
	//////////////////////////
	// create the file source
	//////////////////////////////////////////////////////////////////////////
	MarSystem* src = mng.create("SoundFileSource", "src");

	//////////////////////////////////////////////////////////////////////////
	// Feature Extractor
	//////////////////////////////////////////////////////////////////////////
	// create the correct feature extractor using the table of known
	// feature extractors:

	MarSystem* featExtractor = (*featExtractors[extractorStr])();
	featExtractor->updctrl("mrs_natural/winSize", winSize);

	//////////////////////////////////////////////////////////////////////////
	// Build the overall feature calculation network
	//////////////////////////////////////////////////////////////////////////
	MarSystem* featureNetwork = mng.create("Series", "featureNetwork");
	featureNetwork->addMarSystem(src);

	// convert stereo files to mono
	featureNetwork->addMarSystem(mng.create("Stereo2Mono", "s2m"));
	featureNetwork->addMarSystem(featExtractor);

	//////////////////////////////////////////////////////////////////////////
	// Texture Window Statistics (if any)
	//////////////////////////////////////////////////////////////////////////
	if(memSize != 0)
	{
		featureNetwork->addMarSystem(mng.create("TextureStats", "tStats"));
		featureNetwork->updctrl("TextureStats/tStats/mrs_natural/memSize", memSize);
	}

	//////////////////////////////////////////////////////////////////////////
	// update controls
	//////////////////////////////////////////////////////////////////////////
	featureNetwork->updctrl("SoundFileSource/src/mrs_string/filename", sfName);
	featureNetwork->updctrl("mrs_natural/inSamples", MRS_DEFAULT_SLICE_NSAMPLES);

	//////////////////////////////////////////////////////////////////////////
	// accumulate feature vectors over 30 seconds
	//////////////////////////////////////////////////////////////////////////
	MarSystem* acc = mng.create("Accumulator", "acc");
	acc->updctrl("mrs_natural/nTimes", accSize_);

	//////////////////////////////////////////////////////////////////////////
	// add network to accumulator
	//////////////////////////////////////////////////////////////////////////
	acc->addMarSystem(featureNetwork->clone());

	//////////////////////////////////////////////////////////////////////////
	// WEKA output
	//////////////////////////////////////////////////////////////////////////
	MarSystem* wsink = mng.create("WekaSink", "wsink");

	//////////////////////////////////////////////////////////////////////////
	// Annotator
	//////////////////////////////////////////////////////////////////////////
	MarSystem* annotator = mng.create("Annotator", "annotator");

	//////////////////////////////////////////////////////////////////////////
	// 30-second statistics
	//////////////////////////////////////////////////////////////////////////
	MarSystem* statistics = mng.create("Fanout", "statistics2");
	statistics->addMarSystem(mng.create("Mean", "mn"));
	statistics->addMarSystem(mng.create("StandardDeviation", "std"));

	//////////////////////////////////////////////////////////////////////////
	// Final network compute 30-second statistics
	//////////////////////////////////////////////////////////////////////////
	MarSystem* total = mng.create("Series", "total");
	total->addMarSystem(acc);
	total->addMarSystem(statistics);

	// get parameters
	mrs_real srate = samplingRate_;

	total->updctrl("mrs_natural/inSamples", winSize);
	total->updctrl("Accumulator/acc/Series/featureNetwork/" + src->getType() + "/src/mrs_natural/pos", offset);

	// Calculate duration, offset parameters if necessary
	offset = (mrs_natural) (start * samplingRate_ );
	duration = (mrs_natural) (length * samplingRate_);

	//////////////////////////////////////////////////////////////////////////
	// main loop for extracting the features
	//////////////////////////////////////////////////////////////////////////
	mrs_natural wc = 0;
	mrs_natural samplesPlayed =0;
	string className = "";
	realvec beatfeatures;
	beatfeatures.create((mrs_natural)8,(mrs_natural)1);
	realvec estimate;
	estimate.create((mrs_natural)8,(mrs_natural)1);
	realvec in;
	realvec timbreres;
	realvec fullres;
	realvec afullres;

	in.create(total->getctrl("mrs_natural/inObservations")->to<mrs_natural>(),
		total->getctrl("mrs_natural/inSamples")->to<mrs_natural>());
	timbreres.create(total->getctrl("mrs_natural/onObservations")->to<mrs_natural>(),
		total->getctrl("mrs_natural/onSamples")->to<mrs_natural>());

	if (withBeatFeatures)
	{
		if (extractorStr == "BEAT")
		{
			fullres.create(8, 1);
			afullres.create(8+1, 1);
			annotator->updctrl("mrs_natural/inObservations", 8);
		}
		else
		{
			fullres.create(total->getctrl("mrs_natural/onObservations")->to<mrs_natural>() + 8,
				total->getctrl("mrs_natural/onSamples")->to<mrs_natural>());
			afullres.create(total->getctrl("mrs_natural/onObservations")->to<mrs_natural>() + 8 + 1,
				total->getctrl("mrs_natural/onSamples")->to<mrs_natural>());
			annotator->updctrl("mrs_natural/inObservations", total->getctrl("mrs_natural/onObservations")->to<mrs_natural>()+8);
		}
	}
	else
	{
		fullres.create(total->getctrl("mrs_natural/onObservations")->to<mrs_natural>(),
			total->getctrl("mrs_natural/onSamples")->to<mrs_natural>());

		afullres.create(total->getctrl("mrs_natural/onObservations")->to<mrs_natural>() + 1,
			total->getctrl("mrs_natural/onSamples")->to<mrs_natural>());
		annotator->updctrl("mrs_natural/inObservations", total->getctrl("mrs_natural/onObservations")->to<mrs_natural>());
	}

	annotator->updctrl("mrs_natural/inSamples", total->getctrl("mrs_natural/onSamples"));
	annotator->updctrl("mrs_real/israte", total->getctrl("mrs_real/israte"));

	MarSystem* gcl = mng.create("SVMClassifier" ,"gcl");

	if (wekafname != EMPTYSTRING)
	{
		wsink->updctrl("mrs_natural/inSamples", annotator->getctrl("mrs_natural/onSamples"));
		wsink->updctrl("mrs_natural/inObservations", annotator->getctrl("mrs_natural/onObservations")->to<mrs_natural>());
		wsink->updctrl("mrs_real/israte", annotator->getctrl("mrs_real/israte"));
	}

	gcl->updctrl("mrs_natural/inSamples", annotator->getctrl("mrs_natural/onSamples"));
	gcl->updctrl("mrs_natural/inObservations", annotator->getctrl("mrs_natural/onObservations")->to<mrs_natural>());
	gcl->updctrl("mrs_real/israte", annotator->getctrl("mrs_real/israte"));

	realvec gclres;
	gclres.create(gcl->getctrl("mrs_natural/onObservations")->to<mrs_natural>(),
		gcl->getctrl("mrs_natural/onSamples")->to<mrs_natural>());

	mrs_natural timbreSize = total->getctrl("mrs_natural/onObservations")->to<mrs_natural>();
	mrs_natural beatSize = 8;

	MarSystem *total1 = NULL;

	if (withBeatFeatures)
	{
		if (extractorStr == "BEAT")
		{
			total1 = createBEATextrator();
			annotator->updctrl("mrs_string/inObsNames", total1->getctrl("mrs_string/onObsNames")->to<mrs_string>());
		}
		else
		{
			total1 = createBEATextrator();
			annotator->updctrl("mrs_string/inObsNames", total->getctrl("mrs_string/onObsNames")->to<mrs_string>() + total1->getctrl("mrs_string/onObsNames")->to<mrs_string>());
		}
	}
	else
	{
		annotator->updctrl("mrs_string/inObsNames", total->getctrl("mrs_string/onObsNames"));
	}
	if (wekafname != EMPTYSTRING)
		wsink->updctrl("mrs_string/inObsNames", annotator->getctrl("mrs_string/onObsNames"));

	realvec iwin;

	//iterate over collections
	Collection m,l;
	if (!collection_has_labels)
	{
		if (wekafname != EMPTYSTRING)
		{
			wsink->updctrl("mrs_string/labelNames",classNames);
			wsink->updctrl("mrs_natural/nLabels", (mrs_natural)cls.size());
			wsink->updctrl("mrs_string/filename", wekafname);
		}

		for (cj=0; cj < (mrs_natural)cls.size(); cj++)
		{
			Collection l = cls[cj];
			if (wekafname != EMPTYSTRING)
			{
				if (workspaceDir != EMPTYSTRING)
					wekafname = workspaceDir + wekafname;
				wsink->updctrl("mrs_string/filename", wekafname);
				cout << "Writing weka .arff file to :" << wekafname << endl;
			}
			for (i=0; i < l.size(); i++)//iterate over collection files
			{
				// cout << beatfeatures << endl;
				total->updctrl("Accumulator/acc/Series/featureNetwork/SoundFileSource/src/mrs_string/filename", l.entry(i));
				if (withBeatFeatures)
				{
					srate = total->getctrl("Accumulator/acc/Series/featureNetwork/SoundFileSource/src/mrs_real/osrate")->to<mrs_real>();
					iwin.create((mrs_natural)1, (mrs_natural)(((srate / 22050.0) * 2 * 65536) / 16)); // [!] hardcoded!

					tempo_histoSumBands(total1, l.entry(i), beatfeatures,
						iwin, estimate);
				}
				total->updctrl("Accumulator/acc/Series/featureNetwork/SoundFileSource/src/mrs_natural/pos", offset);
				wc = 0;
				samplesPlayed = 0;
				// total->updctrl("WekaSink/wsink/mrs_natural/label", cj);
				annotator->updctrl("mrs_natural/label", cj);
				// wsink->updctrl("mrs_natural/label", cj);
				if (extractorStr != "BEAT")
				{
					total->process(in, timbreres);
					for (int t=0; t < timbreSize; t++)
						fullres(t,0) = timbreres(t,0);
				}

				if (withBeatFeatures)
				{
					if (extractorStr == "BEAT")
					{
						for (int t=0; t < beatSize; t++)
							fullres(t, 0) = beatfeatures(t,0);
					}
					else
					{
						for (int t=0; t < beatSize; t++)
							fullres(t+timbreSize, 0) = beatfeatures(t,0);
					}
				}
				annotator->process(fullres, afullres);
				if (wekafname != EMPTYSTRING)
					wsink->process(afullres, afullres);
				cerr << "Processed " << l.entry(i) << endl;
			}
		}
	}
	else
	{
		l = cls[0];
		if (workspaceDir != EMPTYSTRING)
		{
			string outCollection = workspaceDir + "extract.txt";
			l.write(outCollection);
			cout << "Writing extract collection to :" << outCollection << endl;
		}
		if (wekafname != EMPTYSTRING)
		{
			wsink->updctrl("mrs_string/labelNames",l.getLabelNames());
			wsink->updctrl("mrs_natural/nLabels", (mrs_natural)l.getNumLabels());
			if (workspaceDir != EMPTYSTRING)
				wekafname = workspaceDir + wekafname;
			wsink->updctrl("mrs_string/filename", wekafname);
			cout << "Writing weka .arff file to :" << wekafname << endl;
		}

		// gcl->updctrl("mrs_natural/nLabels", (mrs_natural)l.getNumLabels());
		gcl->updctrl("mrs_string/mode", "train");

		for (i=0; i < l.size(); i++)//iterate over collection files
		{
			// cout << beatfeatures << endl;
			total->updctrl("Accumulator/acc/Series/featureNetwork/SoundFileSource/src/mrs_string/filename", l.entry(i));
			if (withBeatFeatures)
			{
				srate = total->getctrl("Accumulator/acc/Series/featureNetwork/SoundFileSource/src/mrs_real/osrate")->to<mrs_real>();

				iwin.create((mrs_natural)1, (mrs_natural)(((srate / 22050.0) * 2 * 65536) / 16)); // [!] hardcoded!
				tempo_histoSumBands(total1, l.entry(i), beatfeatures,
					iwin, estimate);
			}
			total->updctrl("Accumulator/acc/Series/featureNetwork/SoundFileSource/src/mrs_natural/pos", offset);
			wc = 0;
			samplesPlayed = 0;
			annotator->updctrl("mrs_natural/label", l.labelNum(l.labelEntry(i)));

			if (extractorStr != "BEAT")
			{
				total->process(in, timbreres);

				// concatenate timbre and beat vectors
				for (int t=0; t < timbreSize; t++)
					fullres(t,0) = timbreres(t,0);
			}
			if (withBeatFeatures)
			{
				if (extractorStr == "BEAT")
				{
					for (int t=0; t < beatSize; t++)
						fullres(t, 0) = beatfeatures(t,0);
				}
				else
				{
					for (int t=0; t < beatSize; t++)
						fullres(t+timbreSize, 0) = beatfeatures(t,0);
				}
			}
			annotator->process(fullres, afullres);
			if (wekafname != EMPTYSTRING)
				wsink->process(afullres, afullres);
			gcl->process(afullres, gclres);
			cerr << "Processed " << l.entry(i) << endl;
		}
	}

	if (testCollection != EMPTYSTRING)
	{
		m.read(testCollection);
		if (wekafname != EMPTYSTRING)
			wsink->updctrl("mrs_string/filename", "predict.arff");
		gcl->updctrl("mrs_string/mode", "predict");

		ofstream prout;
		prout.open(predictCollection.c_str());

		for (i=0; i < m.size(); i++)//iterate over collection files
		{
			// cout << beatfeatures << endl;
			total->updctrl("Accumulator/acc/Series/featureNetwork/SoundFileSource/src/mrs_string/filename", m.entry(i));
			if (withBeatFeatures)
			{
				srate = total->getctrl("Accumulator/acc/Series/featureNetwork/SoundFileSource/src/mrs_real/osrate")->to<mrs_real>();
				iwin.create((mrs_natural)1, (mrs_natural)(((srate / 22050.0) * 2 * 65536) / 16)); // [!] hardcoded!
				tempo_histoSumBands(total1, m.entry(i), beatfeatures,
					iwin, estimate);
			}
			total->updctrl("Accumulator/acc/Series/featureNetwork/SoundFileSource/src/mrs_natural/pos", offset);
			wc = 0;
			samplesPlayed = 0;
			annotator->updctrl("mrs_natural/label", 0);

			if (extractorStr != "BEAT")
			{
				total->process(in, timbreres);

				// concatenate timbre and beat vectors
				for (int t=0; t < timbreSize; t++)
					fullres(t,0) = timbreres(t,0);
			}
			if (withBeatFeatures)
			{
				if (extractorStr == "BEAT")
				{
					for (int t=0; t < beatSize; t++)
						fullres(t, 0) = beatfeatures(t,0);
				}
				else
				{
					for (int t=0; t < beatSize; t++)
						fullres(t+timbreSize, 0) = beatfeatures(t,0);
				}
			}
			annotator->process(fullres, afullres);
			if (wekafname != EMPTYSTRING)
				wsink->process(afullres, afullres);
			gcl->process(afullres, gclres);
			cout << "Predicting " << m.entry(i) << "\t" << l.labelName((mrs_natural)gclres(0,0)) << endl;
			prout << m.entry(i) << "\t" << l.labelName((mrs_natural)gclres(0,0)) << endl;
		}
	}

	delete featureNetwork;
}


void
bextract_train(vector<Collection> cls, Collection cl,
							 mrs_natural label,
							 string pluginName, string classNames,
							 string wekafname,  mrs_natural memSize,
							 string extractorStr,
							 string classifierName)
{
	MRSDIAG("bextract.cpp - bextract_train");

	cout << "Old bextract_train" << endl;

	// hack for backward compatibility
	// this function is being depracated
	// use instead bextract_train_refactored
	if (length == -1.0f)
		length = 30.0f;

	MarSystemManager mng;
	vector<TimeLine> timeLines;
	if (classifierName == EMPTYSTRING)
		classifierName = DEFAULT_CLASSIFIER;

	if (extractorStr == EMPTYSTRING)
		extractorStr = DEFAULT_EXTRACTOR;

	//////////////////////////////////////////////////////////////////////////
	// Find proper sound file format and create SignalSource
	//////////////////////////////////////////////////////////////////////////
	Collection linitial = cls[0];
	string sfName = linitial.entry(0);

	MarSystem *src = mng.create("SoundFileSource", "src");
	src->updctrl("mrs_string/filename", sfName);
	// src->updctrl("mrs_natural/inSamples", MRS_DEFAULT_SLICE_NSAMPLES);
	// src->updctrl("mrs_natural/inSamples", 2048);

	// Calculate duration, offset parameters if necessary
	if (start > 0.0)
		offset = (mrs_natural) (start
		* src->getctrl("mrs_real/israte")->to<mrs_real>()
		* src->getctrl("mrs_natural/onObservations")->to<mrs_natural>());

	duration = (mrs_natural) (length
		* src->getctrl("mrs_real/israte")->to<mrs_real>()
		* src->getctrl("mrs_natural/onObservations")->to<mrs_natural>());

	cout << "duration = " << duration << endl;
	cout << "offset = " << offset << endl;

	//////////////////////////////////////////////////////////////////////////
	// Feature Extractor
	//////////////////////////////////////////////////////////////////////////
	// create the correct feature extractor using the table of known
	// feature extractors:
	MarSystem* featExtractor = (*featExtractors[extractorStr])();

	featExtractor->updctrl("mrs_natural/winSize", winSize);

	//////////////////////////////////////////////////////////////////////////
	// Build the overall feature calculation network
	//////////////////////////////////////////////////////////////////////////
	MarSystem* featureNetwork = mng.create("Series", "featureNetwork");
	featureNetwork->addMarSystem(src);

	if (pluginName != EMPTYSTRING)
	{
		// create audio sink and mute it
		// it is stored in the output plugin
		// which can be used for real-time classification
		MarSystem* dest = mng.create("AudioSink", "dest");
		dest->updctrl("mrs_bool/mute", true);
		featureNetwork->addMarSystem(dest);
	}

	//add the feature extraction network
	featureNetwork->addMarSystem(featExtractor);//mng.create("FeatureExtractor", "featExtractor"));

	//////////////////////////////////////////////////////////////////////////
	//texture window statistics (optional)
	//////////////////////////////////////////////////////////////////////////
	if(memSize != 0)
	{
		featureNetwork->addMarSystem(mng.create("TextureStats", "tStats"));
		featureNetwork->updctrl("TextureStats/tStats/mrs_natural/memSize", memSize);
	}

	//////////////////////////////////////////////////////////////////////////
	// update controls I
	//////////////////////////////////////////////////////////////////////////
	// src has to be configured with hopSize frame length in case a ShiftInput
	// is used in the feature extraction network
	featureNetwork->updctrl("mrs_natural/inSamples", hopSize);
	featureNetwork->updctrl(src->getType() + "/src/mrs_natural/pos", offset);

	//////////////////////////////////////////////////////////////////////////
	// add the Annotator
	//////////////////////////////////////////////////////////////////////////
	featureNetwork->addMarSystem(mng.create("Annotator", "annotator"));

	//////////////////////////////////////////////////////////////////////////
	// add WEKA sink
	//////////////////////////////////////////////////////////////////////////
	if (wekafname != EMPTYSTRING)
		featureNetwork->addMarSystem(mng.create("WekaSink", "wsink"));

	//////////////////////////////////////////////////////////////////////////
	// add classifier and confidence majority calculation
	//////////////////////////////////////////////////////////////////////////
	cout << "classifierName = " << classifierName << endl;
	if (classifierName == "GS")
		featureNetwork->addMarSystem(mng.create("GaussianClassifier", "gaussian"));
	else if (classifierName == "ZeroR")
		featureNetwork->addMarSystem(mng.create("ZeroRClassifier", "zeror"));
	else if (classifierName == "KNN")
		featureNetwork->addMarSystem(mng.create("KNNClassifier", "knn"));
	else
	{
		cerr << "Unsuported classifier : " << classifierName << endl;
		return;
	}

	featureNetwork->addMarSystem(mng.create("Confidence", "confidence"));

	// FileName Sfname(sfName);
	// Sfname.path()+
	//   featureNetwork->updctrl("Confidence/confidence/mrs_string/fileName", Sfname.nameNoExt());

	//////////////////////////////////////////////////////////////////////////
	// link controls
	//////////////////////////////////////////////////////////////////////////
	featureNetwork->linkctrl("mrs_string/filename", "SoundFileSource/src/mrs_string/filename");
	featureNetwork->linkctrl("SoundFileSource/src/mrs_string/currentlyPlaying", "Confidence/confidence/mrs_string/fileName");
	featureNetwork->linkctrl("mrs_real/israte", "SoundFileSource/src/mrs_real/israte");
	featureNetwork->linkctrl("mrs_natural/pos", "SoundFileSource/src/mrs_natural/pos");

	featureNetwork->linkctrl("mrs_bool/notEmpty", "SoundFileSource/src/mrs_bool/notEmpty");
	featureNetwork->linkctrl("mrs_bool/initAudio", "AudioSink/dest/mrs_bool/initAudio");

	MarControlPtr ctrl_filename_ = featureNetwork->getctrl("SoundFileSource/src/mrs_string/filename");
	MarControlPtr ctrl_notEmpty_ = featureNetwork->getctrl("SoundFileSource/src/mrs_bool/notEmpty");

	//////////////////////////////////////////////////////////////////////////
	// main loop for extracting features
	//////////////////////////////////////////////////////////////////////////
	//***********************************
	// if no timelines are being used...
	//***********************************
	if(!tline)
	{
		mrs_natural wc = 0;
		mrs_natural samplesPlayed =0;
		mrs_natural onSamples = featureNetwork->getctrl("mrs_natural/onSamples")->to<mrs_natural>();

		if (classifierName == "GS")
			featureNetwork->updctrl("GaussianClassifier/gaussian/mrs_natural/nClasses", (mrs_natural)cls.size());
		else if (classifierName == "ZeroR")
			featureNetwork->updctrl("ZeroRClassifier/zeror/mrs_natural/nClasses", (mrs_natural)cls.size());
		else if (classifierName == "KNN")
			featureNetwork->updctrl("KNNClassifier/knn/mrs_natural/nLabels", (mrs_natural)cls.size());

		//configure Confidence
		featureNetwork->updctrl("Confidence/confidence/mrs_natural/nLabels", (int)cls.size());
		featureNetwork->updctrl("Confidence/confidence/mrs_bool/mute", true);
		featureNetwork->updctrl("Confidence/confidence/mrs_string/labelNames",classNames);
		featureNetwork->updctrl("Confidence/confidence/mrs_bool/print",true);

		Collection l = cl;
		mrs_natural nLabels = l.getNumLabels();

		if (wekafname != EMPTYSTRING)
		{
			featureNetwork->updctrl("WekaSink/wsink/mrs_string/labelNames", l.getLabelNames());
			featureNetwork->updctrl("WekaSink/wsink/mrs_natural/nLabels", nLabels);
			featureNetwork->updctrl("WekaSink/wsink/mrs_natural/downsample", 1);
			featureNetwork->updctrl("WekaSink/wsink/mrs_string/filename", wekafname);
		}

		if (classifierName == "GS")
			featureNetwork->updctrl("GaussianClassifier/gaussian/mrs_natural/nClasses", nLabels);
		else if (classifierName == "ZeroR")
			featureNetwork->updctrl("ZeroRClassifier/zeror/mrs_natural/nClasses", nLabels);
		else if (classifierName == "KNN")
			featureNetwork->updctrl("KNNClassifier/knn/mrs_natural/nLabels", nLabels);

		//configure Confidence
		featureNetwork->updctrl("Confidence/confidence/mrs_natural/nLabels", nLabels);
		featureNetwork->updctrl("Confidence/confidence/mrs_bool/mute", true);
		featureNetwork->updctrl("Confidence/confidence/mrs_string/labelNames", l.getLabelNames());
		featureNetwork->updctrl("Confidence/confidence/mrs_bool/print",true);

		//iterate over audio files, extract features and label
		for (mrs_natural i=0; i < l.size(); i++)
		{
			//reset texture analysis stats between files
			// if(memSize != 0)
			// featureNetwork->updctrl("TextureStats/tStats/mrs_bool/reset", true);

			featureNetwork->updctrl("Annotator/annotator/mrs_natural/label", l.labelNum(l.labelEntry(i)));

			featureNetwork->updctrl(ctrl_filename_, l.entry(i));
			wc = 0;
			samplesPlayed = 0;
			while (ctrl_notEmpty_->to<mrs_bool>() && (duration > samplesPlayed))
			{
				featureNetwork->tick();
				wc++;
				samplesPlayed = wc * onSamples;
			}
			featureNetwork->tick();
			cout << "Processed " << l.entry(i) << endl;
		}
	}

	//**********************
	// if using timelines
	//**********************
	else
	{
		TimeLine tline;
		mrs_natural numClasses;
		//1st collection is the audio file collection
		Collection audioColl = cls[0];
		//2nd collection  is a collection of timeline files
		//(should be in the same order as the audio files)
		Collection tlColl = cls[1];

		// for each audiofile/timeline pair in collection
		// (and ignore all audio files that have no timeline file)
		// run feature extraction and train classifiers
		for(mrs_natural i=0; i <(mrs_natural)tlColl.size(); ++i)
		{
			//load Audio file from the collection
			featureNetwork->updctrl("SoundFileSource/src/mrs_string/filename", audioColl.entry(i));

			//load timeline for i-th audio file
			tline.load(tlColl.entry(i));

			//get number of classes in the timeline
			numClasses = (mrs_natural)tline.numClasses();

			// create a label for each class
			classNames = "";
			ostringstream sstr;
			for(mrs_natural c=0; c < numClasses; c++)
				sstr << "class_" << c << ",";
			classNames = sstr.str();

			//configure classifiers
			if (classifierName == "GS")
				featureNetwork->updctrl("GaussianClassifier/gaussian/mrs_natural/nLabels", numClasses);
			else if (classifierName == "ZeroR")
				featureNetwork->updctrl("ZeroRClassifier/zeror/mrs_natural/nLabels", numClasses);
			else if (classifierName == "KNN")
				featureNetwork->updctrl("KNNClassifier/knn/mrs_natural/nLabels", numClasses);

			//configure Confidence
			featureNetwork->updctrl("Confidence/confidence/mrs_natural/nLabels", numClasses);
			featureNetwork->updctrl("Confidence/confidence/mrs_bool/mute", true);
			featureNetwork->updctrl("Confidence/confidence/mrs_string/labelNames",classNames);
			featureNetwork->updctrl("Confidence/confidence/mrs_bool/print",true);

			//configure WEKA sink
			if (wekafname != EMPTYSTRING)
			{
				featureNetwork->updctrl("WekaSink/wsink/mrs_natural/nLabels", numClasses);
				featureNetwork->updctrl("WekaSink/wsink/mrs_string/labelNames",classNames);
				featureNetwork->updctrl("WekaSink/wsink/mrs_natural/downsample", 1);
				string name = audioColl.entry(i).substr(0, audioColl.entry(i).length()-4) + "_" + extractorStr + ".arff";
				featureNetwork->updctrl("WekaSink/wsink/mrs_string/filename", name);
			}

			cout << endl;
			cout << "**************************************" << endl;
			cout << "AudioFile: " << audioColl.entry(i) << endl;
			cout << "TimeLine : " << tlColl.entry(i) << endl;
			cout << "**************************************" << endl;

			//iterate over timeline regions
			for (mrs_natural r = 0; r < tline.numRegions(); r++)
			{
				cout << "-----------------------------------------------" << endl;
				cout << "Region " << r+1 << "/" << tline.numRegions() << endl;
				cout << "Region start   = " << tline.regionStart(r) << endl;
				cout << "Region classID = " << tline.regionClass(r) << endl;
				cout << "Region end     = " << tline.regionEnd(r) << endl;

				// set current region class in Annotator
				featureNetwork->updctrl("Annotator/annotator/mrs_natural/label", tline.regionClass(r));

				// set current region class in WEKA sink
				if (wekafname != EMPTYSTRING)
				{
					featureNetwork->updctrl("WekaSink/wsink/mrs_natural/label", tline.regionClass(r)); //[?]
				}

				// reset texture analysis stats between regions
				if(memSize != 0)
					featureNetwork->updctrl("TextureStats/tStats/mrs_bool/reset", true);

				//define audio region boundaries
				mrs_natural start = (mrs_natural)(tline.regionStart(r) * tline.lineSize()); //region start sample
				mrs_natural end = (mrs_natural)(tline.regionEnd(r) * tline.lineSize()); //region end sample

				mrs_natural fileSize = featureNetwork->getctrl(src->getType() + "/src/mrs_natural/size")->to<mrs_natural>();
				if(end > fileSize)
				{
					end = fileSize;
					cout << "WARNING:" << endl;
					cout << "Region end sample behind EOF!" << endl;
					cout << "Setting region end sample to EOF." << endl;
				}

				featureNetwork->updctrl(src->getType() + "/src/mrs_natural/pos", start);
				//featureNetwork->updctrl("mrs_natural/inSamples", hopSize); //[?]
				//featureNetwork->updctrl("mrs_natural/inSamples", tline.lineSize_);//[?]

				//iterate over audio region
				//(using the winSize and hopSize defined by the user)
				mrs_natural numWindows = 0;
				while(featureNetwork->getctrl(src->getType() + "/src/mrs_natural/pos")->to<mrs_natural>() + winSize <= end)
				{
					//cout << "pos = " << featureNetwork->getctrl(src->getType() + "/src/mrs_natural/pos")->to<mrs_natural>() << endl;
					featureNetwork->tick();
					numWindows++;
					cout << '.';
				}
				cout << endl;

				mrs_natural lastpos = featureNetwork->getctrl(src->getType() + "/src/mrs_natural/pos")->to<mrs_natural>();
				if((end - lastpos) > 0)
				{
					cout << "Dropped last frame: " << endl;
					cout << "Dropped " << end - lastpos << " samples from current segment." << endl;
					cout << "Dropped " <<(lastpos + winSize) - end << " samples from next segment." << endl;
				}

				cout << "Processed " << numWindows << " frames!"  << endl;
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// prepare network for classification
	//////////////////////////////////////////////////////////////////////////
	if (classifierName == "GS")
	{
		featureNetwork->updctrl("GaussianClassifier/gaussian/mrs_string/mode","predict");
	}
	else if (classifierName == "ZeroR")
	{
		featureNetwork->updctrl("ZeroRClassifier/zeror/mrs_string/mode","predict") ;
	}
	else if (classifierName == "KNN")
	{
		featureNetwork->updctrl("KNNClassifier/knn/mrs_string/mode","predict");
		featureNetwork->updctrl("KNNClassifier/knn/mrs_natural/k",3); //[!] hardcoded!!!
	}

	featureNetwork->tick();

	if (pluginName != EMPTYSTRING && !pluginMute)
	{
		featureNetwork->updctrl("AudioSink/dest/mrs_bool/mute", false);
		featureNetwork->updctrl("AudioSink/dest/mrs_bool/initAudio", true);//[!][?] this still does not solves the problem of sfplugin being unable to play audio...
	}

	if (wekafname != EMPTYSTRING)
		featureNetwork->updctrl("WekaSink/wsink/mrs_bool/mute", true);

	featureNetwork->updctrl("Confidence/confidence/mrs_bool/mute", false);

	//////////////////////////////////////////////////////////////////////////
	// output trained classifier models
	//////////////////////////////////////////////////////////////////////////
	if (pluginName == EMPTYSTRING) // output to stdout
		cout << (*featureNetwork) << endl;
	else // save to .mpl file
	{
		ofstream oss(pluginName.c_str());
		oss << (*featureNetwork) << endl;
	}

	delete featureNetwork;
}

void
selectClassifier(MarSystem *msys,string classifierName )
{
	cout << "classifierName = " << classifierName << endl;

	if (classifierName == "GS")
		msys->updctrl("Classifier/cl/mrs_string/enableChild", "GaussianClassifier/gaussiancl");
	if (classifierName == "ZEROR")
	{
		cout << "Selecting ZEROR" << endl;
		msys->updctrl("Classifier/cl/mrs_string/enableChild", "ZeroRClassifier/zerorcl");
		cout << *msys << endl;
	}
	if (classifierName == "SVM")
		msys->updctrl("Classifier/cl/mrs_string/enableChild", "SVMClassifier/svmcl");
}

void
selectFeatureSet(MarSystem *featExtractor)
{
	if (chroma_)
		featExtractor->updctrl("mrs_string/enableSPChild", "Series/chromaPrSeries");
	if (mfcc_)
		featExtractor->updctrl("mrs_string/enableSPChild", "MFCC/mfcc");
	if (sfm_)
		featExtractor->updctrl("mrs_string/enableSPChild", "SFM/sfm");
	if (scf_)
		featExtractor->updctrl("mrs_string/enableSPChild", "SCF/scf");
	if (rlf_)
		featExtractor->updctrl("mrs_string/enableSPChild", "Rolloff/rlf");
	if (flx_)
		featExtractor->updctrl("mrs_string/enableSPChild", "Flux/flux");
	if (lsp_)
		featExtractor->updctrl("mrs_string/enableLPCChild", "Series/lspbranch");
	if (lpcc_)
		featExtractor->updctrl("mrs_string/enableLPCChild", "Series/lpccbranch");
	if (ctd_)
		featExtractor->updctrl("mrs_string/enableSPChild", "Centroid/cntrd");
	if (zcrs_)
		featExtractor->updctrl("mrs_string/enableTDChild", "ZeroCrossings/zcrs");
	if (spectralFeatures_)
	{
		featExtractor->updctrl("mrs_string/enableSPChild", "Centroid/cntrd");
		featExtractor->updctrl("mrs_string/enableSPChild", "Flux/flux");
		featExtractor->updctrl("mrs_string/enableSPChild", "Rolloff/rlf");
	}
	if (timbralFeatures_)
	{
		featExtractor->updctrl("mrs_string/enableTDChild", "ZeroCrossings/zcrs");
		
		featExtractor->updctrl("mrs_string/enableSPChild", "MFCC/mfcc");
		featExtractor->updctrl("mrs_string/enableSPChild", "Centroid/cntrd");
		featExtractor->updctrl("mrs_string/enableSPChild", "Flux/flux");
		featExtractor->updctrl("mrs_string/enableSPChild", "Rolloff/rlf");

		featExtractor->updctrl("mrs_string/enableSPChild", "Series/chromaPrSeries");
	}
}


void
bextract_train_refactored(string pluginName,  string wekafname,
						  mrs_natural memSize, string classifierName,
						  mrs_bool single_vector)
{
	MRSDIAG("bextract.cpp - bextract_train_refactored");
	cout << "BEXTRACT REFACTORED" << endl;
	MarSystemManager mng; 


	// Overall extraction and classification network
	MarSystem* bextractNetwork = mng.create("Series", "bextractNetwork");


	// Build the overall feature calculation network
	MarSystem* featureNetwork = mng.create("Series", "featureNetwork");

	// Add a fanout for sound file and audio source ...
	MarSystem *fanout = mng.create("Fanout", "fanout"); 

	// Add a sound file source (which can also read collections)
	MarSystem *src = mng.create("SoundFileSource", "src");
	fanout->addMarSystem(src);
	
	// Add a live audio source for realtime classification 
	MarSystem *mic = mng.create("AudioSource", "mic"); 
	mic->updctrl("mrs_natural/nChannels", 1);	//stereo
	fanout->addMarSystem(mic);
	
	// Add the fanout to our feature Network ... 
	featureNetwork->addMarSystem(fanout);
	
	featureNetwork->updctrl("mrs_real/israte", 44100.0);   //sampling rate  [!hardcoded]
	
	// Disable Microphone for training the classifier ... 
	featureNetwork->updctrl("Fanout/fanout/mrs_natural/disable", 1);
	//featureNetwork->updctrl("Fanout/fanout/AudioSource/mic/mrs_bool/initAudio", false); 

	// Add a TimelineLabeler, if necessary
	if(tline)
	{
		featureNetwork->addMarSystem(mng.create("TimelineLabeler", "timelineLabeler"));
	}

	// create audio sink and mute it it is stored in the output plugin
	// that can be used for real-time classification
	if (pluginName != EMPTYSTRING)
	{
		MarSystem* dest = mng.create("AudioSink", "dest");
		dest->updctrl("mrs_bool/mute", true);
		featureNetwork->addMarSystem(dest);
	}



	// Select whether stereo or mono feature extraction is to be used
	if (stereo_ == true)
	{
		MarSystem* stereoFeatures = mng.create("StereoFeatures", "stereoFeatures");
		selectFeatureSet(stereoFeatures);
		featureNetwork->addMarSystem(stereoFeatures);
	}
	else
	{
		featureNetwork->addMarSystem(mng.create("Stereo2Mono", "m2s"));
		MarSystem* featExtractor = mng.create("TimbreFeatures", "featExtractor");
		selectFeatureSet(featExtractor);
		featureNetwork->addMarSystem(featExtractor);
	}

	// texture statistics
	featureNetwork->addMarSystem(mng.create("TextureStats", "tStats"));
	featureNetwork->updctrl("TextureStats/tStats/mrs_natural/memSize", memSize);
	featureNetwork->updctrl("TextureStats/tStats/mrs_bool/reset", true);




	// Use accumulator if computing single vector / file
	if (single_vector)
	{
		cout << "accSize_ = " << accSize_ << endl;

		MarSystem* acc = mng.create("Accumulator", "acc");
		acc->updctrl("mrs_natural/nTimes", accSize_);
		acc->addMarSystem(featureNetwork);
		bextractNetwork->addMarSystem(acc);
		MarSystem* song_statistics = mng.create("Fanout", "song_statistics");
		song_statistics->addMarSystem(mng.create("Mean", "mn"));
		song_statistics->addMarSystem(mng.create("StandardDeviation", "std"));
		bextractNetwork->addMarSystem(song_statistics);




		bextractNetwork->linkctrl("Accumulator/acc/Series/featureNetwork/Fanout/fanout/SoundFileSource/src/mrs_string/filename",
			"mrs_string/filename"); // added Fanout ... 
		bextractNetwork->linkctrl("mrs_bool/notEmpty",
			"Accumulator/acc/Series/featureNetwork/Fanout/fanout/SoundFileSource/src/mrs_bool/notEmpty"); // added Fanout ... 
		bextractNetwork->linkctrl("mrs_natural/pos",
			"Accumulator/acc/Series/featureNetwork/Fanout/fanout/SoundFileSource/src/mrs_natural/pos"); // added Fanout ... 
		bextractNetwork->linkctrl("mrs_real/duration",
			"Accumulator/acc/Series/featureNetwork/Fanout/fanout/SoundFileSource/src/mrs_real/duration"); // added Fanout ... 
		if (pluginName != EMPTYSTRING)
			bextractNetwork->linkctrl("Accumulator/acc/Series/featureNetwork/AudioSink/dest/mrs_bool/initAudio",
			"mrs_bool/initAudio");
		bextractNetwork->linkctrl("mrs_string/currentlyPlaying",
			"Accumulator/acc/Series/featureNetwork/Fanout/fanout/SoundFileSource/src/mrs_string/currentlyPlaying"); // added Fanout ... 

		if(tline)
		{
			bextractNetwork->linkControl("Accumulator/acc/Series/featureNetwork/TimelineLabeler/timelineLabeler/mrs_string/labelFiles",
				"Accumulator/acc/Series/featureNetwork/Fanout/fanout/SoundFileSource/src/mrs_string/labelNames"); // added Fanout ... 
			bextractNetwork->linkControl("Accumulator/acc/Series/featureNetwork/TimelineLabeler/timelineLabeler/mrs_natural/currentLabelFile",
				"Accumulator/acc/Series/featureNetwork/Fanout/fanout/SoundFileSource/src/mrs_natural/currentLabel"); // added Fanout ... 
			bextractNetwork->linkControl("Accumulator/acc/Series/featureNetwork/TimelineLabeler/timelineLabeler/mrs_natural/pos",
				"Accumulator/acc/Series/featureNetwork/Fanout/fanout/SoundFileSource/src/mrs_natural/pos"); // added Fanout ... 
			bextractNetwork->linkControl("Accumulator/acc/Series/featureNetwork/Fanout/fanout/SoundFileSource/src/mrs_natural/advance",
				"Accumulator/acc/Series/featureNetwork/TimelineLabeler/timelineLabeler/mrs_natural/advance"); // added Fanout ... 

			bextractNetwork->linkctrl("mrs_natural/currentLabel",
				"Accumulator/acc/Series/featureNetwork/TimelineLabeler/timelineLabeler/mrs_natural/currentLabel");
			bextractNetwork->linkctrl("mrs_string/labelNames",
				"Accumulator/acc/Series/featureNetwork/TimelineLabeler/timelineLabeler/mrs_string/labelNames");
			bextractNetwork->linkctrl("mrs_natural/nLabels",
				"Accumulator/acc/Series/featureNetwork/Fanout/fanout/SoundFileSource/src/mrs_natural/nLabels");
		}
		else
		{
			bextractNetwork->linkctrl("mrs_natural/currentLabel",
				"Accumulator/acc/Series/featureNetwork/Fanout/fanout/SoundFileSource/src/mrs_natural/currentLabel"); 
			bextractNetwork->linkctrl("mrs_natural/nLabels",
				"Accumulator/acc/Series/featureNetwork/Fanout/fanout/SoundFileSource/src/mrs_natural/nLabels"); 
			bextractNetwork->linkctrl("mrs_string/labelNames",
				"Accumulator/acc/Series/featureNetwork/Fanout/fanout/SoundFileSource/src/mrs_string/labelNames"); 
		}

		bextractNetwork->linkctrl("Accumulator/acc/Series/featureNetwork/Fanout/fanout/SoundFileSource/src/mrs_natural/advance", 
			"mrs_natural/advance");
	}
	else // running feature extraction
	{
		bextractNetwork->addMarSystem(featureNetwork);
		// link controls to top-level to make life simpler
		bextractNetwork->linkctrl("Series/featureNetwork/Fanout/fanout/SoundFileSource/src/mrs_string/filename",
			"mrs_string/filename"); 
		bextractNetwork->linkctrl("mrs_bool/notEmpty",
			"Series/featureNetwork/Fanout/fanout/SoundFileSource/src/mrs_bool/notEmpty"); 
		bextractNetwork->linkctrl("mrs_natural/pos",
			"Series/featureNetwork/Fanout/fanout/SoundFileSource/src/mrs_natural/pos"); 
		bextractNetwork->linkctrl("mrs_real/duration",
			"Series/featureNetwork/Fanout/fanout/SoundFileSource/src/mrs_real/duration"); 
		if (pluginName != EMPTYSTRING)
			bextractNetwork->linkctrl("Series/featureNetwork/AudioSink/dest/mrs_bool/initAudio",
			"mrs_bool/initAudio");
		bextractNetwork->linkctrl("mrs_string/currentlyPlaying",
			"Series/featureNetwork/Fanout/fanout/SoundFileSource/src/mrs_string/currentlyPlaying"); 

		if(tline)
		{
			bextractNetwork->linkctrl("Series/featureNetwork/TimelineLabeler/timelineLabeler/mrs_natural/currentLabelFile",
				"Series/featureNetwork/Fanout/fanout/SoundFileSource/src/mrs_natural/currentLabel"); 
			bextractNetwork->linkctrl("Series/featureNetwork/TimelineLabeler/timelineLabeler/mrs_string/labelFiles",
				"Series/featureNetwork/Fanout/fanout/SoundFileSource/src/mrs_string/labelNames"); 
			bextractNetwork->linkctrl("Series/featureNetwork/TimelineLabeler/timelineLabeler/mrs_natural/pos",
				"Series/featureNetwork/Fanout/fanout/SoundFileSource/src/mrs_natural/pos"); 
			bextractNetwork->linkctrl("Series/featureNetwork/Fanout/fanout/SoundFileSource/src/mrs_natural/advance",
				"Series/featureNetwork/TimelineLabeler/timelineLabeler/mrs_natural/advance"); 

			bextractNetwork->linkctrl("mrs_natural/currentLabel",
				"Series/featureNetwork/TimelineLabeler/timelineLabeler/mrs_natural/currentLabel");
			bextractNetwork->linkctrl("mrs_string/labelNames",
				"Series/featureNetwork/TimelineLabeler/timelineLabeler/mrs_string/labelNames");
			bextractNetwork->linkctrl("mrs_natural/nLabels",
				"Series/featureNetwork/TimelineLabeler/timelineLabeler/mrs_natural/nLabels");
		}
		else
		{
			bextractNetwork->linkctrl("mrs_natural/currentLabel",
				"Series/featureNetwork/Fanout/fanout/SoundFileSource/src/mrs_natural/currentLabel"); 
			bextractNetwork->linkctrl("mrs_natural/nLabels",
				"Series/featureNetwork/Fanout/fanout/SoundFileSource/src/mrs_natural/nLabels"); 
			bextractNetwork->linkctrl("mrs_string/labelNames",
				"Series/featureNetwork/Fanout/fanout/SoundFileSource/src/mrs_string/labelNames"); 
		}
	}

	// labeling, weka output, classifier and confidence for real-time output
	bextractNetwork->addMarSystem(mng.create("Annotator", "annotator"));
	if (wekafname != EMPTYSTRING)
		bextractNetwork->addMarSystem(mng.create("WekaSink", "wsink"));


	if (!featExtract_)
	{
		bextractNetwork->addMarSystem(mng.create("Classifier", "cl"));
		bextractNetwork->addMarSystem(mng.create("Confidence", "confidence"));

		// link confidence and annotation with SoundFileSource that plays the collection
		bextractNetwork->linkctrl("Confidence/confidence/mrs_string/fileName",
								  "mrs_string/filename");
	}

	bextractNetwork->linkctrl("Annotator/annotator/mrs_natural/label",
								  "mrs_natural/currentLabel");

	// links with WekaSink
	if (wekafname != EMPTYSTRING)
	{
		bextractNetwork->linkctrl("WekaSink/wsink/mrs_string/currentlyPlaying",
			"mrs_string/currentlyPlaying");

		bextractNetwork->linkctrl("WekaSink/wsink/mrs_string/labelNames",
			"mrs_string/labelNames");
		bextractNetwork->linkctrl("WekaSink/wsink/mrs_natural/nLabels", "mrs_natural/nLabels");
	}

	// src has to be configured with hopSize frame length in case a ShiftInput
	// is used in the feature extraction network
	bextractNetwork->updctrl("mrs_natural/inSamples", hopSize);
	if (stereo_)
		featureNetwork->updctrl("StereoFeatures/stereoFeatures/mrs_natural/winSize",
		winSize);
	else
		featureNetwork->updctrl("TimbreFeatures/featExtractor/mrs_natural/winSize",
		winSize);

	if (start > 0.0)
		offset = (mrs_natural) (start * src->getctrl("mrs_real/israte")->to<mrs_real>());
	bextractNetwork->updctrl("mrs_natural/pos", offset);
	bextractNetwork->updctrl("mrs_real/duration", length);

	// confidence is silent during training
	if (!featExtract_)
	{
		bextractNetwork->updctrl("Confidence/confidence/mrs_bool/mute", true);
		bextractNetwork->updctrl("Confidence/confidence/mrs_bool/print",true);
		if (single_vector)
			bextractNetwork->updctrl("Confidence/confidence/mrs_natural/memSize", 1);

		// select classifier to be used
		selectClassifier(bextractNetwork, classifierName);
	}



	// load the collection which is automatically created by bextract
	// based on the command-line arguments

	if (workspaceDir != EMPTYSTRING)
		bextractNetwork->updctrl("mrs_string/filename", workspaceDir + "bextract_single.mf");
	else
		bextractNetwork->updctrl("mrs_string/filename", "bextract_single.mf");

	// play sound if playback is enabled
	if (pluginName != EMPTYSTRING && playback)
	{
		featureNetwork->updctrl("AudioSink/dest/mrs_bool/mute", false);
		featureNetwork->updctrl("mrs_bool/initAudio", true);
	}

	// don't use linkctrl so that only value is copied once and linking doesn't
	// remain for the plugin
	if (!featExtract_)
	{
		bextractNetwork->updctrl("Confidence/confidence/mrs_string/labelNames",
								 bextractNetwork->getctrl("mrs_string/labelNames"));
		bextractNetwork->updctrl("Classifier/cl/mrs_natural/nClasses",
								 bextractNetwork->getctrl("mrs_natural/nLabels"));
		bextractNetwork->updctrl("Confidence/confidence/mrs_natural/nLabels",
								 bextractNetwork->getctrl("mrs_natural/nLabels"));
	}


	// setup WekaSink - has to be done after all updates so that changes are correctly
	// written to file
	if (wekafname != EMPTYSTRING)
	{
		bextractNetwork->updctrl("WekaSink/wsink/mrs_natural/downsample", downSample);

		cout << "Downsampling factor = " << downSample << endl;

		if (workspaceDir != EMPTYSTRING)
		{
			wekafname = workspaceDir + wekafname;
		}
		bextractNetwork->updctrl("WekaSink/wsink/mrs_string/filename", wekafname);
	}



	// main processing loop for training
	MarControlPtr ctrl_notEmpty = bextractNetwork->getctrl("mrs_bool/notEmpty");
	MarControlPtr ctrl_currentlyPlaying = bextractNetwork->getctrl("mrs_string/currentlyPlaying");
	mrs_string previouslyPlaying, currentlyPlaying;


	int n = 0;
	int advance = 1;

	vector<string> processedFiles;
	map<string, realvec> processedFeatures;

	bool seen;
	realvec fvec;
	int label;

	 while (ctrl_notEmpty->to<mrs_bool>())
	{


		if (single_vector)
		{
			currentlyPlaying = ctrl_currentlyPlaying->to<mrs_string>();
			label = bextractNetwork->getctrl("mrs_natural/currentLabel")->to<mrs_natural>();
			seen = false;


			for (int j=0; j<processedFiles.size(); j++)
			{
				if (processedFiles[j] == currentlyPlaying)
					seen = true;
			}


			if (seen)
			{
			  advance++;
			  bextractNetwork->updctrl("mrs_natural/advance", advance);
			  
			  if (wekafname != EMPTYSTRING)
			    bextractNetwork->updctrl("WekaSink/wsink/mrs_string/injectComment", "% filename " + currentlyPlaying);
			  
			  fvec = processedFeatures[currentlyPlaying];
			  fvec(fvec.getSize()-1) = label;
			  
			  if (wekafname != EMPTYSTRING)
			    {
			      bextractNetwork->updctrl("WekaSink/wsink/mrs_realvec/injectVector", fvec);
			      
			      bextractNetwork->updctrl("WekaSink/wsink/mrs_bool/inject", true);
			    }
			}
			else
			{
			  bextractNetwork->tick();
			  
			  fvec = bextractNetwork->getctrl("Annotator/annotator/mrs_realvec/processedData")->to<mrs_realvec>();
			  
			  bextractNetwork->updctrl("mrs_natural/advance", advance);
			  processedFiles.push_back(currentlyPlaying);
			  processedFeatures[currentlyPlaying] = fvec;
			  cout << "Processed: " << n << " - " << currentlyPlaying << endl;
			  advance = 1;
			  bextractNetwork->updctrl("mrs_natural/advance", 1);
			}
			n++;

		}
		else
		{
			bextractNetwork->tick();
			currentlyPlaying = ctrl_currentlyPlaying->to<mrs_string>();
			if (currentlyPlaying != previouslyPlaying)
			{
				cout << "Processed: " << n << " - " << currentlyPlaying << endl;
				n++;

			}

			previouslyPlaying = currentlyPlaying;
		}



	}



	cout << "Finished feature extraction" << endl;
	if (featExtract_)
		return;


	// prepare network for real-time playback/prediction
	bextractNetwork->updctrl("Classifier/cl/mrs_string/mode","predict");

	cout << "Finished classifier training" << endl;
	if (mic_)
	  cout << "Microphone input used" << endl;
	// have the plugin play audio 
	if (pluginName != EMPTYSTRING && !pluginMute)
	{
		featureNetwork->updctrl("mrs_real/israte", 44100.0);
		featureNetwork->updctrl("AudioSink/dest/mrs_bool/mute", false); 
		featureNetwork->updctrl("AudioSink/dest/mrs_bool/initAudio", true); 
		
		// mute Audio since we are listening with mic at runtime ... 
		if (mic_)
		  featureNetwork->updctrl("AudioSink/dest/mrs_bool/mute", true);
		else 
		  featureNetwork->updctrl("AudioSink/dest/mrs_bool/mute", false);
	}
	
	// init mic audio ... 
	if (mic_) 
	  {
	    bextractNetwork->updctrl("mrs_real/israte", 44100.0);   //sampling rate 
	    bextractNetwork->updctrl("Series/featureNetwork/Fanout/fanout/AudioSource/mic/mrs_natural/nChannels", 1);	//stereo
	    bextractNetwork->linkctrl( "mrs_bool/initAudio" , "Series/featureNetwork/Fanout/fanout/AudioSource/mic/mrs_bool/initAudio" ); //important link!!!
	  }
	    
	
	// finally disable the Soundfile Input in Fanout ...
	if (mic_) 
	  {
	    bextractNetwork->updctrl("Series/featureNetwork/Fanout/fanout/mrs_natural/disable", 0);
	    // ... and enable live audio source ... 
	    bextractNetwork->updctrl("Series/featureNetwork/Fanout/fanout/mrs_natural/enable", 1);
	  }
	
		
	// don't output to WekaSink
	if (wekafname != EMPTYSTRING)
		bextractNetwork->updctrl("WekaSink/wsink/mrs_bool/mute", false);

	// enable confidence
	bextractNetwork->updctrl("Confidence/confidence/mrs_bool/mute", false);

	// output trained classifier models
	if (pluginName == EMPTYSTRING) // output to stdout
		;

		// cout << (*bextractNetwork) << endl;
	else // save to .mpl file
	{
		ofstream oss(pluginName.c_str());
		oss << (*bextractNetwork) << endl;
	}

	// predict optional test collection
	if (testCollection != EMPTYSTRING)
	{
		bextractNetwork->updctrl("mrs_natural/advance", 0);
		if (single_vector)
		{
			if (pluginName != EMPTYSTRING && !pluginMute)
				featureNetwork->updctrl("AudioSink/dest/mrs_bool/mute", true);

			Collection m;
			m.read(testCollection);

			Collection l;
			if (workspaceDir != EMPTYSTRING)
				l.read(workspaceDir + "bextract_single.mf");
			else
				l.read("bextract_single.mf");
			bextractNetwork->updctrl("Confidence/confidence/mrs_bool/mute", true);

			 

			if (wekafname != EMPTYSTRING)
				bextractNetwork->updctrl("WekaSink/wsink/mrs_string/filename", "predict.arff");
			bextractNetwork->updctrl("Classifier/cl/mrs_string/mode", "predict");

			ofstream prout;
			prout.open(predictCollection.c_str());

			int correct_instances = 0;
			int num_instances = 0;

			bextractNetwork->updctrl("mrs_string/filename", testCollection);
			bextractNetwork->updctrl("mrs_string/labelNames", l.getLabelNames());
			
			ofstream ofs;
			ofs.open("bextract.mpl");
			ofs << *bextractNetwork << endl;
			ofs.close();
			
		   

			while (ctrl_notEmpty->to<mrs_bool>())
			{
				bextractNetwork->tick();
				currentlyPlaying = ctrl_currentlyPlaying->to<mrs_string>();

				mrs_realvec pr = bextractNetwork->getctrl("Classifier/cl/mrs_realvec/processedData")->to<mrs_realvec>();
				cout << "Predicting " << currentlyPlaying << "\t" << "GT:" << l.labelName((mrs_natural)pr(1,0)) << "\t" << "PR:" << l.labelName((mrs_natural)pr(0,0)) << endl;

				if (single_vector)
				{
					bextractNetwork->updctrl("mrs_natural/advance", 1);
				}


				if ((mrs_natural)pr(0,0) == (mrs_natural)(pr(1,0)))
				  correct_instances++;
				num_instances++;

				prout << currentlyPlaying << "\t" << l.labelName((mrs_natural)pr(0,0)) << endl;
			}
			cout << "Correct instances = " << correct_instances << "/" << num_instances << endl;
		}

		else
		{

			cout << "bextract_train_refactored: predicting test collection: " << testCollection << endl;
			bextractNetwork->updctrl("mrs_string/filename", testCollection);

			while (ctrl_notEmpty->to<mrs_bool>())
			{
				currentlyPlaying = ctrl_currentlyPlaying->to<mrs_string>();
				if (currentlyPlaying != previouslyPlaying)
					cout << "Processing : " << currentlyPlaying << endl;

				bextractNetwork->tick();
				previouslyPlaying = currentlyPlaying;
			}
		}
	}



	delete bextractNetwork;
	return;
}



// train with multiple feature vectors/file
void bextract_train_rmsilence(vector<Collection> cls, mrs_natural label,
															string pluginName, string classNames,
															string wekafname,  mrs_natural memSize,
															string extractorStr,
															string classifierName)
{
	if (classifierName == EMPTYSTRING)
		classifierName = "SMO";

	MRSDIAG("bextract.cpp - bextract_train");
	mrs_natural i;
	mrs_natural cj;

	Collection linitial = cls[0];
	string sfName = linitial.entry(0);

	// default
	if (extractorStr == EMPTYSTRING)
		extractorStr = "STFT";

	MarSystemManager mng;

	// Find proper soundfile format and create SignalSource
	MarSystem *srm = mng.create("SilenceRemove", "srm");
	MarSystem *src = mng.create("SoundFileSource", "src");
	src->updctrl("mrs_string/filename", sfName);

	srm->addMarSystem(src);

	// src->updctrl("mrs_natural/inSamples", MRS_DEFAULT_SLICE_NSAMPLES);
	// srm->updctrl("mrs_natural/inSamples", 2048);
	// srm->updctrl("mrs_natural/inSamples", 2048);

	// Calculate duration, offest parameters if necessary
	if (start > 0.0f)
		offset = (mrs_natural) (start
		* src->getctrl("mrs_real/israte")->to<mrs_real>());

	if (length != 30.0f)
		duration = (mrs_natural) (length
		* src->getctrl("mrs_real/israte")->to<mrs_real>());

	// create audio sink and mute it
	// it is stored in the output plugin
	// which can be used for real-time classification
	MarSystem* dest=NULL;

	if (pluginName != EMPTYSTRING) // output to stdout
	{
		dest = mng.create("AudioSink", "dest");
		dest->updctrl("mrs_bool/mute", true);
	}

	// Calculate windowed power spectrum and then
	// calculate specific feature sets
	MarSystem* spectralShape = mng.create("Series", "spectralShape");
	spectralShape->addMarSystem(mng.create("Windowing", "hamming"));
	spectralShape->addMarSystem(mng.create("Spectrum","spk"));
	spectralShape->updctrl("Spectrum/spk/mrs_real/cutoff", 1.0);
	spectralShape->addMarSystem(mng.create("PowerSpectrum", "pspk"));
	spectralShape->updctrl("PowerSpectrum/pspk/mrs_string/spectrumType","power");

	// Spectrum Shape descriptors
	MarSystem *spectrumFeatures = mng.create("Fanout",  "spectrumFeatures");
	if (extractorStr == "STFT")
	{
		spectrumFeatures->addMarSystem(mng.create("Centroid", "cntrd"));
		spectrumFeatures->addMarSystem(mng.create("Rolloff", "rlf"));
		spectrumFeatures->addMarSystem(mng.create("Flux", "flux"));
	}
	else if (extractorStr == "STFTMFCC")
	{
		spectrumFeatures->addMarSystem(mng.create("Centroid", "cntrd"));
		spectrumFeatures->addMarSystem(mng.create("Rolloff", "rlf"));
		spectrumFeatures->addMarSystem(mng.create("Flux", "flux"));
		spectrumFeatures->addMarSystem(mng.create("MFCC", "mfcc"));
	}
	else if (extractorStr == "MFCC")
		spectrumFeatures->addMarSystem(mng.create("MFCC", "mfcc"));
	else if (extractorStr == "SCF")
		spectrumFeatures->addMarSystem(mng.create("SCF", "scf"));
	else if (extractorStr == "SFM")
		spectrumFeatures->addMarSystem(mng.create("SFM", "sfm"));
	else
	{
		cerr << "Extractor " << extractorStr << " is not supported " << endl;
		return;
	}

	// add the feature to spectral shape
	spectralShape->addMarSystem(spectrumFeatures);

	//  add time-domain zerocrossings
	MarSystem* features = mng.create("Fanout", "features");
	features->addMarSystem(spectralShape);

	if (extractorStr == "STFT")
		features->addMarSystem(mng.create("ZeroCrossings", "zcrs"));

	// Means and standard deviation (statistics) for texture analysis
	MarSystem* statistics = mng.create("Fanout", "statistics");
	statistics->addMarSystem(mng.create("Mean", "mn"));
	statistics->addMarSystem(mng.create("StandardDeviation", "std"));

	// Weka output
	MarSystem* wsink = mng.create("WekaSink", "wsink");

	// Build the overall feature calculation network
	MarSystem* featureNetwork = mng.create("Series", "featureNetwork");
	featureNetwork->addMarSystem(srm->clone());

	if (pluginName != EMPTYSTRING) // don't put audio object
		featureNetwork->addMarSystem(dest);

	cout << "featureNetwork = " << (*featureNetwork) << endl;

	featureNetwork->addMarSystem(features);
	featureNetwork->addMarSystem(mng.create("Memory", "memory"));

	featureNetwork->addMarSystem(statistics);
	if (classifierName == "SMO")
		featureNetwork->addMarSystem(mng.create("NormMaxMin", "norm"));

	// update controls I
	featureNetwork->updctrl("Memory/memory/mrs_natural/memSize", memSize);
	// featureNetwork->updctrl("mrs_natural/inSamples",
	// MRS_DEFAULT_SLICE_NSAMPLES);

	featureNetwork->updctrl("mrs_natural/inSamples", winSize);
	featureNetwork->updctrl("SilenceRemove/srm/" + src->getType() + "/src/mrs_natural/pos", offset);
	featureNetwork->addMarSystem(mng.create("Annotator", "annotator"));
	featureNetwork->addMarSystem(wsink->clone());

	// add classifier and confidence majority calculation
	if (classifierName == "GS")
		featureNetwork->addMarSystem(mng.create("GaussianClassifier", "gaussian"));
	else if (classifierName == "ZeroR")
		featureNetwork->addMarSystem(mng.create("ZeroRClassifier", "zeror"));
	else if (classifierName == "KNN")
		featureNetwork->addMarSystem(mng.create("KNNClassifier", "knn"));
	else if (classifierName == "SMO")
		featureNetwork->addMarSystem(mng.create("SMO", "smo"));
	else
	{
		cerr << "Unsuported classifier : " << classifierName << endl;
		return;
	}

	featureNetwork->addMarSystem(mng.create("Confidence", "confidence"));

	// update controls II
	if (classifierName == "GS")
		featureNetwork->updctrl("GaussianClassifier/gaussian/mrs_natural/nClasses", (mrs_natural)cls.size());
	else if (classifierName == "ZeroR")
		featureNetwork->updctrl("ZeroRClassifier/zeror/mrs_natural/nClasses", (mrs_natural)cls.size());
	else if (classifierName == "KNN")
		featureNetwork->updctrl("KNNClassifier/knn/mrs_natural/nLabels", (mrs_natural)cls.size());
	else if (classifierName == "SMO")
		featureNetwork->updctrl("SMO/smo/mrs_natural/nLabels", (mrs_natural)cls.size());

	featureNetwork->updctrl("Confidence/confidence/mrs_bool/mute", true);
	featureNetwork->updctrl("Confidence/confidence/mrs_string/labelNames",classNames);
	featureNetwork->updctrl("WekaSink/wsink/mrs_string/labelNames",classNames);

	// link controls
	featureNetwork->linkctrl("mrs_string/filename",
		"SilenceRemove/srm/SoundFileSource/src/mrs_string/filename");
	featureNetwork->linkctrl("mrs_real/israte",
		"SilenceRemove/srm/SoundFileSource/src/mrs_real/israte");
	featureNetwork->linkctrl("mrs_natural/pos",
		"SilenceRemove/srm/SoundFileSource/src/mrs_natural/pos");
	featureNetwork->linkctrl("mrs_bool/notEmpty",
		"SilenceRemove/srm/SoundFileSource/src/mrs_bool/notEmpty");

	mrs_natural wc = 0;
	mrs_natural samplesPlayed =0;
	mrs_natural onSamples = featureNetwork->getctrl("mrs_natural/onSamples")->to<mrs_natural>();

	// main loop for extracting the features
	featureNetwork->updctrl("Confidence/confidence/mrs_natural/nLabels", (int)cls.size());
	string className = "";

	MarControlPtr donePtr = featureNetwork->getctrl("SilenceRemove/srm/SoundFileSource/src/mrs_bool/notEmpty");
	MarControlPtr memResetPtr = featureNetwork->getctrl("Memory/memory/mrs_bool/reset");
	MarControlPtr fnamePtr = featureNetwork->getctrl("SilenceRemove/srm/SoundFileSource/src/mrs_string/filename");
	MarControlPtr annLabelPtr = featureNetwork->getctrl("Annotator/annotator/mrs_natural/label");
	MarControlPtr nlabelsPtr = featureNetwork->getctrl("WekaSink/wsink/mrs_natural/nLabels");
	MarControlPtr wekaDownsamplePtr = featureNetwork->getctrl("WekaSink/wsink/mrs_natural/downsample");
	MarControlPtr wekaFnamePtr = featureNetwork->getctrl("WekaSink/wsink/mrs_string/filename");

	for (cj=0; cj < (mrs_natural)cls.size(); cj++)
	{
		Collection l = cls[cj];
		featureNetwork->setctrl(annLabelPtr, cj);
		featureNetwork->setctrl(nlabelsPtr, (mrs_natural)cls.size());
		featureNetwork->setctrl(wekaDownsamplePtr, 40);
		if (wekafname == EMPTYSTRING)
			featureNetwork->updctrl(wekaFnamePtr, "weka.arff");
		else
			featureNetwork->updctrl(wekaFnamePtr, wekafname);
		// featureNetwork->updctrl("WekaSink/wsink/mrs_natural/label", cj);

		cout << "Class " << cj << " is " << l.name() << endl;

		featureNetwork->setctrl(memResetPtr, true);

		for (i=0; i < l.size(); i++)
		{
			featureNetwork->setctrl(memResetPtr, true);
			featureNetwork->updctrl(fnamePtr, l.entry(i));
			wc = 0;
			samplesPlayed = 0;
			while ((donePtr->isTrue()) && (duration > samplesPlayed))
			{
				featureNetwork->tick();
				wc++;
				samplesPlayed = wc * onSamples;
			}
			featureNetwork->tick();
			cerr << "Processed " << l.entry(i) << endl;
		}
	}

	if (classifierName == "GS")
		featureNetwork->updctrl("GaussianClassifier/gaussian/mrs_bool/done",true);
	else if (classifierName == "ZeroR")
		featureNetwork->updctrl("ZeroRClassifier/zeror/mrs_bool/done",true);
	else if (classifierName == "KNN")
		featureNetwork->updctrl("KNNClassifier/knn/mrs_bool/done",true);
	else if (classifierName == "SMO")
		featureNetwork->updctrl("SMO/smo/mrs_bool/done",true);

	if (classifierName == "SMO")
		featureNetwork->updctrl("NormMaxMin/norm/mrs_string/mode", "predict");
	featureNetwork->tick();		// train classifier

	// prepare network for classification
	if (classifierName == "GS")
	{
		featureNetwork->updctrl("GaussianClassifier/gaussian/mrs_bool/done",false);
		featureNetwork->updctrl("GaussianClassifier/gaussian/mrs_string/mode","predict");
	}
	else if (classifierName == "ZeroR")
	{
		featureNetwork->updctrl("ZeroRClassifier/zeror/mrs_bool/done",false);
		featureNetwork->updctrl("ZeroRClassifier/zeror/mrs_string/mode","predict") ;
	}
	else if (classifierName == "KNN")
	{
		featureNetwork->updctrl("KNNClassifier/knn/mrs_bool/done",false);
		featureNetwork->updctrl("KNNClassifier/knn/mrs_string/mode","predict");
		featureNetwork->updctrl("KNNClassifier/knn/mrs_natural/k",3);
	}
	else if (classifierName == "SMO")
	{
		featureNetwork->updctrl("SMO/smo/mrs_bool/done",false);
		featureNetwork->updctrl("SMO/smo/mrs_string/mode","predict");
	}

	/* if (pluginName != EMPTYSTRING)
	{
	featureNetwork->updctrl("AudioSink/dest/mrs_bool/mute", false);
	featureNetwork->updctrl("AudioSink/dest/mrs_bool/init", false);
	}
	*/

	featureNetwork->updctrl("Confidence/confidence/mrs_bool/mute", false);
	featureNetwork->updctrl("mrs_string/filename", "defaultfile");
	featureNetwork->updctrl("WekaSink/wsink/mrs_bool/mute", true);

	if (pluginName == EMPTYSTRING) // output to stdout
		cout << (*featureNetwork) << endl;
	else
	{
		ofstream oss(pluginName.c_str());
		oss << (*featureNetwork) << endl;
	}

	delete featureNetwork;
}

void
readCollection(Collection& l, string name)
{
	MRSDIAG("sfplay.cpp - readCollection");
	ifstream from1(name.c_str());
	mrs_natural attempts=0;

	MRSDIAG("Trying current working directory: " + name);
	if (from1.good() == false)
	{
		attempts++;
	}
	else
	{
		from1 >> l;
		l.setName(name.substr(0, name.rfind(".", name.length())));
	}

	if (attempts == 1)
	{
		string warn;
		warn += "Problem reading collection ";
		warn += name;
		warn += " - tried both default mf directory and current working directory";
		MRSWARN(warn);
		exit(1);
	}
}

void
initOptions()
{
	cmd_options.addBoolOption("help", "h", false);
	cmd_options.addBoolOption("usage", "u", false);
	cmd_options.addBoolOption("verbose", "v", false);
	cmd_options.addStringOption("collection", "c", EMPTYSTRING);
	cmd_options.addBoolOption("normalize", "n", false);
	cmd_options.addRealOption("start", "s", 0);
	cmd_options.addRealOption("length", "l", -1.0f);
	cmd_options.addStringOption("plugin", "p", EMPTYSTRING);
	cmd_options.addStringOption("wekafile", "w", EMPTYSTRING);
	cmd_options.addStringOption("extractor", "e", "REFACTORED");
	cmd_options.addNaturalOption("memory", "m", 40);
	cmd_options.addNaturalOption("winsamples", "ws", 512);
	cmd_options.addNaturalOption("accSize", "as", 1000);
	cmd_options.addNaturalOption("hopsamples", "hp", 512);
	cmd_options.addStringOption("classifier", "cl", EMPTYSTRING);
	cmd_options.addBoolOption("tline", "t", false);
	cmd_options.addBoolOption("pluginmute", "pm", false);
	cmd_options.addBoolOption("playback", "pb", false);
	cmd_options.addStringOption("outputdir", "od", EMPTYSTRING);
	cmd_options.addStringOption("predict", "pr", EMPTYSTRING);
	cmd_options.addStringOption("test", "tc", EMPTYSTRING);
	cmd_options.addBoolOption("stereo", "st", false);
	cmd_options.addNaturalOption("downsample", "ds", 1);
	cmd_options.addBoolOption("shuffle", "sh", false);
	cmd_options.addBoolOption("mic", "mc", false);

	// feature selection options
	cmd_options.addBoolOption("StereoPanningSpectrumFeatures", "spsf", false);
	cmd_options.addBoolOption("MelFrequencyCepstralCoefficients","mfcc", false);
	cmd_options.addBoolOption("Chroma", "chroma", false);
	cmd_options.addBoolOption("SpectralFlatnessMeasure","sfm", false);
	cmd_options.addBoolOption("SpectralCrestFactor","scf", false);
	cmd_options.addBoolOption("SpectralCentroid","ctd", false);
	cmd_options.addBoolOption("SpectralRolloff","rlf", false);
	cmd_options.addBoolOption("SpectralFlux","flx", false);
	cmd_options.addBoolOption("SpectralFeatures", "spfe", false);
	cmd_options.addBoolOption("ZeroCrossings", "zcrs", false);
	cmd_options.addBoolOption("LineSpectralPair", "lsp", false);
	cmd_options.addBoolOption("LinearPredictionCepstralCoefficients", "lpcc", false);

	cmd_options.addBoolOption("TimbralFeatures", "timbral", false);
	cmd_options.addBoolOption("SingleVector", "sv", false);
	cmd_options.addBoolOption("featExtract", "fe", false);
}

void
loadOptions()
{
	helpopt = cmd_options.getBoolOption("help");
	usageopt = cmd_options.getBoolOption("usage");
	start = cmd_options.getRealOption("start");
	length = cmd_options.getRealOption("length");
	normopt = cmd_options.getBoolOption("normalize");
	collectionName = cmd_options.getStringOption("collection");
	pluginName = cmd_options.getStringOption("plugin");
	wekafname = cmd_options.getStringOption("wekafile");
	extractorName = cmd_options.getStringOption("extractor");
	classifierName = cmd_options.getStringOption("classifier");
	memSize = cmd_options.getNaturalOption("memory");
	winSize = cmd_options.getNaturalOption("winsamples");
	hopSize = cmd_options.getNaturalOption("hopsamples");
	accSize_ = cmd_options.getNaturalOption("accSize");
	tline = cmd_options.getBoolOption("tline");
	pluginMute  = cmd_options.getBoolOption("pluginmute");
	playback = cmd_options.getBoolOption("playback");
	workspaceDir = cmd_options.getStringOption("outputdir");
	predictCollection = cmd_options.getStringOption("predict");
	testCollection = cmd_options.getStringOption("test");
	downSample = cmd_options.getNaturalOption("downsample");
	shuffle_ = cmd_options.getBoolOption("shuffle");
	mic_ = cmd_options.getBoolOption("mic");
	stereo_ = cmd_options.getBoolOption("stereo");
	featExtract_ = cmd_options.getBoolOption("featExtract");

	// feature selection options
	spsf_ = cmd_options.getBoolOption("StereoPanningSpectrumFeatures");
	mfcc_ = cmd_options.getBoolOption("MelFrequencyCepstralCoefficients");
	chroma_ = cmd_options.getBoolOption("Chroma");
	sfm_ = cmd_options.getBoolOption("SpectralFlatnessMeasure");
	scf_ = cmd_options.getBoolOption("SpectralCrestFactor");
	ctd_ = cmd_options.getBoolOption("SpectralCentroid");
	rlf_ = cmd_options.getBoolOption("SpectralRolloff");
	flx_ = cmd_options.getBoolOption("SpectralFlux");
	lsp_ = cmd_options.getBoolOption("LineSpectralPair");
	lpcc_ = cmd_options.getBoolOption("LinearPredictionCepstralCoefficients");

	spectralFeatures_ = cmd_options.getBoolOption("SpectralFeatures");
	zcrs_ = cmd_options.getBoolOption("ZeroCrossings");
	timbralFeatures_ = cmd_options.getBoolOption("TimbralFeatures");

	single_vector_ = cmd_options.getBoolOption("SingleVector");

	// default feature set
	if ((mfcc_ == false) &&
		(sfm_ == false)  &&
		(scf_ == false)  &&
		(ctd_ == false)  &&
		(rlf_ == false)  &&
		(flx_ == false)  &&
		(spectralFeatures_ == false) &&
		(zcrs_ == false) &&
		(timbralFeatures_ == false) &&
		(lsp_ == false) &&
		(lpcc_ == false) &&
		(chroma_ ==false))
	{
		timbralFeatures_ = true;
	}
}

void bextract(vector<string> soundfiles, mrs_natural label,
							string pluginName, string classNames,
							string wekafname,  mrs_natural memSize,
							string extractorStr,
							string classifierName)
{
	MarSystemManager mng;

	// Spectrum Shape descriptors
	MarSystem* spectrumFeatures = mng.create("Fanout", "spectrumFeatures");
	if (extractorStr == "STFT")
	{
		spectrumFeatures->addMarSystem(mng.create("Centroid", "cntrd"));
		spectrumFeatures->addMarSystem(mng.create("Rolloff", "rlf"));
		spectrumFeatures->addMarSystem(mng.create("Flux", "flux"));
	}
	else if (extractorStr == "MFCC")
	{
		spectrumFeatures->addMarSystem(mng.create("MFCC", "mfcc"));
	}
	else if (extractorStr == "STFTMFCC")
	{
		spectrumFeatures->addMarSystem(mng.create("Centroid", "cntrd"));
		spectrumFeatures->addMarSystem(mng.create("Rolloff", "rlf"));
		spectrumFeatures->addMarSystem(mng.create("Flux", "flux"));
	}
	else
	{
		cout << "New bextract does not support extractor " << extractorStr << endl;
		return;
	}

	// Means and standard deviations of the spectrum features
	// over a 1-second window (40 analysis frames)
	MarSystem* textureFeatures = mng.create("Series", "textureFeatures");
	textureFeatures->addMarSystem(spectrumFeatures);
	textureFeatures->addMarSystem(mng.create("Memory", "textureMemory"));

	MarSystem* textureStats = mng.create("Fanout", "textureStats");
	textureStats->addMarSystem(mng.create("Mean", "mean"));
	textureStats->addMarSystem(mng.create("StandardDeviation", "std"));
	textureFeatures->addMarSystem(textureStats);

	// The main feature calculation network
	MarSystem* spectralShape = mng.create("Series", "spectralShape");
	spectralShape->addMarSystem(mng.create("SoundFileSource", "src"));
	//spectralShape->addMarSystem(mng.create("AudioSink", "dest"));
	spectralShape->addMarSystem(mng.create("Windowing", "hamming"));
	spectralShape->addMarSystem(mng.create("Spectrum", "spk"));
	spectralShape->addMarSystem(mng.create("PowerSpectrum", "pspk"));
	spectralShape->addMarSystem(textureFeatures);

	// Label each class
	spectralShape->addMarSystem(mng.create("Annotator", "anno"));
	spectralShape->addMarSystem(mng.create("WekaSingk", "wsink"));
	spectralShape->addMarSystem(mng.create("GaussianClassifier", "gsc"));
	spectralShape->addMarSystem(mng.create("Confidence", "conf"));

	vector<string>::iterator si;
	mrs_natural classCount = 0;

	if (wekafname == EMPTYSTRING)
		spectralShape->updctrl("WekaSink/wsink/mrs_string/filename", "weka.arff");
	else
		spectralShape->updctrl("WekaSink/wsink/mrs_string/filename", wekafname);

	spectralShape->updctrl("GaussianClassifier/gsc/mrs_natural/nClasses", (mrs_natural)soundfiles.size());

	spectralShape->linkctrl("mrs_bool/notEmpty",
		"SoundFileSource/src/mrs_bool/notEmpty");
	spectralShape->linkctrl("mrs_string/filename",
		"SoundFileSource/src/mrs_string/filename");
	spectralShape->linkctrl("mrs_natural/pos",
		"SoundFileSource/src/mrs_natural/pos");

	spectralShape->updctrl("GaussianClassifier/gsc/mrs_string/mode","train");
	spectralShape->updctrl("WekaSink/wsink/mrs_string/labelNames",classNames);
	spectralShape->updctrl("Confidence/conf/mrs_string/labelNames",classNames);

	for (si = soundfiles.begin(); si != soundfiles.end(); ++si)
	{
		cout << "Processing class " << classCount << " collection: "
			<< *si << endl;
		spectralShape->updctrl("SoundFileSource/src/mrs_string/filename", *si);
		spectralShape->updctrl("Annotator/anno/mrs_natural/label", classCount);
		classCount ++;

		while(spectralShape->getctrl("mrs_bool/notEmpty")->to<mrs_bool>())
		{
			spectralShape->tick();
		}
	}
	spectralShape->updctrl("GaussianClassifier/gsc/mrs_bool/done",true);

	spectralShape->tick(); // train classifier

	spectralShape->updctrl("GaussianClassifier/gsc/mrs_string/mode","predict");

	if (pluginName == EMPTYSTRING) // output to stdout
		cout << (*spectralShape) << endl;
	else
	{
		ofstream oss(pluginName.c_str());
		oss << (*spectralShape) << endl;
	}
}

void
mirex_bextract()
{
	cout << "MIREX 2007 bextract" << endl;
	cout << "Extracting features for files in collection : " << collectionName << endl;
	cout << "Predicting class for files in collection : " << predictCollection << endl;

	// Get the first filename just to initialize correctly the network
	Collection l;
	l.read(collectionName);
	string sfName = l.entry(0);

	MarSystemManager mng;

	MarSystem* src = mng.create("SoundFileSource", "src");

	MarSystem* featExtractor = (*featExtractors[extractorName])();
	featExtractor->updctrl("mrs_natural/winSize", winSize);

	MarSystem* featureNetwork = mng.create("Series", "featureNetwork");
	featureNetwork->addMarSystem(src);
	featureNetwork->addMarSystem(featExtractor);

	// Texture Window Statistics (if any)
	if(memSize != 0)
	{
		featureNetwork->addMarSystem(mng.create("TextureStats", "tStats"));
		featureNetwork->updctrl("TextureStats/tStats/mrs_natural/memSize", memSize);
	}

	featureNetwork->updctrl("SoundFileSource/src/mrs_string/filename", sfName);
	featureNetwork->updctrl("mrs_natural/inSamples", MRS_DEFAULT_SLICE_NSAMPLES);

	if (extractorName == EMPTYSTRING)
	{
		cout << "Please specify feature extractor" << endl;
		return;
	}

	MarSystem* acc = mng.create("Accumulator", "acc");
	acc->updctrl("mrs_natural/nTimes", accSize_);
	acc->addMarSystem(featureNetwork);

	MarSystem* statistics = mng.create("Fanout", "statistics2");
	statistics->addMarSystem(mng.create("Mean", "mn"));
	statistics->addMarSystem(mng.create("StandardDeviation", "std"));

	MarSystem* total = mng.create("Series", "total");
	total->addMarSystem(acc);
	total->addMarSystem(statistics);
	total->addMarSystem(mng.create("Annotator", "ann"));
	total->addMarSystem(mng.create("WekaSink", "wsink"));

	total->updctrl("mrs_natural/inSamples", winSize);

	if (workspaceDir != EMPTYSTRING)
		wekafname = workspaceDir + wekafname;

	mrs_natural nLabels = l.getNumLabels();

	if (wekafname != EMPTYSTRING)
	{
		cout << "WekaSink nLabels = " << nLabels << endl;
		total->updctrl("WekaSink/wsink/mrs_string/labelNames", l.getLabelNames());
		total->updctrl("WekaSink/wsink/mrs_natural/nLabels", nLabels);
		total->updctrl("WekaSink/wsink/mrs_string/filename", wekafname);
	}

	int i;
	for (i=0; i < l.size(); i++)
	{
		total->updctrl("Accumulator/acc/Series/featureNetwork/SoundFileSource/src/mrs_string/filename", l.entry(i));
		cout << "Label = " << l.labelEntry(i) << endl;
		cout << "LabelID = " << l.labelNum(l.labelEntry(i)) << endl;
		total->updctrl("Annotator/ann/mrs_natural/label", l.labelNum(l.labelEntry(i)));
		cout << "Extracting: " << l.entry(i) << endl;
		total->tick();
	}

	cout << "Extracted features to: " << wekafname << endl;
	string outCollection;
	if (workspaceDir != EMPTYSTRING)
		outCollection = workspaceDir + "extract.txt";
	else
		outCollection = "extract.txt";
	l.write(outCollection);
	cout << "Wrote collection to: " << outCollection << endl;
}

int
main(int argc, const char **argv)
{
	MRSDIAG("bextract.cpp - main");

	//////////////////////////////////////////////////////////////////////////
	// Supported Feature extractors
	//////////////////////////////////////////////////////////////////////////
	// list of supported feature extractors and their creation functions
	featExtractors["MPL_FILE"] = &createExtractorFromFile;
	featExtractors["STFT"] = &createSTFTextractor;
	featExtractors["MFCC"] = &createMFCCextractor;
	featExtractors["STFTMFCC"] = &createSTFTMFCCextractor;
	featExtractors["SCF"] = &createSCFextractor;
	featExtractors["SFM"] = &createSFMextractor;
	featExtractors["SFMSCF"] = &createSFMSCFextractor;
	featExtractors["LSP"] = &createLSPextractor;
	featExtractors["LPCC"] = &createLPCCextractor;
	featExtractors["BEAT"] = &createBEATextrator;
	featExtractors["REFACTORED"] = &createSTFTMFCCextractor;

	// short description of each extractor
	featExtractorDesc["MPL_FILE"] = "not yet implemented...";
	featExtractorDesc["STFT"] = "Centroid, Rolloff, Flux, ZeroCrossings ";
	featExtractorDesc["MFCC"] = "Mel-frequency Cepstral Coefficients ";
	featExtractorDesc["STFTMFCC"] = "Centroid, Rolloff Flux, ZeroCrossings, Mel-frequency Cepstral Coefficients";
	featExtractorDesc["SCF"] = "Spectral Crest Factor (MPEG-7)";
	featExtractorDesc["SFM"] = "Spectral Flatness Measure (MPEG-7)";
	featExtractorDesc["SFMSCF"] = "SCF and SFM features";
	featExtractorDesc["LSP"] = "Linear Spectral Pairs";
	featExtractorDesc["LPCC"] = "LPC derived Cepstral coefficients ";
	featExtractorDesc["BEAT"] = "Beat histogram features";
	featExtractorDesc["REFACTORED"] = "Dummy extractor for refactored bextract";

	//////////////////////////////////////////////////////////////////////////

	string progName = argv[0];
	if (argc == 1)
	{
		printUsage(progName);
		return 0;
	}

	initOptions();
	cmd_options.readOptions(argc, argv);
	loadOptions();

	if (helpopt)
		printHelp(progName);

	if (usageopt)
		printUsage(progName);

	//////////////////////////////////////////////////////////////////////////
	// Print analysis options
	//////////////////////////////////////////////////////////////////////////
	cout << endl;
	cout << "Window Size (in samples): " << winSize << endl;
	cout << "Hop Size (in samples): " << hopSize << endl;
	cout << "Memory Size (in analysis windows):" << memSize << endl;
	cout << "Accumulator size (in analysis windows):" << accSize_ << endl;
	cout << endl;
	cout << "Extractor = " << extractorName << endl;
	cout << endl;

	cout << "collectionName = " << collectionName << endl;
	if (collectionName != EMPTYSTRING)
	{
		if (extractorName.substr(0,2) == "SV")
			extractorName = extractorName.substr(2, extractorName.length());
		mirex_bextract();
		exit(0);
	}

	int i = 0;
	Collection l;

	string classNames = "";
	vector<Collection> cls;

	vector<string> soundfiles = cmd_options.getRemaining();
	vector<string>::iterator sfi;


	for (sfi = soundfiles.begin(); sfi != soundfiles.end(); ++sfi)
	{
		string sfname = *sfi;
		Collection l;
		readCollection(l,sfname);

		if (!l.hasLabels())
		{
			l.labelAll(l.name());
			classNames += (l.name()+',');
		}


		cls.push_back(l);
		i++;
	}

	Collection single;
	single.concatenate(cls);
	if (single.getSize() == 0)
	{
		MRSERR("Collection has no files  - exiting");
		exit(1);
	}

	if (shuffle_)
		single.shuffle();

	if (workspaceDir != EMPTYSTRING)
		single.write(workspaceDir + "bextract_single.mf");
	else
		single.write("bextract_single.mf");


	string extractorStr = extractorName;

	//SINGLE-VALUE Extractor
	if (extractorStr.substr(0,2) == "SV")
	{
		bool withBeatFeatures = extractorName.substr(extractorName.length()-4, extractorName.length()) == "BEAT";
		string extrName = extractorName.substr(2, extractorName.length());

		cout << "extrName = " << extrName << endl;

		if(featExtractors.find(extrName)== featExtractors.end())
		{
			cout << extractorStr << ": Unsupported extractor!" << endl;
			return 1;
		}

		bextract_trainAccumulator(cls, i, pluginName, classNames, wekafname, memSize, extrName,
			withBeatFeatures);
	}
	//REMOVE_SILENCE Extractor
	else if (extractorStr.substr(0,2) == "RS")
	{
		string extrName = extractorName.substr(2, extractorName.length());
		if(featExtractors.find(extrName)== featExtractors.end())
		{
			cout << extractorStr << ": Unsupported extractor!" << endl;
			return 1;
		}
		bextract_train_rmsilence(cls, i, pluginName, classNames, wekafname, memSize, extrName, classifierName);
	}
	else if (extractorStr == "STEREOSPS")
	{
		bextract_trainStereoSPS(cls, classNames, wekafname, memSize);
	}
	else if (extractorStr == "STEREOMFCC")
	{
		bextract_trainStereoMFCC(cls, classNames, wekafname, memSize);
	}
	else if (extractorStr == "STEREOSPSMFCC")
	{
		bextract_trainStereoSPSMFCC(cls, classNames, wekafname, memSize);
	}
	//---------------- ADRess extractors -----------------------------
	else if (extractorStr == "ADRessSTEREOSPS")
	{
		bextract_trainADRessStereoSPS(cls, classNames, wekafname, memSize);
	}
	else if (extractorStr == "ADRessSTEREOSPSMFCC")
	{
		bextract_trainADRessStereoSPSMFCC(cls, classNames, wekafname, memSize);
	}
	//----------------------------------------------------------------
	else if (extractorStr == "BEAT")
	{
		bool withBeatFeatures = true;
		string extrName;
		extrName = extractorStr;
		cout << "extrName = " << extrName << endl;
		bextract_trainAccumulator(cls, i, pluginName, classNames, wekafname, memSize, extrName,
			withBeatFeatures);
	}
	//NORMAL Extractor
	else
	{
		if(featExtractors.find(extractorStr)== featExtractors.end())
		{
			cout << extractorStr << ": Unsupported extractor!" << endl;
			return 1;
		}
		if (extractorStr != "REFACTORED")
		{
			bextract_train(cls, single, i, pluginName, classNames, wekafname, memSize,
				extractorName, classifierName);
		}
		else
		{
			if (classifierName == EMPTYSTRING)
				classifierName = DEFAULT_CLASSIFIER;

			bextract_train_refactored(pluginName, wekafname, memSize, classifierName, single_vector_);
		}
	}

	return 0;
}








