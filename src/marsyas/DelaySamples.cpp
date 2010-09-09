/*
 ** Copyright (C) 2010 Stefaan Lippens
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
#include "DelaySamples.h"

using std::ostringstream;
using namespace Marsyas;

DelaySamples::DelaySamples(mrs_string name) :
	MarSystem("DelaySamples", name)
{
	/// Add any specific controls needed by this MarSystem.
	addControls();
}

DelaySamples::DelaySamples(const DelaySamples& a) :
	MarSystem(a)
{
	/// All member MarControlPtr have to be explicitly reassigned in
	/// the copy constructor.
	ctrl_delay_ = getctrl("mrs_natural/delay");
}

DelaySamples::~DelaySamples()
{
}

MarSystem*
DelaySamples::clone() const
{
	return new DelaySamples(*this);
}

/// Add any specific controls needed by this MarSystem.
void DelaySamples::addControls()
{
	addctrl("mrs_natural/delay", 0, ctrl_delay_);
	setctrlState("mrs_natural/delay", true);
}

void DelaySamples::myUpdate(MarControlPtr sender)
{
	/// Use the default MarSystem setup with equal input/output stream format.
	MarSystem::myUpdate(sender);
	// TODO: prefix with "DelayX" ?

	// Cache delay value for in myProcess
	delay_ = ctrl_delay_->to<mrs_natural> ();
	if (delay_ < 0)
	{
		setctrl("mrs_natural/delay", 0);
		delay_ = 0;
	}

	// Allocate and initialize the buffers.
	this->memory_.stretch(inObservations_, delay_);
	this->memory_.setval(0.0);
}

void DelaySamples::myProcess(realvec& in, realvec& out)
{
	mrs_natural t, o;

	/// Iterate over the observations and samples and do the processing.
	if (delay_ < inSamples_)
	{
		for (o = 0; o < inObservations_; o++)
		{
			// Fetch initial part from memory.
			for (t = 0; t < delay_; t++)
			{
				out(o, t) = memory_(o, t);
			}
			// Copy from input where possible
			for (t = delay_; t < inSamples_; t++)
			{
				out(o, t) = in(o, t - delay_);
			}
			// Update memory
			for (t = 0; t < delay_; t++)
			{
				memory_(o, t) = in(o, inSamples_ - delay_ + t);
			}
		}
	}
	else
	{
		for (o = 0; o < inObservations_; o++)
		{
			// Fetch from memory
			for (t = 0; t < inSamples_; t++)
			{
				out(o, t) = memory_(o, t);
			}
			// Shift memory
			for (t = 0; t < delay_ - inSamples_; t++)
			{
				memory_(o, t) = memory_(o, t + inSamples_);
			}
			// Update memory
			for (t = 0; t < inSamples_; t++)
			{
				memory_(o, t + delay_ - inSamples_) = in(o, t);
			}

		}
	}
}
