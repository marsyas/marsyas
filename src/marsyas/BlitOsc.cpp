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
	//Add any specific controls needed by BlitOsc
	//(default controls all MarSystems should have
	//were already added by MarSystem::addControl(),
	//called by :MarSystem(name) constructor).
	//If no specific controls are needed by a MarSystem
	//there is no need to implement and call this addControl()
	//method (see for e.g. Rms.cpp)
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

	setctrlState("mrs_real/frequency", true);
	setctrlState("mrs_bool/noteon", true);
}


void BlitOsc::myUpdate(MarControlPtr sender)
{
	delay_ = 2.1;
	le_ = 0.005;

	frequency_ = (getctrl("mrs_real/frequency")->to<mrs_real>());
	noteon_ = (getctrl("mrs_bool/noteon")->to<mrs_bool>());

	israte_ = (getctrl("mrs_real/israte")->to<mrs_real>());

	mrs_real d = israte_/frequency_;

	// Set all Coefficients to zero
	ax1_ = 0;
	ax2_ = 0;
	ay1_ = 0;
	ay2_ = 0;
	ly1_ = 0;

	a1_ = -2 * ((delay_ - 2) / (delay_ + 1));
	a2_ = ((delay_ - 1)*(delay_ - 2))/((delay_ + 1)*(delay_ + 2));

	N_ = (mrs_natural)floor(d);

	dc_ = frequency_/israte_;

	//phase_ = 0;

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

	// It is important to loop over both observations
	// and channels so that for example a gain can be
	// applied to multi-channel signals
	for (mrs_natural t = 0; t < inSamples_; t++)
	{
		if (phase_ >= N_)
		{
			phase_ = 0;
			out(0,t) = leakyIntegrator(allPass(0.95) - dc_);
		}
		else
		{
			phase_++;
			out(0,t) = leakyIntegrator(allPass(0) - dc_);
		}
	}
}
