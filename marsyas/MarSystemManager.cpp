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
  // registerPrototype("Filter", new Filter("filterp"));
  // registerPrototype("ERB", new ERB("erbp"));

  registerPrototype("ClipAudioRange", new ClipAudioRange("clpr"));
  registerPrototype("HarmonicEnhancer", new HarmonicEnhancer("hepr"));
  registerPrototype("Reassign", new Reassign("reassignpr"));
  registerPrototype("SilenceRemove", new SilenceRemove("silenceremovepr"));
  registerPrototype("NormMaxMin", new NormMaxMin("normmaxminpr"));
  registerPrototype("Normalize", new Normalize("normalizepr"));

  registerPrototype("LPC", new LPC("lpcpr"));
  registerPrototype("LPCwarped", new LPCwarped("lpcwarppr"));
  registerPrototype("LPCResyn", new LPCResyn("lpcresynpr"));
  registerPrototype("LSP", new LSP("lsppr"));


  registerPrototype("SOM", new SOM("sompr"));

  // example of creating a prototype for a composite MarSystem 
  MarSystem* pvocpr = new Series("pvocpr");
  pvocpr->addMarSystem(new ShiftInput("si"));
  pvocpr->addMarSystem(new PvFold("fo"));
  pvocpr->addMarSystem(new Spectrum("spk"));
  pvocpr->addMarSystem(new PvConvert("conv"));
  pvocpr->addMarSystem(new PvOscBank("ob"));
  pvocpr->addMarSystem(new ShiftOutput("so"));
  pvocpr->addMarSystem(new Gain("gt"));
  
  pvocpr->linkctrl("mrs_natural/Decimation", 
		   "ShiftInput/si/mrs_natural/Decimation");
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
}

MarSystemManager::~MarSystemManager()
{
  map<string, MarSystem *>::const_iterator iter;
  
  for (iter=registry.begin(); iter != registry.end(); ++iter)
    {
      delete iter->second;
    }
  registry.clear();
}

void 
MarSystemManager::registerPrototype(string name, MarSystem *marsystem)
{
  registry[name] = marsystem;
}

MarSystem* 
MarSystemManager::getPrototype(string type) 
{
  
  if (registry.find(type) != registry.end())
    return (MarSystem *)(registry[type])->clone();
  else 
    {
      MRSWARN("MarSystemManager::getPrototype No prototype found for " + type);
      return 0;
    }
}

MarSystem* 
MarSystemManager::create(string type, string name) 
{
  if (registry.find(type) != registry.end())
    {
      MarSystem* m = (MarSystem *)(registry[type])->clone();
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

  cout << "mtype = " << mtype << endl;
  
  
  
  is >> skipstr >> skipstr >> skipstr;
  string mname;
  is >> mname;

  cout << "mname = " << mname << endl;
  
  

  MarSystem* msys = getPrototype(mtype);
  
  if (msys == 0)
    {
      MRSWARN("MarSystem::getMarSystem - MarSystem not supported");
      return 0;
    }
  else
    {
      msys->setName(mname);
      is >> (msys->ncontrols_);
      
      msys->update();
      
      workingSet[msys->getName()] = msys; // add to workingSet

      
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
	      
	      if (pos == 0) 
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

    for (iter=registry.begin(); iter != registry.end(); ++iter)
    {
        retVal.push_back (iter->first);
    }

    return retVal;
} 


	
// Added by Stuart Bray Dec 2004. invoked by MslModel
map<string, MarSystem*> MarSystemManager::getWorkingSet(istream& is) {
  getMarSystem(is);
  return workingSet;
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
   return (registry.find(name) != registry.end());
}
