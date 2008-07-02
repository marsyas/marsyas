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

#include "PvUnconvert.h"

#include <algorithm> 

using namespace std;
using namespace Marsyas;

PvUnconvert::PvUnconvert(string name):MarSystem("PvUnconvert",name)
{
  //type_ = "PvUnconvert";
  //name_ = name;

	addControls();
	transient_counter_ = 0;
}


PvUnconvert::PvUnconvert(const PvUnconvert& a):MarSystem(a)
{
	ctrl_mode_ = getctrl("mrs_string/mode");
	ctrl_lastphases_ = getctrl("mrs_realvec/lastphases");
	ctrl_analysisphases_ = getctrl("mrs_realvec/analysisphases");
	ctrl_phaselock_ = getctrl("mrs_bool/phaselock");
	transient_counter_ = 0;
	
}


PvUnconvert::~PvUnconvert()
{
  
}


MarSystem* 
PvUnconvert::clone() const 
{
  return new PvUnconvert(*this);
}  


void 
PvUnconvert::addControls()
{
  addctrl("mrs_natural/Interpolation", MRS_DEFAULT_SLICE_NSAMPLES/4);
  addctrl("mrs_string/mode", "loose_phaselock", ctrl_mode_);
  addctrl("mrs_realvec/lastphases", realvec(), ctrl_lastphases_);
  addctrl("mrs_realvec/analysisphases", realvec(), ctrl_analysisphases_);
  addctrl("mrs_bool/phaselock", true, ctrl_phaselock_);
}

void
PvUnconvert::myUpdate(MarControlPtr sender)
{
	(void) sender;
	setctrl("mrs_natural/onSamples", getctrl("mrs_natural/inSamples"));
	setctrl("mrs_natural/onObservations", getctrl("mrs_natural/inObservations")->to<mrs_natural>() - 2);
	setctrl("mrs_real/osrate", getctrl("mrs_real/israte")->to<mrs_real>() / getctrl("mrs_natural/onObservations")->to<mrs_natural>());  
	
	mrs_natural inObservations = getctrl("mrs_natural/inObservations")->to<mrs_natural>();
	mrs_natural onObservations = getctrl("mrs_natural/onObservations")->to<mrs_natural>();
	mrs_real israte = getctrl("mrs_real/israte")->to<mrs_real>();
	
	N2_ = onObservations/2;

	{
		MarControlAccessor acc(ctrl_lastphases_);
		mrs_realvec& lastphases = acc.to<mrs_realvec>();
		lastphases.create(N2_+1);
	}

	{
		MarControlAccessor acc(ctrl_analysisphases_);
		mrs_realvec& analysisphases = acc.to<mrs_realvec>();
		analysisphases.create(N2_+1);
	}



	mag_.create(N2_+1);	
	phase_.create(N2_+1);
	lphase_.create(N2_+1);
	iphase_.create(N2_+1);
	
	lmag_.create(N2_+1);
	regions_.create(N2_+1);
	
	
	fundamental_ = (mrs_real) (israte  / onObservations);
	factor_ = (((getctrl("mrs_natural/Interpolation")->to<mrs_natural>()* TWOPI)/(israte)));

	
}

void 
PvUnconvert::myProcess(realvec& in, realvec& out)
{
	

	MarControlAccessor acc(ctrl_lastphases_);
	mrs_realvec& lastphases = acc.to<mrs_realvec>();
	MarControlAccessor  acc1(ctrl_analysisphases_);
	mrs_realvec& analysisphases = acc1.to<mrs_realvec>();
	
	

	if (ctrl_phaselock_->to<mrs_bool>())
	{
		lastphases = analysisphases;
		ctrl_phaselock_->setValue(false);
	}
	

	mrs_natural re, amp, im, freq;
	mrs_real avg_re;
	mrs_real avg_im;
	
	const mrs_string& mode = ctrl_mode_->to<mrs_string>();	    
	
	for (t=0; t <= N2_; t++)
	{
		re = amp = 2*t; 
		im = freq = 2*t+1;
		if (t== N2_)
		{
			re = 1;
		}
		mag_(t) = in(re,0);
		if (t==N2_)
			mag_(t) = 0.0;
	}


	if (mode == "identity_phaselock")
	{
		int previous_peak=0;
		int peak = 0;
		
		// calculate significant peaks and corresponding non-overlapping 
		// intervals 
		if ((t > 2) && (t <= N2_-2))
		{
			if ((mag_(t) > mag_(t-1)) &&
				(mag_(t) > mag_(t-2)) &&
				(mag_(t) > mag_(t+1)) && 
				(mag_(t) > mag_(t+2)))
			{
				peak = t;
				for (int j=previous_peak; j< previous_peak + (int)((peak-previous_peak)/2.0); j++) 
				{
					regions_(j) = previous_peak;
				}
				
				for (int j= previous_peak + (int)((peak-previous_peak)/2.0); j < peak; j++) 
				{
					regions_(j) = peak;					
				}
				previous_peak = peak;
			}
		}
	}
	
	// cout << regions_ << endl;


// 	for (t=0; t <= N2_; t++)
// 		{
// 			re = amp = 2*t; 
// 			im = freq = 2*t+1;
// 			if ((t >= 1) || (t < N2_))
// 			{
// 				avg_re = mag_(t) * cos(phase_(t)) +
// 					0.25 * mag_(t-1) * cos(phase_(t-1)) +
// 					0.25 * mag_(t+1) * cos(phase_(t));
				
				
// 				avg_im = -mag_(t) * sin(phase_(t)) -
// 					-0.25 * mag_(t-1) * sin(phase_(t-1)) -
// 					-0.25 * mag_(t+1) * sin(phase_(t));
// 				lphase_(t) = -atan2(avg_im,avg_re);
// 			}
// 			lmag_(t) = mag_(t);
// 			out(re,0) = lmag_(t) * cos(lphase_(t));
// 			if (t != N2_)
// 				out(im,0) = -lmag_(t) * sin(lphase_(t));
// 		}

	
			
	
	for (t=0; t <= N2_; t++)
	{
		re = amp = 2*t; 
		im = freq = 2*t+1;
		
		if (mode == "identity_phaselock")
		{
			if ((t > 2) && (t <= N2_-2))
			{
				if ((mag_(t) > mag_(t-1)) &&
					(mag_(t) > mag_(t-2)) &&
					(mag_(t) > mag_(t+1)) && 
					(mag_(t) > mag_(t+2)))
				{
					lastphases(t) += (in(freq,0) - t * fundamental_);
					iphase_(t) = lastphases(t) * factor_;
				}
				else
				{
					iphase_(t) = phase_(regions_(t)) + analysisphases(t) - analysisphases(regions_(t));
					lastphases(t) += (iphase_(t));
					phase_(t) = lastphases(t) * factor_;
				}
			}
			else 
			{
				mag_(t) = 0.0;
				
				lastphases(t) += (in(freq,0) - t * fundamental_);
				iphase_(t) = lastphases(t) * factor_;
			}
			
			if (t == N2_) 
				re = 1;
			
			out(re,0) = mag_(t) * cos(iphase_(t));
			if (t != N2_)
				out(im,0) = -mag_(t) * sin(iphase_(t));
			
		}
		else if (mode == "loose_phaselock") 
		{
		}
		else // classic
		{
			cout << "classic " << endl;
			
			lastphases(t) += (in(freq,0) - t * fundamental_);
			phase_(t) = lastphases(t) * factor_;
			
			if (t == N2_) 
			{
				re = 1;
			}
			
			out(re,0) = mag_(t) * cos(phase_(t));
			if (t != N2_)
				out(im,0) = -mag_(t) * sin(phase_(t));
		}
	}
		
		
		

	
}


 





	

	

	
	

	
