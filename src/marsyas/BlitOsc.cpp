/*
** Copyright (C) 1998-2011 George Tzanetakis <gtzan@cs.uvic.ca>
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

#include "BlitOsc.h"

using namespace Marsyas;

BlitOsc::BlitOsc(mrs_string name):MarSystem("BlitOsc", name)
{
	addControls();
}

BlitOsc::BlitOsc(const BlitOsc& a) : MarSystem(a)
{
  // For any MarControlPtr in a MarSystem
  // it is necessary to perform this getctrl
  // in the copy constructor in order for cloning to work
}

BlitOsc::~BlitOsc()
{
}

MarSystem*
BlitOsc::clone() const
{
	return new BlitOsc(*this);
}

void BlitOsc::addControls()
{
	//Add specific controls needed by this MarSystem.
	addctrl("mrs_real/frequency", 440.0);
	addctrl("mrs_bool/noteon", false);
	addctrl("mrs_natural/type", 0);

	setctrlState("mrs_real/frequency", true);
	setctrlState("mrs_natural/type", true);
	setctrlState("mrs_bool/noteon", true);
}


void BlitOsc::myUpdate(MarControlPtr sender)
{
	delay_ = 1.9;
	le_ = 0.005;

	frequency_ = (getctrl("mrs_real/frequency")->to<mrs_real>());
	type_ = (getctrl("mrs_natural/type")->to<mrs_natural>());
	noteon_ = (getctrl("mrs_bool/noteon")->to<mrs_bool>());
	israte_ = (getctrl("mrs_real/israte")->to<mrs_real>());

	// Set all Coefficients to zero
	ax1_ = 0;
	ax2_ = 0;
	ay1_ = 0;
	ay2_ = 0;
	ly1_ = 0;

	a1_ = -2 * ((delay_ - 2) / (delay_ + 1));
	a2_ = ((delay_ - 1)*(delay_ - 2))/((delay_ + 1)*(delay_ + 2));

	phase_ = 0;
	inv_ = 1;

	switch (type_)
	{
		case 0: // Saw
			dc_ = (frequency_)/israte_;
			break;
		case 1: // Square
			// The frequency has to be doubled to compensate for
			// the frequency be being halved by the square wave
			// being bipolar
			frequency_ *= 2;
			dc_ = 0;
			break;
	}

	mrs_real d = israte_/frequency_;
	N_ = (mrs_natural)floor(d);

	// no change to network flow
	MarSystem::myUpdate(sender);
}

mrs_real BlitOsc::allPass(mrs_real x)
{
	mrs_real y = (a2_ * x) + (a1_ * ax1_) + (ax2_) - (a1_ * ay1_) - (a2_ * ay2_);
	ax2_ = ax1_;
	ax1_ = x;
	ay2_ = ay1_;
	ay1_ = y;
	return y;
}

mrs_real BlitOsc::leakyIntegrator(mrs_real x)
{
	mrs_real y = x + ((1 - le_) * ly1_);
	ly1_ = y;
	return y;
}

void BlitOsc::myProcess(realvec& in, realvec& out)
{
	for (mrs_natural t = 0; t < inSamples_; t++)
	{
		if (phase_ >= N_)
		{
			phase_ = 0;
			switch (type_)
			{
				case 0: // Saw
					out(0,t) = leakyIntegrator(allPass(0.95) - dc_);
					break;
				case 1: // Square
					out(0,t) = leakyIntegrator(allPass(0.95 * inv_));
					inv_ = (-inv_);
					break;
			}
		}
		else
		{
			phase_++;
			out(0,t) = leakyIntegrator(allPass(0) - dc_);
		}
	}
}
