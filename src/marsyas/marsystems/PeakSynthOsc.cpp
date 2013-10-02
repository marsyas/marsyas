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

#include "PeakSynthOsc.h"
#include <marsyas/peakView.h>

using std::ostringstream;
using namespace Marsyas;

PeakSynthOsc::PeakSynthOsc(mrs_string name):MarSystem("PeakSynthOsc",name)
{
  addControls();
}

PeakSynthOsc::PeakSynthOsc(const PeakSynthOsc& a):MarSystem(a)
{
  ctrl_samplingFreq_ = getctrl("mrs_real/samplingFreq");
  ctrl_peakGroup2Synth_ = getctrl("mrs_natural/peakGroup2Synth");
  ctrl_isSilence_ = getctrl("mrs_bool/isSilence");
  ctrl_synSize_ = getctrl("mrs_natural/synSize");
  ctrl_delay_ = getctrl("mrs_natural/delay");
  ctrl_harmonize_ = getctrl("mrs_realvec/harmonize");
}

PeakSynthOsc::~PeakSynthOsc()
{
}

MarSystem*
PeakSynthOsc::clone() const
{
  return new PeakSynthOsc(*this);
}

void
PeakSynthOsc::addControls()
{
  addctrl("mrs_real/samplingFreq", MRS_DEFAULT_SLICE_SRATE, ctrl_samplingFreq_);
  ctrl_samplingFreq_->setState(true);

  addctrl("mrs_natural/peakGroup2Synth", 0, ctrl_peakGroup2Synth_);
  addctrl("mrs_bool/isSilence", true, ctrl_isSilence_);

  addctrl("mrs_natural/synSize", MRS_DEFAULT_SLICE_NSAMPLES, ctrl_synSize_);
  ctrl_synSize_->setState(true);

  addctrl("mrs_natural/delay", 0, ctrl_delay_);
  ctrl_delay_->setState(true);

  addctrl("mrs_realvec/harmonize", realvec(), ctrl_harmonize_);
}

void
PeakSynthOsc::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)
  ctrl_onSamples_->setValue(ctrl_synSize_, NOUPDATE);
  ctrl_onObservations_->setValue(1, NOUPDATE);
  ctrl_osrate_->setValue(ctrl_samplingFreq_, NOUPDATE);
  ctrl_onObsNames_->setValue("audio,", NOUPDATE);

  delay_ = ctrl_delay_->to<mrs_natural>();
  factor_ = TWOPI/ctrl_samplingFreq_->to<mrs_real>();
}

void
PeakSynthOsc::sine(realvec& out, mrs_real f, mrs_real a, mrs_real p)
{
  if(f > 0.0 && a > 0.0)
  {
    for (mrs_natural i=0 ; i < onSamples_ ; ++i)
      out(i) += a*cos(factor_*f*(i-delay_)+p); // consider -fftSize/2 for synth in phase
  }
}

void
PeakSynthOsc::myProcess(realvec& in, realvec& out)
{
  out.setval(0);
  silence_ = true;

  pkGrp2Synth_ = ctrl_peakGroup2Synth_->to<mrs_natural>();
  Nb_ = in.getSize()/peakView::nbPkParameters ; //number of peaks in the current frame
  nbH_ = ctrl_harmonize_->to<mrs_realvec>().getSize();

  if(nbH_)
    for(mrs_natural j=0 ; j<(nbH_-1)/2 ; j++)
    {
      mulF_ = ctrl_harmonize_->to<mrs_realvec>()(1+j*2);
      mulA_ = ctrl_harmonize_->to<mrs_realvec>()(2+j*2);
      //cout << "mulF_" << mulF_ << "mulA_" << mulA_ << endl;
      for (mrs_natural i=0; i < Nb_; ++i)
      {
        // either synthesize peaks with a corresponding GroupID or all with a group ID >= 0
        mrs_bool synthMe = (pkGrp2Synth_ < 0)? (in(i+peakView::pkGroup*Nb_) >= 0) : (in(i+peakView::pkGroup*Nb_) == pkGrp2Synth_);
        if( synthMe )
        {
          sine(out, in(i)*mulF_, in(i+Nb_)*mulA_, in(i+2*Nb_));
          silence_ = false;
        }
      }
    }
  else
    for (mrs_natural i=0; i < Nb_; ++i)
    {
      // either synthesize peaks with a corresponding GroupID or all with a group ID >= 0
      mrs_bool synthMe = (pkGrp2Synth_ < 0)? (in(i+peakView::pkGroup*Nb_) >= 0) : (in(i+peakView::pkGroup*Nb_) == pkGrp2Synth_);
      if( synthMe )
      {
        sine(out, in(i), in(i+Nb_), in(i+2*Nb_));
        silence_ = false;
      }
    }

  //signal if at least one peak was synthesized or not
  ctrl_isSilence_->setValue(silence_);
}
