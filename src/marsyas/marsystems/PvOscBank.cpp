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

#include "PvOscBank.h"

using std::ostringstream;
using namespace Marsyas;

PvOscBank::PvOscBank(mrs_string name):MarSystem("PvOscBank",name)
{
  //type_ = "PvOscBank";
  //name_ = name;

  psize_ = 0;
  size_ = 0;
  PS_ = 0;

  addControls();
}


PvOscBank::PvOscBank(const PvOscBank& a):MarSystem(a)
{
  ctrl_analysisphases_ = getctrl("mrs_realvec/analysisphases");
  ctrl_frequencies_ = getctrl("mrs_realvec/frequencies");
  ctrl_regions_ = getctrl("mrs_realvec/regions");
  ctrl_peaks_ = getctrl("mrs_realvec/peaks");

  ctrl_phaselock_ = getctrl("mrs_bool/phaselock");
  ctrl_onsetsAudible_ = getctrl("mrs_bool/onsetsAudible");
  ctrl_rmsIn_ = getctrl("mrs_real/rmsIn");

  psize_ = 0;
  PS_ = 0;
}


PvOscBank::~PvOscBank()
{
}

MarSystem*
PvOscBank::clone() const
{
  return new PvOscBank(*this);
}

void
PvOscBank::addControls()
{
  addctrl("mrs_natural/Interpolation", MRS_DEFAULT_SLICE_NSAMPLES/4);
  setctrlState("mrs_natural/Interpolation", true);
  addctrl("mrs_real/PitchShift", 1.0);
  setctrlState("mrs_real/PitchShift", true);
  addctrl("mrs_real/SynthesisThreshold", 0.0);
  setctrlState("mrs_real/SynthesisThreshold", true);
  addctrl("mrs_natural/winSize", MRS_DEFAULT_SLICE_NSAMPLES);
  setctrlState("mrs_natural/winSize", true);


  addctrl("mrs_realvec/analysisphases", realvec(), ctrl_analysisphases_);
  addctrl("mrs_realvec/frequencies", realvec(), ctrl_frequencies_);
  addctrl("mrs_realvec/regions", realvec(), ctrl_regions_);
  addctrl("mrs_realvec/peaks", realvec(), ctrl_peaks_);
  addctrl("mrs_bool/phaselock", false, ctrl_phaselock_);
  addctrl("mrs_bool/onsetsAudible", true, ctrl_onsetsAudible_);
  addctrl("mrs_real/rmsIn", 0.0, ctrl_rmsIn_);
}

void
PvOscBank::myUpdate(MarControlPtr sender)
{
  mrs_natural t;
  (void) sender;  //suppress warning of unused parameter(s)
  setctrl("mrs_natural/onSamples", getctrl("mrs_natural/winSize"));
  setctrl("mrs_natural/onObservations", (mrs_natural)1);
  setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));

  inObservations_ = getctrl("mrs_natural/inObservations")->to<mrs_natural>();
  inSamples_ = getctrl("mrs_natural/inSamples")->to<mrs_natural>();

  size_ = inObservations_/2;
  temp_.create(getctrl("mrs_natural/winSize")->to<mrs_natural>());



  if (size_ != psize_)
  {
    {
      MarControlAccessor acc(ctrl_analysisphases_);
      mrs_realvec& analysisphases = acc.to<mrs_realvec>();
      analysisphases.create(size_);
    }

    {
      MarControlAccessor acc(ctrl_frequencies_);
      mrs_realvec& frequencies = acc.to<mrs_realvec>();
      frequencies.create(size_);
    }

    {
      MarControlAccessor acc(ctrl_regions_);
      mrs_realvec& regions = acc.to<mrs_realvec>();
      regions.create(size_);
    }

    {
      MarControlAccessor acc(ctrl_peaks_);
      mrs_realvec& peaks = acc.to<mrs_realvec>();
      peaks.create(size_);
    }




    lastamp_.stretch(size_);
    magnitudes_.stretch(size_);



    lastfreq_.stretch(size_);
    index_.stretch(size_);
    N_ = size_;
    L_ = 8192;
    table_.stretch(L_+2); 			// leave some head-room for numerical errors

    for (t=0; t < L_; t++)
    {
      table_(t) = N_ * cos(TWOPI * t/L_);
    }
  }

  psize_ = size_;


  P_ = getctrl("mrs_real/PitchShift")->to<mrs_real>();
  I_ = getctrl("mrs_natural/Interpolation")->to<mrs_natural>();
  S_ = getctrl("mrs_real/SynthesisThreshold")->to<mrs_real>();
  R_ = getctrl("mrs_real/osrate")->to<mrs_real>();
}



int
PvOscBank::subband(int bin)
{
  int si = 0;

  if (bin  < 16)
    si = 0;
  else if ((bin >= 16) && (bin < 32))
    si = 1;
  else if (bin < 512)
    si = (int)(log(bin*1.0) / log(2.0))-3;
  else if (bin > 512)
    si = 6;
  return si;
}


bool
PvOscBank::isPeak(int bin, mrs_realvec& magnitudes, mrs_real maxAmp)
{
  bool res = true;

  int h = subband(bin);
  h = 2;
  if ((bin > 2) && (bin <= size_-2))
    for (int i = bin-h; i < bin+h; ++i)
    {
      if (magnitudes(bin) < magnitudes(i))
        res = false;
    }
  if (magnitudes(bin) < 0.005 * maxAmp)
    res = false;
  if (bin == 0)
    res = true;
  return res;
}


void
PvOscBank::myProcess(realvec& in, realvec& out)
{


  mrs_natural c,t;
  MarControlAccessor acc(ctrl_frequencies_);
  mrs_realvec& frequencies = acc.to<mrs_realvec>();

  MarControlAccessor  acc1(ctrl_analysisphases_);
  mrs_realvec& analysisphases = acc1.to<mrs_realvec>();


  MarControlAccessor  acc2(ctrl_regions_);
  mrs_realvec& regions = acc2.to<mrs_realvec>();


  MarControlAccessor  acc3(ctrl_peaks_);
  mrs_realvec& peaks = acc3.to<mrs_realvec>();

  if (PS_ > 1.0)
    NP_ = (mrs_natural)(N_/PS_);
  else
    NP_ = N_;



  peaks.setval(0.0);


  for (t=0; t < NP_; t++)
  {
    frequencies(t) =  in(2*t+1,0);
  }
  PS_ = P_;
  Iinv_ = (mrs_real)(1.0 / I_);
  Pinc_ = PS_ * L_ / TWOPI;
  Nw_ = getctrl("mrs_natural/winSize")->to<mrs_natural>();
  mrs_real maxAmp =0.0;

  for (t=0; t < NP_; t++)
  {
    magnitudes_(t) =  in(2*t,0);
    if (magnitudes_(t) > maxAmp)
      maxAmp = magnitudes_(t);
    if (t==0)
      magnitudes_(t) = 0.0;
    if (t==size_)
      magnitudes_(t) = 0.0;


    while (analysisphases(t) > PI)
      analysisphases(t) -= TWOPI;
    while (analysisphases(t) < -PI)
      analysisphases(t) += TWOPI;

  }

  for (int i=0; i < size_; ++i)
  {
    regions(i) = i;
  }



  // calculate regions of influence
  int previous_peak=0;
  int peak = 0;
  for (t=0; t < NP_; t++)
  {
    if (isPeak(t, magnitudes_, maxAmp))
    {

      // calculate significant peaks and corresponding
      // non-overlapping intervals
      peak = t;

      if (peak-previous_peak == 1)
        regions(peak) = peak;
      else
      {
        for (int j=previous_peak; j< previous_peak + (int)((peak-previous_peak)/2.0); j++)
        {
          regions(j) = previous_peak;
        }

        for (int j= previous_peak + (int)((peak-previous_peak)/2.0); j < peak; j++)
        {
          regions(j) = peak;
        }
      }
      previous_peak = peak;
    }

  }



  mrs_real factor = 1;
  for (t=0; t < NP_; t++)
  {

    while (analysisphases(t) > PI)
      analysisphases(t) -= TWOPI;
    while (analysisphases(t) < -PI)
      analysisphases(t) += TWOPI;

    // if (ctrl_phaselock_->to<bool>() == true)
    // {
    // if (t == 0)
    // cout << "PHASELOCKED" << endl;

    // index_(t) = analysisphases(t);
    // factor = 2.25;
    // factor = 1.5;
    // }
  }

  factor = 1.5;

  for (t=0; t < NP_; t++)
  {
    frequencies(t) *= Pinc_;

    f_ = lastfreq_(t);
    finc_ = (frequencies(t) - f_) * Iinv_;

    //if ((magnitudes_(t) < 1.0e-06)||(magnitudes_(t) < 0.01 * maxAmp))
    if (magnitudes_(t) < 1.0e-07)
    {
      magnitudes_(t) = 0.0;
      a_ = lastamp_(t);
      ainc_ = (magnitudes_(t) - a_)*Iinv_;
    }
    else
    {
      a_ = lastamp_(t);
      ainc_ = (magnitudes_(t) - a_)*Iinv_;
    }

    if (t == regions(t))
    {
      address_ = index_((mrs_natural)regions(t));
    }
    else
    {
      address_ = index_(t);
    }


    while (address_ >= L_)
      address_ -= L_;
    while (address_ < 0)
      address_ += L_;



    if (ainc_ != 0.0 || a_ != 0.0)
    {
      peaks(t) = magnitudes_(t);

      // accumulate I samples from each oscillator
      // into output slice
      for (c=0; c < I_; ++c)
      {
        naddress_ = (mrs_natural)address_;
        temp_(c) += a_  * factor * table_(naddress_);
        address_ += f_;

        while (address_ >= L_)
          address_ -= L_;
        while (address_ < 0)
          address_ += L_;

        a_ += ainc_;
        f_ += finc_;
      }

    }

    index_(t) = address_;
    lastamp_(t) = magnitudes_(t);
    lastfreq_(t) = frequencies(t);

  }

  ctrl_phaselock_->setValue(false);



  for (t=0; t < Nw_; t++)
    out(0,t) = temp_(t);
  for  (t=0; t < Nw_-I_; t++)
    temp_(t) = temp_(t+I_);
  for (t=Nw_-I_; t<Nw_; t++)
    temp_(t) = 0.0;




}
