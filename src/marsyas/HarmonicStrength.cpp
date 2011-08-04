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
#include "HarmonicStrength.h"

using std::ostringstream;
using namespace Marsyas;

HarmonicStrength::HarmonicStrength(mrs_string name) : MarSystem("HarmonicStrength", name)
{
	addControls();
}

HarmonicStrength::HarmonicStrength(const HarmonicStrength& a) : MarSystem(a)
{
	ctrl_base_frequency_ = getctrl("mrs_real/base_frequency");
	ctrl_harmonics_ = getctrl("mrs_realvec/harmonics");
	ctrl_harmonicsSize_ = getctrl("mrs_natural/harmonicsSize");
}


HarmonicStrength::~HarmonicStrength()
{
}

MarSystem*
HarmonicStrength::clone() const
{
	return new HarmonicStrength(*this);
}

void
HarmonicStrength::addControls()
{
	addctrl("mrs_real/base_frequency", 440.0, ctrl_base_frequency_);
	addctrl("mrs_realvec/harmonics", realvec(), ctrl_harmonics_);
	addctrl("mrs_natural/harmonicsSize", 1, ctrl_harmonicsSize_);
	setctrlState("mrs_natural/harmonicsSize", true);
}

void
HarmonicStrength::myUpdate(MarControlPtr sender)
{
	MRSDIAG("HarmonicStrength.cpp - HarmonicStrength:myUpdate");

	/// Use the default MarSystem setup with equal input/output stream format.
	MarSystem::myUpdate(sender);
	ctrl_onObservations_->setValue(ctrl_harmonicsSize_->to<mrs_natural>(), NOUPDATE);
}

void
HarmonicStrength::myProcess(realvec& in, realvec& out)
{
	mrs_natural h,t;

	mrs_natural num_harmonics = ctrl_harmonicsSize_->to<mrs_natural>();
	mrs_real base_freq = ctrl_base_frequency_->to<mrs_real>();
        MarControlAccessor acc(ctrl_harmonics_);
        mrs_realvec& harmonics = acc.to<mrs_realvec>();
	mrs_real srate = ctrl_israte_->to<mrs_real>();
	mrs_real freq2bin = srate / inObservations_ / 4.0;
	
	mrs_natural bin = base_freq * freq2bin;;
	// TODO: really improve this!

	// Iterate over the samples (remember, FFT is vertical)
	for (t = 0; t < inSamples_; t++)
	{
		mrs_real base_strength = in(bin,t) + in(bin+1,t);

		std::cout<<"freq\tbin\tstrength"<<std::endl;
		for (h = 0; h < num_harmonics; h++)
		{
			mrs_real freq = harmonics(h) * base_freq;
			bin = freq * freq2bin;
			mrs_real strength = in(bin,t) + in(bin+1,t);

			std::cout<<freq<<"\t"<<bin<<"\t"<<strength<<std::endl;

			out(h, t) = strength/base_strength;
		}
	}
	//std::cout<<in;
}

