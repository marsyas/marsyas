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


PvOscBank::PvOscBank(const PvOscBank& a):MarSystem(a) 
{
	ctrl_analysisphases_ = getctrl("mrs_realvec/analysisphases");
	ctrl_phases_ = getctrl("mrs_realvec/phases");
	ctrl_phaselock_ = getctrl("mrs_bool/phaselock");
	ctrl_onsetsAudible_ = getctrl("mrs_bool/onsetsAudible");
	ctrl_rmsIn_ = getctrl("mrs_real/rmsIn");
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
	addctrl("mrs_realvec/phases", realvec(), ctrl_phases_);
	addctrl("mrs_bool/phaselock", false, ctrl_phaselock_);
	addctrl("mrs_bool/onsetsAudible", true, ctrl_onsetsAudible_);
	addctrl("mrs_real/rmsIn", 0.0, ctrl_rmsIn_);
}

void
PvOscBank::myUpdate(MarControlPtr sender)
{
	
	(void) sender;
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
			MarControlAccessor acc(ctrl_phases_);
			mrs_realvec& phases = acc.to<mrs_realvec>();
			phases.create(size_);
		}

		


		lastamp_.stretch(size_);
		magnitudes_.stretch(size_);
		regions_.stretch(size_);
		
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
	int si;
	
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
		for (int i = bin-h; i < bin+h; i++)
		{
			if (magnitudes(bin) < magnitudes(i))
				res = false;
		}
	
	if (magnitudes(bin) < 0.005 * maxAmp) 
		res = false;
	return res;
}

	
void 
PvOscBank::myProcess(realvec& in, realvec& out)
{
	MarControlAccessor acc(ctrl_phases_);
	mrs_realvec& phases = acc.to<mrs_realvec>();
	
	MarControlAccessor  acc1(ctrl_analysisphases_);
	mrs_realvec& analysisphases = acc1.to<mrs_realvec>();
	
	
	if (ctrl_phaselock_->to<bool>() == true)
	{
		
		ctrl_phaselock_->setValue(false);
		for (t=0; t < NP_; t++)
		{
			phases(t) = in(2*t+1,0);
		}
		if (ctrl_onsetsAudible_->to<mrs_bool>() == true) 
		{
			phases(t) = in(2*t+1,0);
		}
	}
	else
	{
		for (t=0; t < NP_; t++)
		{
			phases(t) =  in(2*t+1,0);
		}
		PS_ = P_;		
	}
	

	

	temp_.setval(0.0);

	
	if (PS_ > 1.0)
		NP_ = (mrs_natural)(N_/PS_);
	else
		NP_ = N_;
	
	Iinv_ = (mrs_real)(1.0 / I_);
	Pinc_ = PS_ * L_ / TWOPI;
	
	
	Nw_ = getctrl("mrs_natural/winSize")->to<mrs_natural>();

	mrs_real omega_k;

	mrs_real maxAmp =0.0;
	
	
	for (t=0; t < NP_; t++)
	{
		magnitudes_(t) =  1.5 * in(2*t,0);
		if (t==0) 
			magnitudes_(t) = 0.0;
		if (t==size_)
			magnitudes_(t) = 0.0;
	}

	


	// calculate regions of influence 
	for (t=0; t < NP_; t++)
	{
		int previous_peak=0;
		int peak = 0;
		int peakCount = 0;
		

		if (isPeak(t, magnitudes_, maxAmp))
		{
			// calculate significant peaks and corresponding 
			// non-overlapping intervals 
			peak = t;
			
			if (peak-previous_peak == 1)
				regions_(peak) = peak;
			else 
			{
				for (int j=previous_peak; j< previous_peak + (int)((peak-previous_peak)/2.0); j++) 
				{
					regions_(j) = previous_peak;
				}
				
				for (int j= previous_peak + (int)((peak-previous_peak)/2.0); j < peak; j++) 
				{
					regions_(j) = peak;					
				}
			}
			previous_peak = peak;
		}
	}
		
	/* static int count = 0;
	if (count == 0) 
	{
		for (t=0; t < NP_; t++)
		{

			lastfreq_(t) = phases(t);
		}
		
	}
	count++;
	*/ 
	
	
	
	for (t=0; t < NP_; t++)
	{
		phases(t) *= Pinc_;
		
		
		while (analysisphases(t) > PI) 
			analysisphases(t) -= TWOPI;
		while (analysisphases(t) < -PI) 
			analysisphases(t) += TWOPI;      		


		f_ = lastfreq_(t);			
		// finc_ = analysisphases(t) * Iinv_;
		finc_ = (phases(t) - f_) * Iinv_;
		
		
		a_ = lastamp_(t);
		ainc_ = (magnitudes_(t) - a_)*Iinv_;
		address_ = index_(t);
		



		if (ainc_ != 0.0 || a_ != 0.0)
		{
			// accumulate I samples from each oscillator 
			// into output slice 
			for (c=0; c < I_; c++)
			{
				naddress_ = (mrs_natural)address_;
				temp_(c) += a_ * table_(naddress_);
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
		lastfreq_(t) = phases(t);

	}

	mrs_real rmsOut = 0.0;
	for (t=0; t < Nw_; t++) 
	{
		out(0,t) = temp_(t);
		rmsOut += (out(0,t) * out(0,t));
	}
	
 


	/* 

	rmsOut /= Nw_;
	rmsOut = sqrt(rmsOut);

	mrs_real rmsIn = ctrl_rmsIn_->to<mrs_real>();
	out *= 1.25;
	out *= (rmsIn / rmsOut); 
	*/ 

	
	for  (t=0; t < Nw_-I_; t++)
		temp_(t) = temp_(t+I_);
	for (t=Nw_-I_; t<Nw_; t++) 
		temp_(t) = 0.0;
	 
	
}
