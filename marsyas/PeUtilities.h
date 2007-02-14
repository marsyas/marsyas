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
    \class PeUtilities
    \brief utilties for peaks extraction project

  
*/

#ifndef MARSYAS_PEUTILITIES_H
#define MARSYAS_PEUTILITIES_H

#include "realvec.h"
#include <fstream>
//using std::ofstream;
#include "PeClusters.h"
#include "MarSystemManager.h"
#include <string>

#define EMPTYSTRING "MARSYAS_EMPTY"

namespace Marsyas
{

	typedef enum {
	pkFrequency,
	pkAmplitude,
	pkPhase,
	pkDeltaFrequency,
	pkDeltaAmplitude,
	pkTime,
	pkGroup,
	nbPkParameters
	} pkParameter;


	int peaks2M (realvec&, realvec&, realvec&, mrs_natural maxNbPeaks, mrs_natural *nbPkFrame);
	void peaks2V (realvec&, realvec&, realvec&, mrs_natural maxNbPeaks, mrs_natural label=-1);

	void extractParameter(realvec&, std::vector<realvec>&, pkParameter, mrs_natural);

	void selectPeaks (realvec&, realvec&, pkParameter type, int value, int maxNbPeaks);

	void updateLabels(realvec&, realvec&);

	mrs_real correlatePeakSets(realvec&, realvec&, realvec&, realvec&);
mrs_real cosinePeakSets(realvec&, realvec&, realvec&, realvec&, realvec&, realvec&, realvec&, realvec&, realvec&, realvec&, mrs_natural);

	mrs_real compareTwoPeakSets(realvec&, realvec&, realvec&, realvec&);
	mrs_real compareTwoPeakSets2(realvec&, realvec&, realvec&, realvec&);
	mrs_real compareTwoPeakSets3(realvec&, realvec&, realvec&, realvec&);

	void synthNetCreate(MarSystemManager *mng, std::string outsfname, bool microphone, mrs_natural synType);

void
synthNetConfigure(MarSystem *pvseries, std::string sfName, std::string outsfname, std::string ressfname, mrs_natural Nw, 
									mrs_natural D, mrs_natural S, mrs_natural accSize, bool microphone, mrs_natural synType, mrs_natural bopt, mrs_natural delay);

mrs_real harmonicWeighting(mrs_real, mrs_real, mrs_real);
mrs_real harmonicWeightingBasic(mrs_real, mrs_real);
void discrete2labels(realvec &labels, realvec& n, mrs_natural nbClusters, mrs_natural nbPeaks);
}

#endif

