/*
** Copyright (C) 2000 George Tzanetakis <gtzan@cs.princeton.edu>
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



#ifndef MARSYAS_MultiPitch_H
#define MARSYAS_MultiPitch_H

#include "MarSystem.h"	
#include "Filter.h"
#include "HalfWaveRectifier.h"
#include "AutoCorrelationFFT.h"
#include "PeakEnhancer.h"
#include "Series.h"
#include "Fanout.h"
#include "Sum.h"
#include "Parallel.h"

namespace Marsyas
{
/** 
    \class MultiPitch
    \brief Multiple pitch extractor

    Multiple pitch extractor based on the paper by 
	T.Tolonen and M.Karjalainen "A computationally efficient multipitch analysis 
	model IEEE Trans.on Speech and Audio Processing vol 8, num 6. 2000.

	Output is a vector of pitch candidates of half the length of the input signal (Pitches correspond to the frequencies in the fft of the input signal)

	Controls:
	-none
*/
class MultiPitch: public MarSystem
{
  private: 
	//Add specific controls needed by this MarSystem.
	void addControls();
	
	MarSystem* lpf1_;
	MarSystem* lpf2_;
	MarSystem* hpf1_;
	MarSystem* hwr_;
	MarSystem* hwr2_;
	MarSystem* hwr3_;
	MarSystem* autocorlo_;
	MarSystem* autocorhi_;
	MarSystem* sum_;
	MarSystem* pe2_;
	MarSystem* pe3_;
	MarSystem* pe4_;

	MarSystem* fan_;
	MarSystem* hinet_;
	MarSystem* lonet_;

	MarSystem* net_;
	MarSystem* par_;
	MarSystem* p_;
  
	//mrs_string resaModeOld_;
	void myUpdate(MarControlPtr sender);

public:

	MultiPitch(std::string name);
	MultiPitch(const MultiPitch& a);
	~MultiPitch();
	MarSystem* clone() const;  

	void myProcess(realvec& in, realvec& out);
  
};
}

#endif
