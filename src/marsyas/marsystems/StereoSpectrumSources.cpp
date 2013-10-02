/*
** Copyright (C) 1998-2007 George Tzanetakis <gtzan@cs.uvic.ca>
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
#include "StereoSpectrumSources.h"
#include "Peaker.h"

using namespace std;
using namespace Marsyas;

StereoSpectrumSources::StereoSpectrumSources(mrs_string name):MarSystem("StereoSpectrumSources", name)
{
  panPeaker_ = new Peaker("panPeaker");
}

StereoSpectrumSources::StereoSpectrumSources(const StereoSpectrumSources& a):MarSystem(a)
{
  panPeaker_ = new Peaker("panPeaker");
}

StereoSpectrumSources::~StereoSpectrumSources()
{
  delete panPeaker_;
}

MarSystem*
StereoSpectrumSources::clone() const
{
  return new StereoSpectrumSources(*this);
}

void
StereoSpectrumSources::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)
  MRSDIAG("StereoSpectrumSources.cpp - StereoSpectrumSources:myUpdate");

  ctrl_onSamples_->setValue(ctrl_inSamples_, NOUPDATE);
  ctrl_onObservations_->setValue(1, NOUPDATE);
  ctrl_osrate_->setValue(ctrl_israte_, NOUPDATE);
  ctrl_onObsNames_->setValue("StereoSpectrumSources,", NOUPDATE);

  panPeaker_->updControl("mrs_natural/inSamples", inObservations_-1);
  panPeaker_->updControl("mrs_natural/inObservations", 1);
  panPeaker_->updControl("mrs_natural/peakStart", 0);
  panPeaker_->updControl("mrs_natural/peakEnd", inObservations_-2);
  panPeaker_->updControl("mrs_real/peakStrength", 1.0);
}

void
StereoSpectrumSources::myProcess(realvec& in, realvec& out)
{
  mrs_natural t,o;
  for (t = 0; t < inSamples_; t++)
  {
    //start by sorting in non-descending order the panning values
    //of all spectrum bins of the current frame
    in.getCol(t, orderedPans_);
    orderedPans_.sort();

    //MATLAB_PUT(orderedPans_, "orderedPans");
    //MATLAB_EVAL("plot(orderedPans)");

    //calculate derivative, i.e changes of panning
    panChanges_.create(inObservations_-1);
    for(o=0; o<inObservations_-1; ++o)
      panChanges_(o) = orderedPans_(o+1)-orderedPans_(o);

    //MATLAB_PUT(panChanges_, "panChanges");
    //MATLAB_EVAL("plot(panChanges)");

    //look for peaks in pan changes, i.e. a good estimate of
    //the number of stereo sources in the signal
    panPeaks_.create(inObservations_-1);
    panPeaker_->process(panChanges_, panPeaks_);

    //MATLAB_PUT(panPeaks_, "panPeaks");
    //MATLAB_EVAL("plot(panPeaks)");

    out(0, t) = 0.0;
    for(o=0; o < inObservations_-1; ++o)
      out(0,t) += (panPeaks_(o) != 0.0);//peaks are the non-zero values in panPeaks_

    //cout << out(0,t) << endl;

    // other option for calculating this (pseudo-code):
    // if abs(running-average - current_value) > 0.3 * running_average
  }
}
