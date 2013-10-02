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

#ifndef MARSYAS_PEUTILITIES_H
#define MARSYAS_PEUTILITIES_H

#include <marsyas/system/MarSystemManager.h>
#include <marsyas/realvec.h>
#include <marsyas/export.h>
#include <fstream>
#include <string>

namespace Marsyas
{
/**
\file
\ingroup none
\brief Utilties for peaks extraction project


\todo WhyTF does PeUtilities exist in the global namespace?  Fix this;
make them a static class or delete the whole thing or something.
*/



// 	typedef enum {
// 		pkFrequency,
// 		pkAmplitude,
// 		pkPhase,
// 		pkDeltaFrequency,
// 		pkDeltaAmplitude,
// 		pkFrame,
// 		pkGroup,
// 		pkVolume,
// 		pkPan,
// 		pkBinLow,
// 		pkBin,
// 		pkBinHigh,
// 		nbPkParameters
// 	} pkParameter;
//
//
//
// 	int peaks2M (realvec&, realvec&, realvec&, mrs_natural maxNbPeaks, mrs_natural *nbPkFrame, mrs_natural start=0);
// 	void peaks2V (realvec&, realvec&, realvec&, mrs_natural maxNbPeaks, mrs_natural label=-1);
//
// 	void extractParameter(realvec&, std::vector<realvec>&, pkParameter, mrs_natural);
//
// 	void selectPeaks (realvec&, realvec&, pkParameter type, int value, int maxNbPeaks);
//
// 	void updateLabels(realvec&, realvec&);
//
// 	mrs_real correlatePeakSets(realvec&, realvec&, realvec&, realvec&);
// 	mrs_real cosinePeakSets(realvec&, realvec&, realvec&, realvec&, realvec&, realvec&, realvec&, realvec&, realvec&, realvec&, mrs_natural);
//
// 	mrs_real compareTwoPeakSets(realvec&, realvec&, realvec&, realvec&);
// 	mrs_real compareTwoPeakSets2(realvec&, realvec&, realvec&, realvec&);
// 	mrs_real compareTwoPeakSets3(realvec&, realvec&, realvec&, realvec&);

marsyas_EXPORT	void synthNetCreate(MarSystemManager *mng, std::string outsfname, bool microphone, mrs_natural synType, bool residual=0);
marsyas_EXPORT	void synthNetConfigure(MarSystem *pvseries, std::string sfName, std::string outsfname, std::string ressfname, std::string panningInfo, mrs_natural nbChannels, mrs_natural Nw,
                                       mrs_natural D, mrs_natural S, mrs_natural accSize, bool microphone, mrs_natural synType, mrs_natural bopt, mrs_natural delay, mrs_real fs, bool residual=0);

// 	mrs_real harmonicWeighting(mrs_real, mrs_real, mrs_real);
// 	mrs_real harmonicWeightingBasic(mrs_real, mrs_real);
// 	void discrete2labels(realvec &labels, realvec& n, mrs_natural nbClusters, mrs_natural nbPeaks);
//
// 	void peakStore(realvec& peaks, std::string filename, mrs_real fs, mrs_natural hopSize);
// 	void peakLoad(realvec& peaks, std::string filename, mrs_real &fs, mrs_natural &nbSines, mrs_natural &nbFrames, mrs_natural &hopSize, bool tf_format = true);
//
// 	void computeHarmonicityMap(realvec& map, mrs_natural nbBins);
}

#endif

