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
    \class PeSynOscBank
    \brief PeSynOscBank

    A bank of sine wave oscillators used for pevocoding. 

*/

#include "PeSynOscBank.h"
#include "PeUtilities.h"

using namespace std;
using namespace Marsyas;

PeSynOscBank::PeSynOscBank(string name):MarSystem("PeSynOscBank",name)
{
  //type_ = "PeSynOscBank";
  //name_ = name;
  
	psize_ = 0;
  size_ = 0;

	addControls();
}

PeSynOscBank::PeSynOscBank(const PeSynOscBank& a):MarSystem(a)
{
	ctrl_harmonize_ = getctrl("mrs_realvec/harmonize");
}


PeSynOscBank::~PeSynOscBank()
{
}

MarSystem* 
PeSynOscBank::clone() const 
{
  return new PeSynOscBank(*this);
}

void 
PeSynOscBank::addControls()
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
PeSynOscBank::myUpdate(MarControlPtr sender)
{
  setctrl("mrs_natural/onSamples", getctrl("mrs_natural/Interpolation"));
  setctrl("mrs_natural/onObservations", (mrs_natural)1);
  setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));  

  // mrs_natural inObservations = getctrl("mrs_natural/inObservations")->toNatural();

  //defaultUpdate();
	inObservations_ = getctrl("mrs_natural/inObservations")->toNatural();

	nbH_ = (ctrl_harmonize_->toVec().getSize()-1)/2;
	// replace this !!
	if (!nbH_)
		nbH_=1;
  size_ = 2048*nbH_;
  
  if (size_ != psize_) 
  {
    lastamp_.stretch(size_);
		nextamp_.stretch(size_);
    lastfreq_.stretch(size_);
    nextfreq_.stretch(size_);
    index_.stretch(size_);
    N_ = inObservations_/nbPkParameters;

    L_ = 8192;
    table_.stretch(L_);
    
    for (t=0; t < L_; t++)
		{
			table_(t) = N_ * cos(TWOPI * t/L_);
		}
		  psize_ = size_;
  }
  

  N_ = inObservations_/nbPkParameters;
  P_ = getctrl("mrs_real/PitchShift")->toReal();
  I_ = getctrl("mrs_natural/Interpolation")->toNatural();
  S_ = getctrl("mrs_real/SynthesisThreshold")->toReal();
  R_ = getctrl("mrs_real/osrate")->toReal();
}
	
void 
PeSynOscBank::myProcess(realvec& in, realvec& out)
{
  
  //checkFlow(in,out);
  //cout << in;
  out.setval(0.0);

  
  if (P_ > 1.0)
    NP_ = (mrs_natural)(N_/P_);
  else
    NP_ = N_;

  Iinv_ = (mrs_real)(1.0 / I_);
  Pinc_ = P_ * L_ / R_;
  
	nextamp_.setval(0);
  nextfreq_.setval(0);

  bool flag = false;
  
	/*	if(nbH_)
		for(mrs_natural j=0 ; j<nbH_ ; j++)
	*/	{
			mrs_real mulF = 1;//ctrl_harmonize_->toVec()(1+j*2); 
			mrs_real mulA = 0.1;//ctrl_harmonize_->toVec()(2+j*2);

  for (t=0; t < NP_; t++)
    {
			mrs_natural index = (mrs_natural) ceil(in(t)/44100*2048);
			if(lastfreq_(index))
			{
  		f_ = lastfreq_(index);
      finc_ = (in(t)*Pinc_*mulF - f_)*Iinv_;
			}
			else
			{
  f_ = in(t)*Pinc_*mulF;
	finc_=0;
			}
      a_ = lastamp_(index);
      ainc_ = (in(t+NP_)*mulA - a_)*Iinv_;
      
      address_ = index_(index);		
		
      /* avoid extra computing */ 
      if ((a_ != 0.0 || ainc_!=0.0))
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

      index_(index) = address_;	  
	  
      
      /* save current values for next iteration */ 
      
      nextamp_(index) = in(t+NP_)*mulA;
      nextfreq_(index) = in(t)*Pinc_*mulF;
    }
	
		}
	lastamp_ = nextamp_;
	lastfreq_ = nextfreq_;
	cout << out;
}
