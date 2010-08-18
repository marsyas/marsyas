/*
** Copyright (C) 1998-2010 George Tzanetakis <gtzan@cs.uvic.ca>
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

#include "common.h"
#include "PeakViewMerge.h"
#include "peakView.h"


using namespace Marsyas;

using std::abs;

static const mrs_real freqResolution	= 15.;	// Hertz

static inline mrs_natural FindDuplicate (peakView *peaks, mrs_real frequency, mrs_natural numPeaks)
{
	for (mrs_natural i = 0; i < numPeaks; i++)	
	{
		mrs_real freq = (*peaks)(i, peakView::pkFrequency);
		if (abs(freq - frequency) <= freqResolution)
			return i;
	}
	return -1;
}

static inline void WriteOutput (peakView &Out, peakView *In, mrs_natural inputIdx, mrs_natural outputIdx)
{
	for (mrs_natural i = 0; i < peakView::nbPkParameters; i++)	
		Out(outputIdx, (peakView::pkParameter)i)	= (*In)(inputIdx, (peakView::pkParameter)i);
}


PeakViewMerge::PeakViewMerge(mrs_string name):MarSystem("PeakViewMerge",name)
{
	addControls();
}

PeakViewMerge::PeakViewMerge(const PeakViewMerge& a):MarSystem(a)
{
	ctrl_mode_ = getctrl("mrs_string/mode"); 
}

PeakViewMerge::~PeakViewMerge()
{
}

MarSystem* 
PeakViewMerge::clone() const 
{
	return new PeakViewMerge(*this);
}

void 
PeakViewMerge::addControls()
{
	addctrl("mrs_string/mode", "AND", ctrl_mode_);
}


void
PeakViewMerge::myUpdate(MarControlPtr sender)
{
	(void) sender;
	MRSDIAG("PeakViewMerge.cpp - PeakViewMerge:myUpdate");

	ctrl_onObservations_->setValue(ctrl_inObservations_, NOUPDATE);
	ctrl_onSamples_->setValue(ctrl_inSamples_, NOUPDATE);
	ctrl_osrate_->setValue(ctrl_israte_, NOUPDATE);
	ctrl_onObsNames_->setValue(ctrl_inObsNames_, NOUPDATE);
}

void 
PeakViewMerge::myProcess(realvec& in, realvec& out)
{
	peakView	*In[kNumMatrices],
				Out (out);
	mrs_natural i,
				numPeaks[kNumMatrices],
				numRows		= in.getRows ()/kNumMatrices,   // this assumes that frameMaxNumPeaks is equal in the input files
//				currFrame	= (mrs_natural)(in(numRows*peakView::pkFrame/peakView::nbPkParameters,0) + .1),
				outputIdx	= 0;
	out.setval(0.);
	for (i = 0; i < kNumMatrices; i++)
	{
		peakViewIn_[i].stretch (numRows, in.getCols ());
		in.getSubMatrix (i*numRows, 0, peakViewIn_[i]);
		In[i]		= new peakView(peakViewIn_[i]);
		numPeaks[i]	= In[i]->getTotalNumPeaks ();
	}

	if (ctrl_mode_->to<mrs_string>() == "OR")
	{
		// write all entries of the second peakView to output
		for (i = 0; i < numPeaks[1]; i++)
		{
			WriteOutput (Out, In[1], i, outputIdx);
			outputIdx++;
		}

		// write all entries of the first peakView to output except duplicates
		for (i = 0; i < numPeaks[0]; i++)
		{
			mrs_natural Idx;
			for (mrs_natural k = 1; k < kNumMatrices; k++)
				Idx	= FindDuplicate (In[k], (*In[0])(i, peakView::pkFrequency), numPeaks[k]);

			if (Idx < 0)
			{
				WriteOutput (Out, In[0], i, outputIdx);
				outputIdx++;
			}
		}
	}
	else
	{
		// find duplicates and write only them to output
		for (i = 0; i < numPeaks[0]; i++)
		{
			mrs_natural Idx;
			for (mrs_natural k = 1; k < kNumMatrices; k++)
				Idx	= FindDuplicate (In[k], (*In[0])(i, peakView::pkFrequency), numPeaks[k]);

			if (Idx >= 0)
			{
				WriteOutput (Out, In[0], i, outputIdx);
				outputIdx++;
			}
		}
	}
}

