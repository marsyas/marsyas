/*
** Copyright (C) 1998-2006 George Tzanetakis <gtzan@cs.uvic.ca>
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
\class MarSystemManager
\brief MarSystemManager of MarSystem objects

Combines a series of MarSystem objects to a single MarSystem 
corresponding to executing the System objects one after the other 
in sequence. 
*/

#include "MarSystemManager.h"
#include "MarSystem.h"

#include "Gain.h"
#include "HalfWaveRectifier.h"
#include "common.h"
#include "Series.h"
#include "Parallel.h"
#include "Fanin.h" 
#include "Fanout.h" 
#include "TimeStretch.h" 
#include "Peaker.h"
#include "Peaker1.h"
#include "MaxArgMax.h"
#include "MinArgMin.h"
#include "AutoCorrelation.h"
#include "Spectrum.h"
#include "InvSpectrum.h"
#include "Negative.h"
#include "PvConvert.h"
#include "PvFold.h" 
#include "PvOscBank.h" 
#include "ShiftInput.h" 
#include "ShiftOutput.h"
#include "Shifter.h"
#include "PvUnconvert.h"
#include "PvOverlapadd.h"
#include "PvConvolve.h"
#include "AuFileSource.h" 
#include "WavFileSource.h" 
#include "SineSource.h"
#include "NoiseSource.h"
#include "AudioSink.h"

#include "PeConvert.h"
#include "PeClust.h"
#include "PeOverlapadd.h"
#include "PeResidual.h"
#include "RealvecSource.h"

#include "Windowing.h"

#include "AuFileSink.h"
#include "WavFileSink.h"
#include "Hamming.h"
#include "PowerSpectrum.h"
#include "Centroid.h"
#include "Rolloff.h" 
#include "Flux.h" 
#include "ZeroCrossings.h"
#include "Memory.h"
#include "Mean.h" 
#include "StandardDeviation.h"
#include "PlotSink.h"
#include "GaussianClassifier.h"
#include "SoundFileSource.h"
#include "SoundFileSink.h" 
#include "Confidence.h"
#include "Rms.h"
#include "WekaSink.h" 
#include "MFCC.h"
#include "SCF.h"
#include "SFM.h"
#include "Accumulator.h"
#include "Shredder.h"
#include "WaveletPyramid.h"
#include "WaveletBands.h"
#include "FullWaveRectifier.h"
#include "OnePole.h"
#include "Norm.h"
#include "Sum.h"
#include "DownSampler.h"
#include "PeakPeriods2BPM.h"
#include "Histogram.h"
#include "BeatHistoFeatures.h"
#include "FM.h"
#include "Annotator.h" 
#include "ZeroRClassifier.h"
#include "KNNClassifier.h"
#include "Kurtosis.h"
#include "Skewness.h"
#include "ViconFileSource.h"
#include "AudioSource.h"
#include "ClassOutputSink.h"
#include "Filter.h" 
#include "ERB.h"
#include "ClipAudioRange.h"
#include "HarmonicEnhancer.h"
#include "Reassign.h"
#include "SilenceRemove.h"
#include "NormMaxMin.h" 
#include "Normalize.h"
#include "SMO.h"
#include "Plucked.h" 
#include "Delay.h"
#include "LPC.h"
#include "LPCwarped.h"
#include "LPCResyn.h"
#include "LPCC.h"
#include "LSP.h"
#include "SOM.h"

#ifndef WIN32 
#include "NetworkTCPSink.h"
#include "NetworkTCPSource.h"
#include "NetworkUDPSink.h"
#include "NetworkUDPSource.h"
#endif

#ifdef CYGWIN
#include "NetworkTCPSink.h"
#include "NetworkTCPSource.h"
#include "NetworkUDPSink.h"
#include "NetworkUDPSource.h"
#endif 

using namespace std;
using namespace Marsyas;

MarSystemManager::MarSystemManager()
{
	registerPrototype("Gain", new Gain("gp"));
	registerPrototype("SoundFileSource", new SoundFileSource("sfp"));
	registerPrototype("HalfWaveRectifier", new HalfWaveRectifier("hwrp"));
	registerPrototype("AutoCorrelation", new AutoCorrelation("acrp"));
	registerPrototype("Series", new Series("srp"));
	registerPrototype("Fanin", new Fanin("finp"));
	registerPrototype("Fanout", new Fanout("fonp"));
	registerPrototype("Parallel", new Parallel("parallelp"));

	registerPrototype("TimeStretch", new TimeStretch("tscp"));

	registerPrototype("Peaker", new Peaker("pkrp"));
	registerPrototype("Peaker1", new Peaker1("pkr1pr"));
	registerPrototype("MaxArgMax", new MaxArgMax("mxrp"));
	registerPrototype("MinArgMin", new MinArgMin("mnrp"));
	registerPrototype("Spectrum", new Spectrum("spkp"));
	registerPrototype("InvSpectrum", new InvSpectrum("ispkp"));
	registerPrototype("Negative", new Negative("negp"));
	registerPrototype("PvConvert", new PvConvert("pvconvp"));
	registerPrototype("PvUnconvert", new PvUnconvert("pvuconvp"));
	registerPrototype("PvFold", new PvFold("pvfp"));
	registerPrototype("PvOverlapadd", new PvOverlapadd("pvovlfp"));
	registerPrototype("PvOscBank", new PvOscBank("pvoscp"));
	registerPrototype("ShiftInput", new ShiftInput("sip"));
	registerPrototype("ShiftOutput", new ShiftOutput("sop"));
	registerPrototype("Shifter", new Shifter("sp"));
	registerPrototype("PvConvolve", new PvConvolve("pvconvpr"));

	registerPrototype("PeConvert", new PeConvert("peconvp"));
	registerPrototype("PeClust", new PeClust("peclust"));
	registerPrototype("PeOverlapadd", new PeOverlapadd("peovlfp"));
	registerPrototype("PeResidual", new PeResidual("peres"));
	registerPrototype("RealvecSource", new RealvecSource("realvecSrc"));

	registerPrototype("AuFileSource", new AuFileSource("aufp"));
	registerPrototype("WavFileSource", new WavFileSource("wavfp"));
	registerPrototype("SoundFileSink", new SoundFileSink("sfsp"));
	registerPrototype("AudioSink", new AudioSink("audiosinkp"));
	registerPrototype("AuFileSink", new AuFileSink("ausinkp"));
	registerPrototype("WavFileSink", new WavFileSink("wavsinkp"));

	registerPrototype("Hamming", new Hamming("hmp"));  
	registerPrototype("Windowing", new Windowing("win"));
	registerPrototype("PowerSpectrum", new PowerSpectrum("pspkp"));
	registerPrototype("Centroid", new Centroid("cntrp"));
	registerPrototype("Rolloff", new Rolloff("rlfp"));
	registerPrototype("Flux", new Flux("flup"));
	registerPrototype("ZeroCrossings", new ZeroCrossings("zcrsp"));
	registerPrototype("Memory", new Memory("memp"));
	registerPrototype("Mean", new Mean("meanp"));
	registerPrototype("StandardDeviation", new StandardDeviation("stdp"));
	registerPrototype("PlotSink", new PlotSink("plotsp"));
	registerPrototype("GaussianClassifier", new GaussianClassifier("gaussp"));

	registerPrototype("Confidence", new Confidence("confp"));
	registerPrototype("Rms", new Rms("rms"));
	registerPrototype("WekaSink", new WekaSink("wsink"));
	registerPrototype("MFCC", new MFCC("mfcc"));
	registerPrototype("SCF", new SCF("scf"));
	registerPrototype("SFM", new SFM("sfm"));
	registerPrototype("Accumulator", new Accumulator("acc"));
	registerPrototype("Shredder", new Shredder("shred"));

	registerPrototype("WaveletPyramid", new WaveletPyramid("wvpyramid"));
	registerPrototype("WaveletBands",   new WaveletBands("wvbands"));
	registerPrototype("FullWaveRectifier", new FullWaveRectifier("fwr"));
	registerPrototype("OnePole", new OnePole("lpf"));
	registerPrototype("Norm", new Norm("norm"));
	registerPrototype("Sum", new Sum("sum"));
	registerPrototype("DownSampler", new DownSampler("ds"));

	registerPrototype("PeakPeriods2BPM", new PeakPeriods2BPM("p2bpm"));
	registerPrototype("Histogram", new Histogram("histop"));
	registerPrototype("BeatHistoFeatures", new BeatHistoFeatures("bhfp"));
	registerPrototype("SineSource", new SineSource("sinesp"));
	registerPrototype("NoiseSource", new NoiseSource("noisesrcsp"));
	registerPrototype("FM", new FM("fmp"));
	registerPrototype("Annotator", new Annotator("anonp"));
	registerPrototype("ZeroRClassifier", new ZeroRClassifier("zerorp"));
	registerPrototype("KNNClassifier", new KNNClassifier("knnp"));

	registerPrototype("SMO", new SMO("smopr"));

	registerPrototype("Plucked", new Plucked("pluckedpr"));
	registerPrototype("Delay", new Delay("delaypr"));

	registerPrototype("Kurtosis", new Kurtosis("kurtosisp"));
	registerPrototype("Skewness", new Skewness("Skewnessp"));
	registerPrototype("ViconFileSource", new ViconFileSource("viconfilesourcep"));

	registerPrototype("AudioSource", new AudioSource("audiosourcep"));
	registerPrototype("ClassOutputSink", new ClassOutputSink("classoutputsinkp"));
	registerPrototype("Filter", new Filter("filterp"));
	registerPrototype("ERB", new ERB("erbp"));

	registerPrototype("ClipAudioRange", new ClipAudioRange("clpr"));
	registerPrototype("HarmonicEnhancer", new HarmonicEnhancer("hepr"));
	registerPrototype("Reassign", new Reassign("reassignpr"));
	registerPrototype("SilenceRemove", new SilenceRemove("silenceremovepr"));
	registerPrototype("NormMaxMin", new NormMaxMin("normmaxminpr"));
	registerPrototype("Normalize", new Normalize("normalizepr"));

	registerPrototype("LPC", new LPC("lpcpr"));
	registerPrototype("LPCwarped", new LPCwarped("lpcwarppr"));
	registerPrototype("LPCResyn", new LPCResyn("lpcresynpr"));
	registerPrototype("LPCC", new LPCC("lpccpr"));
	registerPrototype("LSP", new LSP("lsppr"));

	registerPrototype("SOM", new SOM("sompr"));

#ifndef WIN32
	registerPrototype("NetworkTCPSink", new NetworkTCPSink("tcpsink"));
	registerPrototype("NetworkTCPSource", new NetworkTCPSource("tcpsource"));
	registerPrototype("NetworkUDPSink", new NetworkUDPSink("udpsink"));
	registerPrototype("NetworkUDPSource", new NetworkUDPSource("udpsource"));
#endif 

#ifdef CYGWIN
	registerPrototype("NetworkTCPSink", new NetworkTCPSink("tcpsink"));
	registerPrototype("NetworkTCPSource", new NetworkTCPSource("tcpsource"));
	registerPrototype("NetworkUDPSink", new NetworkUDPSink("udpsink"));
	registerPrototype("NetworkUDPSource", new NetworkUDPSource("udpsource"));
#endif 

	//////////////////////////////////////////////////////////////////////////
	// Composite MarSystem prototypes
	//////////////////////////////////////////////////////////////////////////

	// texture window analysis composite prototype
	MarSystem* textureStatspr = new Series("tstatspr");
	MarSystem* meanstdpr = new Fanout("meanstdpr");
	meanstdpr->addMarSystem(new Mean("meanpr"));
	meanstdpr->addMarSystem(new StandardDeviation("stdpr"));
	textureStatspr->addMarSystem(new Memory("mempr"));
	textureStatspr->addMarSystem(meanstdpr);
	textureStatspr->linkctrl("mrs_natural/memSize", "Memory/mempr/mrs_natural/memSize");
	textureStatspr->linkctrl("mrs_bool/reset", "Memory/mempr/mrs_bool/reset");
	registerPrototype("TextureStats", textureStatspr);

	// Power spectrum composite prototype
	MarSystem* pspectpr = new Series("pspectpr");
	pspectpr->addMarSystem(create("ShiftInput", "si"));
	pspectpr->addMarSystem(create("Hamming", "hamming"));
	pspectpr->addMarSystem(create("Spectrum","spk"));
	pspectpr->updctrl("Spectrum/spk/mrs_real/cutoff", 1.0);
	pspectpr->addMarSystem(create("PowerSpectrum", "pspk"));
	pspectpr->updctrl("PowerSpectrum/pspk/mrs_string/spectrumType","power");
	pspectpr->linkctrl("mrs_real/cutoff","Spectrum/spk/mrs_real/cutoff");
	pspectpr->linkctrl("mrs_natural/WindowSize","ShiftInput/si/mrs_natural/WindowSize");
	registerPrototype("PowerSpectrumNet", pspectpr);
	
	// LPC composite prototype
	MarSystem* LPCnetpr = new Series("lpcnetpr");
	// create and configure the pre-emphasis filter as a FIR:
	// H(z) = 1 + aZ-1 ; a = -0.97
	LPCnetpr->addMarSystem(create("Filter", "preEmph"));
	realvec ncoeffs(2);
	realvec dcoeffs(1);
	ncoeffs(0) = 1.0;
	ncoeffs(1) = -0.97;
	dcoeffs(0) = 1.0;
	LPCnetpr->updctrl("Filter/preEmph/mrs_realvec/ncoeffs", ncoeffs);
	LPCnetpr->updctrl("Filter/preEmph/mrs_realvec/dcoeffs", dcoeffs);
	LPCnetpr->addMarSystem(create("ShiftInput", "si"));
	LPCnetpr->addMarSystem(create("Hamming", "ham"));
	LPCnetpr->addMarSystem(create("LPCwarped", "lpc"));
	LPCnetpr->linkctrl("mrs_realvec/preEmphFIR","Filter/preEmph/mrs_realvec/ncoeffs");
	LPCnetpr->linkctrl("mrs_natural/WindowSize","ShiftInput/si/mrs_natural/WindowSize");
	LPCnetpr->linkctrl("mrs_natural/order","LPCwarped/lpc/mrs_natural/order");
	LPCnetpr->linkctrl("mrs_real/lambda","LPCwarped/lpc/mrs_real/lambda");
	LPCnetpr->linkctrl("mrs_real/gamma","LPCwarped/lpc/mrs_real/gamma");
	registerPrototype("LPCnet", LPCnetpr);

	// Phase Vocoder composite prototype 
	MarSystem* pvocpr = new Series("pvocpr");
	pvocpr->addMarSystem(new ShiftInput("si"));
	pvocpr->addMarSystem(new PvFold("fo"));
	pvocpr->addMarSystem(new Spectrum("spk"));
	pvocpr->addMarSystem(new PvConvert("conv"));
	pvocpr->addMarSystem(new PvOscBank("ob"));
	pvocpr->addMarSystem(new ShiftOutput("so"));
	pvocpr->addMarSystem(new Gain("gt"));

	pvocpr->linkctrl("mrs_natural/Decimation", 
		"ShiftInput/si/mrs_natural/Decimation"); // [?] missing WindowSize initialization ?
	pvocpr->linkctrl("mrs_natural/Decimation", 
		"PvFold/fo/mrs_natural/Decimation");
	pvocpr->linkctrl("mrs_natural/Decimation", 
		"PvConvert/conv/mrs_natural/Decimation");
	pvocpr->linkctrl("mrs_natural/Decimation", 
		"ShiftOutput/so/mrs_natural/Decimation");

	pvocpr->linkctrl("mrs_natural/Sinusoids", 
		"PvConvert/conv/mrs_natural/Sinusoids");
	pvocpr->linkctrl("mrs_natural/FFTSize", 
		"PvFold/fo/mrs_natural/FFTSize");

	pvocpr->linkctrl("mrs_natural/WindowSize", 
		"PvFold/fo/mrs_natural/WindowSize");
	pvocpr->linkctrl("mrs_natural/WindowSize", 
		"ShiftOutput/so/mrs_natural/WindowSize");

	pvocpr->linkctrl("mrs_natural/Interpolation", 
		"PvOscBank/ob/mrs_natural/Interpolation");
	pvocpr->linkctrl("mrs_natural/Interpolation", 
		"ShiftOutput/so/mrs_natural/Interpolation");

	pvocpr->linkctrl("mrs_real/PitchShift", 
		"PvOscBank/ob/mrs_real/PitchShift");

	pvocpr->linkctrl("mrs_real/gain", 
		"Gain/gt/mrs_real/gain");

	registerPrototype("PhaseVocoder", pvocpr);

	// prototype for Peak Extraction stuff
	MarSystem* peAnalysePr = new Series("peAnalysePr");
	peAnalysePr->addMarSystem(create("ShiftInput", "si"));
	peAnalysePr->addMarSystem(create("Shifter", "sh"));
	peAnalysePr->addMarSystem(create("Windowing", "wi"));
  MarSystem *parallel = create("Parallel", "par");
	parallel->addMarSystem(create("Spectrum", "spk1"));
	parallel->addMarSystem(create("Spectrum", "spk2"));
	peAnalysePr->addMarSystem(parallel);
	peAnalysePr->addMarSystem(create("PeConvert", "conv"));

	peAnalysePr->linkctrl("mrs_natural/Decimation", 
		"ShiftInput/si/mrs_natural/Decimation");
	peAnalysePr->linkctrl("mrs_natural/WindowSize", 
		"ShiftInput/si/mrs_natural/WindowSize");
	peAnalysePr->linkctrl("mrs_natural/FFTSize", 
		"Windowing/wi/mrs_natural/size");
	peAnalysePr->linkctrl("mrs_string/WindowType", 
		"Windowing/wi/mrs_string/type");
	peAnalysePr->linkctrl("mrs_natural/zeroPhasing", 
		"Windowing/wi/mrs_natural/zeroPhasing");
	peAnalysePr->linkctrl("mrs_natural/Sinusoids", 
		"PeConvert/conv/mrs_natural/Sinusoids");
	peAnalysePr->linkctrl("mrs_natural/Decimation", 
		"PeConvert/conv/mrs_natural/Decimation");
	peAnalysePr->linkctrl("mrs_natural/FFTSize", 
		"PvFold/fo/mrs_natural/FFTSize");

	peAnalysePr->updctrl("Shifter/sh/mrs_natural/shift", 1);

	registerPrototype("PeAnalyse", peAnalysePr);
}

MarSystemManager::~MarSystemManager()
{
	map<string, MarSystem *>::const_iterator iter;

	for (iter=registry_.begin(); iter != registry_.end(); ++iter)
	{
		delete iter->second;
	}
	registry_.clear();
}

void 
MarSystemManager::registerPrototype(string type, MarSystem *marsystem)
{
	//change type_ of composite to the user specified one
	marsystem->setType(type);
	//and register it
	registry_[type] = marsystem;
}

MarSystem* 
MarSystemManager::getPrototype(string type) 
{

	if (registry_.find(type) != registry_.end())
		return (MarSystem *)(registry_[type])->clone();
	else 
	{
		MRSWARN("MarSystemManager::getPrototype No prototype found for " + type);
		return 0;
	}
}

MarSystem* 
MarSystemManager::create(string type, string name) 
{
	if (registry_.find(type) != registry_.end())
	{
		MarSystem* m = (MarSystem *)(registry_[type])->clone();
		m->setName(name);
		return m;
	}

	else 
	{
		MRSWARN("MarSystemManager::getPrototype No prototype found for " + type);
		return 0;
	}
}

MarSystem* 
MarSystemManager::getMarSystem(istream& is)
{
	string skipstr;
	mrs_natural i;
	is >> skipstr;
	string mcomposite;
	bool   isComposite;

	is >> mcomposite;
	string marSystem = "MarSystem";
	string marSystemComposite = "MarSystemComposite";  
	if (mcomposite == marSystem)
		isComposite = false;
	else if (mcomposite == marSystemComposite)
		isComposite = true;
	else
		return 0;

	is >> skipstr >> skipstr >> skipstr;
	string mtype;
	is >> mtype;

	is >> skipstr >> skipstr >> skipstr;
	string mname;
	is >> mname;

	MarSystem* msys = getPrototype(mtype);

	if (msys == 0)
	{
		MRSWARN("MarSystem::getMarSystem - MarSystem not supported");
		return 0;
	}
	else
	{
		msys->setName(mname);
		is >> *msys; //read controls into MarSystem [!]

		msys->update();

		workingSet_[msys->getName()] = msys; // add to workingSet

		is >> skipstr;
		is >> skipstr;
		is >> skipstr;
		is >> skipstr;

		if (skipstr != "links") 
		{
			MRSWARN("Problem with reading links");
			MRSWARN("mtype = " << mtype);
			MRSWARN("mname = " << mname);
			MRSWARN("skipstr = " << skipstr);
		}

		is >> skipstr;

		mrs_natural nLinks;
		is >> nLinks;

		for (i=0; i < nLinks; i++)
		{
			is >> skipstr;
			is >> skipstr;
			is >> skipstr;
			string visible;
			string vshortcname;

			is >> visible;

			string prefix = "/" + mtype + "/" + mname + "/";
			string::size_type pos = visible.find(prefix, 0);

			if (pos == 0) 
				vshortcname = visible.substr(prefix.length(), visible.length());

			is >> skipstr;
			is >> skipstr;
			is >> skipstr;
			is >> skipstr;
			is >> skipstr;
			is >> skipstr;

			mrs_natural nSynonyms = 0;
			is >> nSynonyms;

			vector<string> synonymList;
			synonymList = msys->synonyms_[vshortcname];

			for (int j=0; j < nSynonyms; j++)
			{
				string inside;
				is >> skipstr;
				is >> inside;

				prefix = "/" + mtype + "/" + mname + "/";
				pos = inside.find(prefix, 0);
				string shortcname;

				if (pos == 0) //and what happens if the prefix is not found?!? [?][!]
					shortcname = inside.substr(prefix.length(), inside.length());

				synonymList.push_back(shortcname);
				msys->synonyms_[vshortcname] = synonymList;
			}
		}
		if (isComposite == true)
		{
			is >> skipstr >> skipstr >> skipstr;
			mrs_natural nComponents;
			is >> nComponents;
			for (i=0; i < nComponents; i++)
			{
				MarSystem* cmsys = getMarSystem(is);
				if (cmsys == 0)
					return 0;
				msys->addMarSystem(cmsys);
			}
			msys->update();
		}
	}

	return msys;
}

//
// Added by Adam Parkin, Jul 2005, invoked by MslModel
//
// Returns a list of all registered prototypes
//
vector <string> MarSystemManager::registeredPrototypes()
{
	vector <string> retVal;

	map<string, MarSystem *>::const_iterator iter;

	for (iter=registry_.begin(); iter != registry_.end(); ++iter)
	{
		retVal.push_back (iter->first);
	}

	return retVal;
} 

// Added by Stuart Bray Dec 2004. invoked by MslModel
map<string, MarSystem*> MarSystemManager::getWorkingSet(istream& is) 
{
	getMarSystem(is);
	return workingSet_;
}

//
// Added by Adam Parkin Jul 2005, invoked by MslModel
//
// Takes as an argument a string, if the string corresponds
// to a registered MarSystem, the function returns true,
// otherwise returns false
//
bool MarSystemManager::isRegistered (string name)
{
	return (registry_.find(name) != registry_.end());
}
