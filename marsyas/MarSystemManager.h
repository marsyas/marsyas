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
    \brief MarSystemManager 
    

*/

#ifndef MARSYAS_MARSYSTEMMANAGER_H
#define MARSYAS_MARSYSTEMMANAGER_H




#include "MarSystem.h"
#include "Gain.h"
#include "HalfWaveRectifier.h"
#include "common.h"
#include "Series.h"
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
#include "PvUnconvert.h"
#include "PvOverlapadd.h"
#include "PvConvolve.h"
#include "AuFileSource.h" 
#include "WavFileSource.h" 
#include "SineSource.h"
#include "NoiseSource.h"
#include "AudioSink.h"

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


class MarSystemManager
{
protected:
  std::map<std::string, MarSystem*> registry;
  std::map<std::string, MarSystem*> workingSet; 
  
public:
  MarSystemManager();
  ~MarSystemManager();
  void registerPrototype(std::string name, MarSystem *);
  MarSystem* getPrototype(std::string type);
  MarSystem* create(std::string type, std::string name);
  MarSystem* getMarSystem(std::istream& is);

  std::map<std::string, MarSystem*> getWorkingSet(std::istream& is);

  bool isRegistered (std::string name);

  std::vector <std::string> registeredPrototypes(); 
};



#endif

	

	
	
