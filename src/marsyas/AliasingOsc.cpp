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

#include "common.h" 
#include "AliasingOsc.h"

using std::ostringstream;
using namespace Marsyas;

AliasingOsc::AliasingOsc(mrs_string name) : MarSystem("AliasingOsc", name)
{
	addControls();
}

AliasingOsc::AliasingOsc(const AliasingOsc& a) : MarSystem(a)
{
	// IMPORTANT!
	/// All member MarControlPtr have to be explicitly reassigned in
	/// the copy constructor.
	// Otherwise this would result in trying to deallocate them twice!
	//ctrl_gain_EXAMPLE_ = getctrl("mrs_real/gain");
}


AliasingOsc::~AliasingOsc()
{
}

MarSystem*
AliasingOsc::clone() const
{
	// Every MarSystem should do this.
	return new AliasingOsc(*this);
}

void
AliasingOsc::addControls()
{
	addctrl("mrs_real/frequency", 440.0);
	addctrl("mrs_real/cyclicrate", 0.0);

	setctrlState("mrs_real/frequency", true);
	setctrlState("mrs_real/cyclicrate", true);
}

void
AliasingOsc::myUpdate(MarControlPtr sender)
{
	MRSDIAG("AliasingOsc.cpp - AliasingOsc:myUpdate");

	mrs_real frequency = (getctrl("mrs_real/frequency")->to<mrs_real>());
	mrs_real irate = (getctrl("mrs_real/israte")->to<mrs_real>());
	incr_ = frequency / irate;

	cyclicRate_ = (getctrl("mrs_real/cyclicrate")->to<mrs_real>());


	/// Use the default MarSystem setup with equal input/output stream format.
	MarSystem::myUpdate(sender);
}

void
AliasingOsc::myProcess(realvec& in, realvec& out)
{
	(void) in;

	mrs_natural t,o;

	mrs_real frequency = (getctrl("mrs_real/frequency")->to<mrs_real>());
	mrs_real irate = (getctrl("mrs_real/israte")->to<mrs_real>());

	for (o = 0; o < inObservations_; o++)
	{
		for (t = 0; t < inSamples_; t++)
		{

			incr_ = (frequency * (in(0,t) + 1) ) / irate;
			currentValue_ = currentValue_ + incr_;
			out(0, t) = currentValue_;

			if (currentValue_ >= cyclicRate_)
			{
				out(0,t) = 0.9;
			}
			else
			{
				out(0,t) = -0.9;
			}

			if(currentValue_ >= 1.0)
			{
				currentValue_ = -1.0;
			}
		}
	}
}
