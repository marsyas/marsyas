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

#include "APDelayOsc.h"

using std::ostringstream;
using namespace Marsyas;

APDelayOsc::APDelayOsc(mrs_string name):MarSystem("APDelayOsc", name)
{
	frequency_ = 0;

    delaylineSize_ = 0;
	delayline_ = NULL;

    ly1_ = 0;  // Leaky Integrator last output

    a_ = 0;    // AllPass coefficient
    ax1_ = 0;  // AllPass last input
    ay1_ = 0;  // AllPass last output

	dx1_ = 0;  // dcBlocker last input
	dy1_ = 0;  // dcBlocker last output

    israte_ = 0;
    dcoff_ = 0;

    wp_ = 0;  // Write Pointer
    rp_ = 0;  // Read pointer one
    rpp_ = 0; // Read pointer two
    N_ = 0;

	noteon_ = true;

	addControls();
}

APDelayOsc::APDelayOsc(const APDelayOsc& a) : MarSystem(a)
{
	// IMPORTANT!
	/// All member MarControlPtr have to be explicitly reassigned in
	/// the copy constructor.
	// Otherwise this would result in trying to deallocate them twice!
}


APDelayOsc::~APDelayOsc()
{
}

MarSystem* APDelayOsc::clone() const
{
	// Every MarSystem should do this.
	return new APDelayOsc(*this);
}

void APDelayOsc::addControls()
{
	addctrl("mrs_real/frequency", 440.0);
	addctrl("mrs_natural/type", 0);
	addctrl("mrs_bool/noteon", false);

	setctrlState("mrs_real/frequency", true);
	setctrlState("mrs_natural/type", true);
	setctrlState("mrs_bool/noteon", true);
}

void APDelayOsc::myUpdate(MarControlPtr sender)
{
	MRSDIAG("APDelayOsc.cpp - APDelayOsc:myUpdate");

	frequency_ = (getctrl("mrs_real/frequency")->to<mrs_real>());
	noteon_ = (getctrl("mrs_bool/noteon")->to<mrs_bool>());
	type_ = (getctrl("mrs_natural/type")->to<mrs_natural>());

	israte_ = (getctrl("mrs_real/israte")->to<mrs_real>());

	// loweset frequency on a piano is 27.5Hz ... sample rate/27.5 for commute
	// this is the longest delay line required
	if (delaylineSize_ == 0) 
	{
		delaylineSize_ = israte_/27.5;
		delayline_.create((mrs_natural)delaylineSize_);
	  
		for (t = 0; t < delaylineSize_; t++)
		{
			delayline_(t) = 0.0;
		}
    }
  
	if (noteon_)
    {		
		ly1_ = 0;  // Leaky Integrator last output

		ax1_ = 0;  // AllPass last input
		ay1_ = 0;  // AllPass last output

		// Allpass coefficient
		// Chosen through experimentation
		mrs_real delay = 1.3;
		a_ = (1 - delay)/(1 + delay);

		// The amount of delay to generate the correct pitch
		mrs_real d = israte_/frequency_;
		N_ = (mrs_natural)floor(d);

		// Choses the differences between generating saw
		// or square waves.
		switch (type_)
		{
			case 0: // Saw
				dc_ = frequency_ / israte_;
				neg_ = 1;
				break;
			case 1: // Square
				dc_ = 0;
				neg_ = -1;
				N_ = N_/2; // Because this is bipolar
				           // we need to halve the period
				break;
		}

		// Initialize the delay line
		for (t = 0; t < N_; t++)
		{
			delayline_(t) = 0;
		}
		// initialize the system with an impulse
		delayline_(1) = 0.95;

		// Initialize our read and write pointers
		rp_ = 1;
		rpp_ = 0; // Not used, but could be used to do pitch shifting
		wp_ = N_-1;
    }
	/// Use the default MarSystem setup with equal input/output stream format.
	MarSystem::myUpdate(sender);
}

mrs_real APDelayOsc :: allPass(mrs_real x)
{
	mrs_real y = (a_ * x) + (ax1_ - (a_ * ay1_));
	ax1_ = x;
	ay1_ = y;
	return y;
}

mrs_real APDelayOsc :: leakyIntegrator(mrs_real x)
{
	mrs_real y = x + (0.995 * ly1_);
	ly1_ = y;
	return y;
}

mrs_real APDelayOsc :: dcBlocker(mrs_real x)
{
	mrs_real y = (x - dx1_) + (0.95 * dy1_);
	dx1_ = x;
	dy1_ = y;
	return y;
}

void APDelayOsc::myProcess(realvec& in, realvec& out)
{
	(void) in;

	mrs_natural t,o;
	mrs_real y, x = 0;

	if (noteon_)
	{
		for (t = 0; t < inSamples_; t++)
		{
			// rp holds the current sample
			x  = delayline_(rp_);

			y = allPass(x);

			// Write current sample back into the delay buffer.
			// If square each sample is negated.
			delayline_(wp_) = y * neg_;

			// The leaky Integrator is used to apply an
			// exponential decay as frequencies increase.
			y = leakyIntegrator(y - dc_);

			// Increment the delay line pointers
			wp_ = (wp_ + 1)  % N_;
			rp_ = (rp_ + 1)  % N_;

			out(0,t) = y;
		}
	}
}
