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

#include "../common_source.h"
#include "PeakLabeler.h"
#include <marsyas/peakView.h>

using std::ostringstream;
using namespace Marsyas;

//#define MTLB_DBG_LOG

PeakLabeler::PeakLabeler(mrs_string name):MarSystem("PeakLabeler", name)
{
  addControls();
}

PeakLabeler::PeakLabeler(const PeakLabeler& a) : MarSystem(a)
{
  ctrl_peakLabels_ = getctrl("mrs_realvec/peakLabels");
}

PeakLabeler::~PeakLabeler()
{
}

MarSystem*
PeakLabeler::clone() const
{
  return new PeakLabeler(*this);
}

void
PeakLabeler::addControls()
{
  addctrl("mrs_realvec/peakLabels", realvec(), ctrl_peakLabels_);
}

void
PeakLabeler::myUpdate(MarControlPtr sender)
{
  //the out flow parameters are the same as the out flow ones
  MarSystem::myUpdate(sender);
}

void
PeakLabeler::myProcess(realvec& in, realvec& out)
{
  out = in;

  peakView outPeakView(out);

  mrs_natural maxNumPeaks = outPeakView.getFrameMaxNumPeaks();
  const realvec& peakLabels = ctrl_peakLabels_->to<mrs_realvec>(); //reading unlocked and linked control!!! --> should use MarControlAccessor [TODO][!]

  if((mrs_natural)peakLabels.getSize() != outPeakView.getTotalNumPeaks())
  {
    MRSERR("PeakLabeler::myProcess - peakLabels control and input peaks number mismatch! Labeling not performed!");
  }
  else
  {
    //fill in the peak labeling using the info
    //from the peakLabels control (if any)
    labelIdx_ = 0;
    for(mrs_natural f=0; f < outPeakView.getNumFrames(); ++f)
    {
      for(mrs_natural p=0; p < outPeakView.getFrameNumPeaks(f); ++p)
      {
//				outPeakView(p, peakView::pkGroup, f) = peakLabels(labelIdx_); // assignment operator doesn't seem to work
        out(p + maxNumPeaks*peakView::pkGroup, f) = peakLabels(labelIdx_);
        labelIdx_++;
      }
    }
  }

#ifdef MARSYAS_MATLAB
#ifdef MTLB_DBG_LOG
  MATLAB_PUT(out, "out");
  MATLAB_PUT((mrs_natural)peakView::nbPkParameters, "numPeakParams");
  MATLAB_EVAL("numPeaks	= size(out,1)/numPeakParams;");
  MATLAB_EVAL("numFrames	= size(out,2);");
  MATLAB_EVAL("groupIdx	= 6;");
  MATLAB_EVAL("dispRange	= -1 + min([min(min(out(1+numPeaks*groupIdx:numPeaks*(groupIdx+1),1:numFrames))) -max(max(out(1+numPeaks*groupIdx:numPeaks*(groupIdx+1),1:numFrames)))]);");
  MATLAB_EVAL("dispFreqRange = 100:20:3000;");
  MATLAB_EVAL("spectrogram = zeros(length(dispFreqRange), size(out,2))+dispRange;");
  MATLAB_EVAL("for (f=1:numFrames) for (i=1:numPeaks) [v,freqIdx] = min(abs(dispFreqRange - out(i,f))); spectrogram(freqIdx,f) = out(i+numPeaks*groupIdx,f); end; end;");
  MATLAB_EVAL("figure(82),imagesc(1:10,dispFreqRange,spectrogram,[dispRange abs(dispRange)]),colorbar,xlabel('frames'),ylabel('frequency'),title('Clusters')");
#endif
#endif
}
