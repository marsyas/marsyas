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
    \class PvOscBank
    \brief PvOscBank

    A bank of sine wave oscillators used for phasevocoding. 

*/

#include "PvOscBank.h"

using namespace std;
using namespace Marsyas;

PvOscBank::PvOscBank(string name):MarSystem("PvOscBank",name)
{
  //type_ = "PvOscBank";
  //name_ = name;
  
	psize_ = 0;
  size_ = 0;

	addControls();
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
}

void
PvOscBank::localUpdate()
{
  setctrl("mrs_natural/onSamples", getctrl("mrs_natural/Interpolation"));
  setctrl("mrs_natural/onObservations", (mrs_natural)1);
  setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));  

  // mrs_natural inObservations = getctrl("mrs_natural/inObservations").toNatural();

  //defaultUpdate();
	inObservations_ = getctrl("mrs_natural/inObservations").toNatural();

  size_ = inObservations_/2 + 1;
  
  if (size_ != psize_) 
  {
    lastamp_.stretch(size_);
    lastfreq_.stretch(size_);
    index_.stretch(size_);
    N_ = inObservations_/2;

    L_ = 8192;
    table_.stretch(L_);
    
    for (t=0; t < L_; t++)
		{
			table_(t) = N_ * cos(TWOPI * t/L_);
		}
  }
  
  psize_ = size_;
  
  P_ = getctrl("mrs_real/PitchShift").toReal();
  I_ = getctrl("mrs_natural/Interpolation").toNatural();
  S_ = getctrl("mrs_real/SynthesisThreshold").toReal();
  R_ = getctrl("mrs_real/osrate").toReal();
}
	
void 
PvOscBank::process(realvec& in, realvec& out)
{
  
  checkFlow(in,out);
  
  out.setval(0.0);

  
  if (P_ > 1.0)
    NP_ = (mrs_natural)(N_/P_);
  else
    NP_ = N_;

  Iinv_ = (mrs_real)(1.0 / I_);
  Pinc_ = P_ * L_ / R_;
  

  bool flag = false;
  
  for (t=0; t < NP_; t++)
    {
      in(2*t+1,0) *= Pinc_;
      f_ = lastfreq_(t);
      finc_ = (in(2*t+1,0) - f_)*Iinv_;

      
      a_ = lastamp_(t);
      ainc_ = (in(2*t,0) - a_)*Iinv_;
      
      address_ = index_(t);
      
      /* avoid extra computing */ 
      if (ainc_ != 0.0 || a_ != 0.0)
	{
	  // accumulate I samples from each oscillator 
	  // into output slice 
	  for (c=0; c < I_; c++)
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
	  
	}

      index_(t) = address_;	  
	  
      
      /* save current values for next iteration */ 
      
      lastamp_(t) = in(2*t,0);
      lastfreq_(t) = in(2*t+1, 0);
    }
}
