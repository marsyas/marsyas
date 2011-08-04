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

HarmonicPeakInfo
HarmonicStrength::find_peak(mrs_real central_bin, mrs_realvec& in, mrs_natural t)
{
	HarmonicPeakInfo info;

	mrs_natural radius = 2;
	// find peak in 2*radius
	// in real-world signals, the harmonic isn't always a
	// literal multiple of the base frequency.  We allow a bit
	// of margin (the "radius") to find the best bin
	// TODO: use quadratic interpolation for a better estimate
	//       of the magnitude
	mrs_natural best_bin = -1;
	mrs_real best_magnitude = 0; 
	for (mrs_natural i=central_bin-radius; i<central_bin+1+radius; i++) {
		if (in(i,t) > best_magnitude) {
			best_bin = i;
			best_magnitude = in(i,t);
		}
	}

	info.bin_num = best_bin;
	info.magnitude = log(best_magnitude);
	return info;
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
	
	mrs_real bin = base_freq * freq2bin;

	// Iterate over the samples (remember, FFT is vertical)
	for (t = 0; t < inSamples_; t++)
	{
		HarmonicPeakInfo info = find_peak(bin, in, t);
		mrs_real base_strength = info.magnitude;
//		std::cout<<"\tfreq\tbin\tbin_num\tstrength"<<std::endl;
		for (h = 0; h < num_harmonics; h++)
		{
			mrs_real freq = harmonics(h) * base_freq;
			bin = freq * freq2bin;
			info = find_peak(bin, in, t);
			mrs_real strength = info.magnitude;

//			std::cout<<"\t"<<freq<<"\t"<<bin<<"\t"<<info.bin_num<<"\t"<<strength<<std::endl;

			// we're dealing with log(), so minus is division
			out(h, t) = strength - base_strength;
		}
	}
}

