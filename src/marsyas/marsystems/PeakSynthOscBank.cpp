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

#include "PeakSynthOscBank.h"
#include <marsyas/peakView.h>


using std::ostringstream;
using std::cout;
using std::endl;

using namespace Marsyas;

PeakSynthOscBank::PeakSynthOscBank(mrs_string name):MarSystem("PeakSynthOscBank",name)
{
  psize_ = 0;
  size_ = 0;

  addControls();
}

PeakSynthOscBank::PeakSynthOscBank(const PeakSynthOscBank& a):MarSystem(a)
{
  ctrl_harmonize_ = getctrl("mrs_realvec/harmonize");
}

PeakSynthOscBank::~PeakSynthOscBank()
{
}

MarSystem*
PeakSynthOscBank::clone() const
{
  return new PeakSynthOscBank(*this);
}

void
PeakSynthOscBank::addControls()
{
  addctrl("mrs_natural/Interpolation", MRS_DEFAULT_SLICE_NSAMPLES/4);
  setctrlState("mrs_natural/Interpolation", true);
  addctrl("mrs_real/PitchShift", 1.0);
  setctrlState("mrs_real/PitchShift", true);
  addctrl("mrs_real/SynthesisThreshold", 0.0);
  setctrlState("mrs_real/SynthesisThreshold", true);
  addctrl("mrs_realvec/harmonize", realvec(), ctrl_harmonize_);
  setctrlState("mrs_realvec/harmonize", true);
}

void
PeakSynthOscBank::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)
  setctrl("mrs_natural/onSamples", getctrl("mrs_natural/Interpolation"));
  setctrl("mrs_natural/onObservations", (mrs_natural)1);
  setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));

  inObservations_ = getctrl("mrs_natural/inObservations")->to<mrs_natural>();

  nbH_ = (ctrl_harmonize_->to<mrs_realvec>().getSize()-1)/2;
  // replace this !!
  if (!nbH_)
  {
    MarControlAccessor acc(ctrl_harmonize_, NOUPDATE);
    realvec& harmonize = acc.to<mrs_realvec>();
    harmonize.stretch(3);
    harmonize(1) = 1.0;
    harmonize(2) = 1.0;
  }

  size_ = 2048*nbH_;

  //if (size_ != psize_)
  {
    lastamp_.stretch(size_);
    nextamp_.stretch(size_);
    lastfreq_.stretch(size_);
    nextfreq_.stretch(size_);
    index_.stretch(size_);
    nextindex_.stretch(size_);
    N_ = inObservations_/peakView::nbPkParameters;

    L_ = 8192;
    table_.stretch(L_);

    for (mrs_natural t=0; t < L_; t++)
    {
      table_(t) =  cos(TWOPI * t/L_);
    }
    psize_ = size_;
  }
  // N_ = inObservations_/nbPkParameters;
  P_ = getctrl("mrs_real/PitchShift")->to<mrs_real>();
  I_ = getctrl("mrs_natural/Interpolation")->to<mrs_natural>();
  S_ = getctrl("mrs_real/SynthesisThreshold")->to<mrs_real>();
  R_ = getctrl("mrs_real/osrate")->to<mrs_real>();
}

void
PeakSynthOscBank::myProcess(realvec& in, realvec& out)
{
  mrs_natural t,c;
  out.setval(0.0);

  if (P_ > 1.0)
    NP_ = (mrs_natural)(N_/P_);
  else
    NP_ = N_;

  Iinv_ = (mrs_real)(1.0 / I_);
  Pinc_ = P_ * L_ / R_;

  nextamp_.setval(0);
  nextfreq_.setval(0);
  nextindex_.setval(0);

  // FIXME This line defines a (possibly) unused variable
  // bool flag = false;

  if(nbH_)
  {
    for(mrs_natural j=0 ; j<nbH_ ; j++)
    {
      mrs_real mulF = ctrl_harmonize_->to<mrs_realvec>()(1+j*2);
      mrs_real mulA = ctrl_harmonize_->to<mrs_realvec>()(2+j*2);

      for (t=0; t < NP_; t++)
      {
        mrs_natural index = (mrs_natural) ceil(in(t)/R_*2048*2+0.5);
        if (in(t) == 0.0 || index >= 2048)
          break;
        index+=j*2048;

        /* save current values for next iteration */

        if(nextfreq_(index))
        {
          cout << "PROBLEM"<<endl;
        }
        nextamp_(index) = in(t+NP_)*mulA;
        nextfreq_(index) = in(t)*Pinc_*mulF;
      }
    }
  }

  for (mrs_natural t=0; t < nextamp_.getSize(); t++)
  {
    // cout << endl << index << endl;
    if(lastfreq_(t) && nextfreq_(t))
    {
      f_ = lastfreq_(t);
      finc_ = (nextfreq_(t) - f_)*Iinv_;
    }
    else if(nextfreq_(t))
    {
      f_ = nextfreq_(t);
      finc_=0;
    }
    else
    {
      f_ = lastfreq_(t);
      finc_=0;
    }

    a_ = lastamp_(t);
    ainc_ = (nextamp_(t) - a_)*Iinv_;

    address_ = index_(t);

    /* avoid extra computing */
    if ((a_ != 0.0 || ainc_!=0.0))
    {
      // accumulate I samples from each oscillator
      // into output slice
      for (c=0; c < I_; ++c)
      {
        naddress_ = (mrs_natural)address_ % L_;
        out(0, c) += a_ * table_(naddress_);
        address_ += f_;

        while (address_ >= L_)
          address_ -= L_;
        while (address_ < 0)
          address_ += L_;

        a_ += ainc_;
        f_ += finc_;
      }
      // move down one parenthesis
    }
    nextindex_(t) = address_;
  }

  lastamp_ = nextamp_;
  lastfreq_ = nextfreq_;
  index_ = nextindex_;
}
