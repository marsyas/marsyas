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
	ctrl_regions_ = getctrl("mrs_realvec/regions");
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
  addctrl("mrs_natural/Decimation", MRS_DEFAULT_SLICE_NSAMPLES/4);
  addctrl("mrs_string/mode", "loose_phaselock", ctrl_mode_);
  addctrl("mrs_realvec/lastphases", realvec(), ctrl_lastphases_);
  addctrl("mrs_realvec/analysisphases", realvec(), ctrl_analysisphases_);
  addctrl("mrs_realvec/regions", realvec(), ctrl_regions_);
  addctrl("mrs_bool/phaselock", false, ctrl_phaselock_);
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


	{
		MarControlAccessor acc(ctrl_regions_);
		mrs_realvec& regions = acc.to<mrs_realvec>();
		regions.create(N2_+1);
	}



	mag_.create(N2_+1);	
	phase_.create(N2_+1);
	lphase_.create(N2_+1);
	iphase_.create(N2_+1);
	
	lmag_.create(N2_+1);
	
	
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

	MarControlAccessor  acc2(ctrl_regions_);
	mrs_realvec& regions = acc2.to<mrs_realvec>();
	
	
	
	static int count = 0;
	count++;
	


	mrs_natural re, amp, im, freq;
	mrs_real avg_re;
	mrs_real avg_im;
	
	const mrs_string& mode = ctrl_mode_->to<mrs_string>();	    

	mrs_real interpolation = getctrl("mrs_natural/Interpolation")->to<mrs_natural>() * 1.0;
	mrs_real decimation = getctrl("mrs_natural/Decimation")->to<mrs_natural>() * 1.0;
	mrs_real tratio = interpolation / decimation;
	mrs_real beta = 0.66 + tratio/3.0;
	



	
	// calculate magnitude 
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
		
		
		for (t=0; t <= N2_; t++)
		{
		
			if ((t > 2) && (t <= N2_-2))
			{
				if ((mag_(t) > mag_(t-1)) &&
					(mag_(t) > mag_(t-2)) &&
					(mag_(t) > mag_(t+1)) && 
					(mag_(t) > mag_(t+2)))
				{
				}
			}
		}
		
	}
	

	if (mode == "loose_phaselock")
	{
		for (t=0; t <= N2_; t++)
		{
			re = amp = 2*t; 
			im = freq = 2*t+1;
			if (t == N2_) 
				re = 1;		
			
			phase_(t) = lastphases(t) + tratio * in(freq,0);
			while (phase_(t) > PI) 
				phase_(t) -= TWOPI;
			while (phase_(t) < -PI) 
				phase_(t) += TWOPI;
			
		}
	}
	
		
	
	// propagate phases for peaks 
	if ((mode == "identity_phaselock")||(mode == "scaled_phaselock"))
	{
		int previous_peak=0;
		int peak = 0;
		for (t=0; t <= N2_; t++)
		{
			re = amp = 2*t; 
			im = freq = 2*t+1;

			

			if ((t > 2) && (t <= N2_-2))
			{
				if ((mag_(t) > mag_(t-1)) &&
					(mag_(t) > mag_(t-2)) &&
					(mag_(t) > mag_(t+1)) && 
					(mag_(t) > mag_(t+2)))
				{


					if (mode == "identity_phaselock")
						phase_(t) = lastphases(t) + tratio * in(freq,0);
					else if (mode == "scaled_phaselock")
						phase_(t) = lastphases(regions(t)) + tratio * in(freq,0);
					

					while (phase_(t) > PI) 
						phase_(t) -= TWOPI;
					while (phase_(t) < -PI) 
						phase_(t) += TWOPI;
					lastphases(t) = phase_(t);			
					iphase_(t) = phase_(t);



					// calculate significant peaks and corresponding 
					// non-overlapping intervals 
					peak = t;
					for (int j=previous_peak; j< previous_peak + (int)((peak-previous_peak)/2.0); j++) 
					{
						regions(j) = previous_peak;
					}
					
					for (int j= previous_peak + (int)((peak-previous_peak)/2.0); j < peak; j++) 
					{
						regions(j) = peak;					
					}
					previous_peak = peak;
					





				}
			}
			else 
			{
				phase_(t) = lastphases(t) + tratio * in(freq,0);
				while (phase_(t) > PI) 
					phase_(t) -= TWOPI;
				while (phase_(t) < -PI) 
					phase_(t) += TWOPI;
				iphase_(t) = phase_(t);
				lastphases(t) = phase_(t);			
			}
		}
	}


	

	// resynthesis for all bins 
	for (t=0; t <= N2_; t++)
	{
		re = amp = 2*t; 
		im = freq = 2*t+1;
		if (t == N2_) 
			re = 1;		

		if ((mode == "identity_phaselock")||(mode == "scaled_phaselock"))
		{

			if ((t > 2) && (t <= N2_-2))
			{
				while (analysisphases(t) > PI) 
					analysisphases(t) -= TWOPI;
				while (analysisphases(t) < -PI) 
					analysisphases(t) += TWOPI;				
				

				iphase_(t) = phase_(regions(t)) + beta * (analysisphases(t) - analysisphases(regions(t)));
				while (iphase_(t) > PI) 
					iphase_(t) -= TWOPI;
				while (iphase_(t) < -PI) 
					iphase_(t) += TWOPI;
				lastphases(t) = iphase_(t);
			}


			if (ctrl_phaselock_->to<mrs_bool>())
			{
				cout << "Phase locking to analysis phases" << endl;
				iphase_ = analysisphases;
				ctrl_phaselock_->setValue(false);
			}
			
			out(re,0) = mag_(t) * cos(iphase_(t));
			if (t != N2_)
				out(im,0) = -mag_(t) * sin(iphase_(t));
		}
		else if (mode == "loose_phaselock") 
		{
			 if (t == N2_) 
				 re = 1;		
			
 			if ((t >= 2) && (t < N2_-2))
 			{
 				avg_re = mag_(t) * cos(phase_(t)) +
 					0.25 * mag_(t-1) * cos(phase_(t-1)) +
 					0.25 * mag_(t+1) * cos(phase_(t));
 				avg_im = -mag_(t) * sin(phase_(t)) -
 					-0.25 * mag_(t-1) * sin(phase_(t-1)) -
 					-0.25 * mag_(t+1) * sin(phase_(t));
 				lphase_(t) = -atan2(avg_im,avg_re);
 			}
			else 
				lphase_(t) = phase_(t);
			
 			lmag_(t) = mag_(t);

			if (ctrl_phaselock_->to<mrs_bool>())
			{
				cout << "Phase locking to analysis phases" << endl;
				lphase_ = analysisphases;
				ctrl_phaselock_->setValue(false);
			}
			
			while (lphase_(t) > PI) 
				lphase_(t) -= TWOPI;
			while (lphase_(t) < -PI) 
				lphase_(t) += TWOPI;
			lastphases(t) = lphase_(t);




 			out(re,0) = lmag_(t) * cos(lphase_(t));
 			if (t != N2_)
 				out(im,0) = -lmag_(t) * sin(lphase_(t));
		}
		else // classic
		{
			if (t == N2_) 
			{
				re = 1;
			}
			
			phase_(t) = lastphases(t) + tratio * in(freq,0);
			while (phase_(t) > PI) 
				phase_(t) -= TWOPI;
			while (phase_(t) < -PI) 
				phase_(t) += TWOPI;

			if (ctrl_phaselock_->to<mrs_bool>())
			{
				cout << "Phase locking to analysis phases" << endl;
				phase_ = analysisphases;
				ctrl_phaselock_->setValue(false);
			}
			
			out(re,0) = mag_(t) * cos(phase_(t));
			if (t != N2_)
				out(im,0) = -mag_(t) * sin(phase_(t));
			lastphases(t) = phase_(t);
			

		}
	}
}



		
		

	


 





	

	

	
	

	
