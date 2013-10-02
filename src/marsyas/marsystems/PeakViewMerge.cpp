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

#include "../common_source.h"
#include "PeakViewMerge.h"
#include <marsyas/peakView.h>


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
  ctrl_mode_			= getctrl("mrs_string/mode");
  ctrl_totalNumPeaks_ = getctrl("mrs_natural/totalNumPeaks");
  ctrl_frameMaxNumPeaks1_ = getctrl("mrs_natural/frameMaxNumPeaks1");
  ctrl_frameMaxNumPeaks2_ = getctrl("mrs_natural/frameMaxNumPeaks2");
  ctrl_noNegativeGroups_ = getctrl("mrs_bool/discardNegativeGroups");
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
  addControl("mrs_string/mode", "AND", ctrl_mode_);
  addControl("mrs_natural/totalNumPeaks", 0, ctrl_totalNumPeaks_);
  addControl("mrs_natural/frameMaxNumPeaks1", 0, ctrl_frameMaxNumPeaks1_);
  addControl("mrs_natural/frameMaxNumPeaks2", 0, ctrl_frameMaxNumPeaks2_);
  addControl("mrs_bool/discardNegativeGroups", false, ctrl_noNegativeGroups_);
}


void
PeakViewMerge::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)
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
  mrs_natural i, rowIdx = 0,
                 numPeaks[kNumMatrices],
                 outputIdx	= 0;
  const mrs_bool discNegGroups	= ctrl_noNegativeGroups_->to<mrs_bool>();

  out.setval(0.);

  for (i = 0; i < kNumMatrices; i++)
  {
    mrs_natural	numRows		= (i==kMat1)? ctrl_frameMaxNumPeaks1_->to<mrs_natural>() :  ctrl_frameMaxNumPeaks2_->to<mrs_natural>();
    numRows					*= peakView::nbPkParameters;
    if (numRows == 0) // if the controls have not been set assume both matrixes to be of equal size
      numRows	= in.getRows ()/kNumMatrices;
    peakViewIn_[i].stretch (numRows, in.getCols ());
    in.getSubMatrix (rowIdx, 0, peakViewIn_[i]);
    rowIdx		+= numRows;
    In[i]		= new peakView(peakViewIn_[i]);
    numPeaks[i]	= In[i]->getTotalNumPeaks ();
  }

  if (ctrl_mode_->to<mrs_string>() == "OR")
  {
    // write all entries of the second peakView to output
    for (i = 0; i < numPeaks[1]; i++)
    {
      if (discNegGroups && (*In[1])(i,peakView::pkGroup) < 0)
        continue;
      WriteOutput (Out, In[1], i, outputIdx);
      outputIdx++;
    }

    // write all entries of the first peakView to output except duplicates
    for (i = 0; i < numPeaks[0]; i++)
    {
      mrs_natural Idx;
      if (discNegGroups && (*In[0])(i,peakView::pkGroup) < 0)
        continue;
      for (mrs_natural k = 1; k < kNumMatrices; k++)
        Idx	= FindDuplicate (In[k], (*In[0])(i, peakView::pkFrequency), numPeaks[k]);

      if (Idx < 0)
      {
        WriteOutput (Out, In[0], i, outputIdx);
        outputIdx++;
      }
    }
  }
  else if (ctrl_mode_->to<mrs_string>() == "AND")
  {
    // find duplicates and write only them to output
    for (i = 0; i < numPeaks[0]; i++)
    {
      mrs_natural Idx;
      if (discNegGroups && (*In[0])(i,peakView::pkGroup) < 0)
        continue;
      for (mrs_natural k = 1; k < kNumMatrices; k++)
        Idx	= FindDuplicate (In[k], (*In[0])(i, peakView::pkFrequency), numPeaks[k]);

      if (Idx >= 0)
      {
        if (discNegGroups && (*In[1])(Idx,peakView::pkGroup) < 0)
          continue;
        WriteOutput (Out, In[0], i, outputIdx);
        outputIdx++;
      }
    }
  }
  else if (ctrl_mode_->to<mrs_string>() == "ANDOR")
  {
    // keep the input[0] peaks that are not in input[1]
    for (i = 0; i < numPeaks[0]; i++)
    {
      mrs_natural Idx;
      if (discNegGroups && (*In[0])(i,peakView::pkGroup) < 0)
        continue;
      for (mrs_natural k = 1; k < kNumMatrices; k++)
        Idx	= FindDuplicate (In[k], (*In[0])(i, peakView::pkFrequency), numPeaks[k]);

      if (Idx < 0)
      {
        WriteOutput (Out, In[0], i, outputIdx);
        outputIdx++;
      }
    }
  }
  else if (ctrl_mode_->to<mrs_string>() == "XOR")
  {
    // find duplicates and write only residual to output
    for (i = 0; i < numPeaks[0]; i++)
    {
      if (discNegGroups && (*In[0])(i,peakView::pkGroup) < 0)
        continue;
      mrs_natural Idx	= FindDuplicate (In[1], (*In[0])(i, peakView::pkFrequency), numPeaks[1]);

      if (Idx < 0)
      {
        WriteOutput (Out, In[0], i, outputIdx);
        outputIdx++;
      }
    }
    // find duplicates and write only residual to output
    for (i = 0; i < numPeaks[1]; i++)
    {
      if (discNegGroups && (*In[1])(i,peakView::pkGroup) < 0)
        continue;
      mrs_natural Idx= FindDuplicate (In[0], (*In[1])(i, peakView::pkFrequency), numPeaks[0]);

      if (Idx < 0)
      {
        WriteOutput (Out, In[1], i, outputIdx);
        outputIdx++;
      }
    }
  }
  else
  {
    MRSERR("PeakViewMerfe::myProcess() : illegal mode string: " << ctrl_mode_->to<mrs_string>());
  }

  for (i = 0; i < kNumMatrices; i++)
  {
    delete In[i];
  }

  ctrl_totalNumPeaks_->setValue(outputIdx);
}

