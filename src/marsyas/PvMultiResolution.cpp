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

#include "PvMultiResolution.h"

using namespace std;
using namespace Marsyas;

PvMultiResolution::PvMultiResolution(string name):MarSystem("PvMultiResolution", name)
{
	flux_ = new Flux("flux");
	r_ = 0.1;
	m_ = 0.999;
	
	addControls();
}

PvMultiResolution::PvMultiResolution(const PvMultiResolution& a) : MarSystem(a)
{
	ctrl_mode_ = getctrl("mrs_string/mode");
	ctrl_transient_ = getctrl("mrs_bool/transient");
	r_ = 1.0e-13;
	m_ = 0.9;
	flux_ = new Flux("flux");
}


PvMultiResolution::~PvMultiResolution()
{
	delete flux_;
}

MarSystem*
PvMultiResolution::clone() const
{
	return new PvMultiResolution(*this);
}

void
PvMultiResolution::addControls()
{

	addctrl("mrs_string/mode", "long", ctrl_mode_);
	addctrl("mrs_bool/transient", false, ctrl_transient_);
}

void
PvMultiResolution::myUpdate(MarControlPtr sender)
{
	MRSDIAG("PvMultiResolution.cpp - PvMultiResolution:myUpdate");

	(void) sender;
	ctrl_onSamples_->setValue(ctrl_inSamples_, NOUPDATE);
	ctrl_onObservations_->setValue(ctrl_inObservations_->to<mrs_natural>() / 2, NOUPDATE);
	ctrl_osrate_->setValue(ctrl_israte_, NOUPDATE);
	
	median_buffer_.create(20);
	mbindex_ = 0;


	powerSpectrum_.create(onObservations_/2,1);
	whiteSpectrum_.create(onObservations_/2,1);
	
	flux_->updctrl("mrs_natural/inSamples", 1);
	flux_->updctrl("mrs_natural/inObservations", onObservations_/2);
	flux_->updctrl("mrs_real/israte", 44100);
	flux_->updctrl("mrs_string/mode", "DixonDAFX06");
	fluxval_.create(1,1);
	
}

void
PvMultiResolution::myProcess(realvec& in, realvec& out)
{
	const mrs_string& mode = ctrl_mode_->to<mrs_string>();
	


	mrs_real max = DBL_MIN;
	mrs_real maxLong = DBL_MIN;
	mrs_real powerShort = 0.0;
	mrs_real powerLong = 0.0;
	


	
	if (mode == "short")
	{
		// short window 
		for (o=0; o < inObservations_/2; o++)
			for (t = 0; t < inSamples_; t++)
			{
				out(o,t) = in(o, t);		
			}
		for (o=0; o < onObservations_/2; o++) 
			for (t = 0; t < inSamples_; t++)
			{
				
				out(2*o, t) = 0.75 * out(2*o,t);
			}


	}
	else if (mode == "long") 
	{

		// long window 
		for (o=inObservations_/2; o < inObservations_; o++)
			for (t = 0; t < inSamples_; t++)
			{
				out(o-inObservations_/2,t) = in(o,t);
			}
		
		for (o=0; o < onObservations_/2; o++) 
			for (t = 0; t < inSamples_; t++)
			{
				out(2*o, t) = out(2*o,t);
			}
	}
	else if (mode == "shortlong_mixture") 
	{

		
		for (o=0; o < inObservations_/2; o++)
			for (t = 0; t < inSamples_; t++)
			{
				out(o,t) = in(o, t);		
			}
		
		/* use long window for frequencies lower than approx. 2000 Hz 
		 and short window for higher frequencies */ 
		for (o=inObservations_/2; o < inObservations_/2 + 200; o++)
			for (t = 0; t < inSamples_; t++)
			{
				out(o-inObservations_/2,t) = in(o,t);
			}
		
		for (o=0; o < 200; o++) 
			for (t = 0; t < inSamples_; t++)
			{
				out(2*o, t) = 2 * out(2*o,t);
			}
	}
	
	else if (mode == "transient_switch")
	{

		// short window 
		for (o=0; o < inObservations_/2; o++)
			for (t = 0; t < inSamples_; t++)
			{
				out(o,t) = in(o, t);		
			}

		
		/* calculate power and use median for dynamic thresholding */ 
		for (o=0; o < onObservations_/2; o++) 
			for (t = 0; t < inSamples_; t++)
			{
				powerSpectrum_(o,0) = out(2*o,t) * out(2*o,t);
			}


		// adaptive pre-whitening 
		for (o=0; o < onObservations_/2; o++) 
		{
			
			if (powerSpectrum_(o,0) < r_) 
				whiteSpectrum_(o,0) = r_;
			else 
			{
				if (m_ * whiteSpectrum_(o,0) > powerSpectrum_(o,0))
					whiteSpectrum_(o,0) = m_ * whiteSpectrum_(o,0);
				else
					whiteSpectrum_(o,0) = powerSpectrum_(o,0);
			}
			powerSpectrum_(o,0) = powerSpectrum_(o,0) / whiteSpectrum_(o,0);
		}
		

		
		flux_->process(powerSpectrum_, fluxval_); 
				
		
		median_buffer_(mbindex_) = fluxval_(0,0);
		mbindex_++;
		if (mbindex_ == 20)
		{
			mbindex_ = 0;
		}



		
		if (fluxval_(0,0) - median_buffer_.median() <= 35.0)    // steady state use long window 
		{
			for (o=inObservations_/2; o < inObservations_; o++)
				for (t = 0; t < inSamples_; t++)
				{
					out(o-inObservations_/2,t) = in(o,t);
				}
			
			for (o=0; o < onObservations_/2; o++) 
				for (t = 0; t < inSamples_; t++)
				{
					out(2*o, t) = 1.0 * out(2*o,t);
				}
			ctrl_transient_->setValue(false, NOUPDATE);
		}
		else // transient 
		{
			cout << "TRANSIENT " << endl;
			
			// use short 
			for (o=0; o < inObservations_/2; o++)
				for (t = 0; t < inSamples_; t++)
				{
					out(o,t) = in(o, t);		
				}

			for (o=0; o < onObservations_/2; o++) 
				for (t = 0; t < inSamples_; t++)
				{
					out(2*o, t) = 1.0 * out(2*o,t);
				}

			ctrl_transient_->setValue(true, NOUPDATE);			
			// cout<< fluxval_(0,0)-median_buffer_.median() << endl;		
		}
	}
	
	
	
}


