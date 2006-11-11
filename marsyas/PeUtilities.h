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
using std::ofstream;
#include "PeClusters.h"

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
	nbPeaksParameters
	} pkParameter;


	int peaks2M (realvec&, realvec&, realvec&, int maxNbPeaks);
	void peaks2V (realvec&, realvec&, realvec&, int maxNbPeaks);

	void extractParameter(realvec&, realvec&, pkParameter);

	void selectPeaks (realvec&, realvec&, pkParameter type, int value, int maxNbPeaks);

	void updateLabels(realvec&, realvec&);

	mrs_real compareTwoPeakSets(realvec&, realvec&, realvec&, realvec&);

}

#endif

