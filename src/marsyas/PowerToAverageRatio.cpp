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
#include "PowerToAverageRatio.h"

using std::ostringstream;
using namespace Marsyas;

PowerToAverageRatio::PowerToAverageRatio(mrs_string name) : MarSystem("PowerToAverageRatio", name)
{
	/// Add any specific controls needed by this MarSystem.
	addControls();
}

PowerToAverageRatio::PowerToAverageRatio(const PowerToAverageRatio& a) : MarSystem(a)
{
	/// All member MarControlPtr have to be explicitly reassigned in
	/// the copy constructor.
	ctrl_cancelDCoffset_ = getctrl("mrs_bool/cancelDCoffset");
}


PowerToAverageRatio::~PowerToAverageRatio()
{
}

MarSystem*
PowerToAverageRatio::clone() const
{
	return new PowerToAverageRatio(*this);
}

void
PowerToAverageRatio::addControls()
{
	/// Add any specific controls needed by this MarSystem.

	addctrl("mrs_bool/cancelDCoffset", false);
	setctrlState("mrs_bool/cancelDCoffset", true);

}

void
PowerToAverageRatio::myUpdate(MarControlPtr sender)
{
	MRSDIAG("PowerToAverageRatio.cpp - PowerToAverageRatio:myUpdate");

	/// Use the default MarSystem setup with equal input/output stream format.
	MarSystem::myUpdate(sender);

	// overwrite default settings as needed
	ctrl_onSamples_->setValue((mrs_natural)1, NOUPDATE);

	// Add prefix to the observation names.
	mrs_string inObsNames = ctrl_inObsNames_->to<mrs_string>();
	ctrl_onObsNames_->setValue(obsNamesAddPrefix(inObsNames, "PowerToAverageRatio_"), NOUPDATE);
}

void
PowerToAverageRatio::myProcess(realvec& in, realvec& out)
{
	mrs_natural t,o;

	/// Iterate over the observations and samples and do the processing.
	for (o = 0; o < inObservations_; o++)
	{
		mrs_real dc_offset = 0.0;
		if (ctrl_cancelDCoffset_->isTrue()) {
			// find mean sample value (i.e. the DC)
			for (t = 0; t < inSamples_; t++)
				dc_offset += in(o,t);
			dc_offset /= inSamples_;
		}
		mrs_real samp_max = 0.0;
		mrs_real rms = 0.0;
		for (t = 0; t < inSamples_; t++)
		{
			mrs_real samp_abs = fabs(in(o,t) - dc_offset);
			if (samp_max < samp_abs)
				samp_max = samp_abs;
			rms += samp_abs*samp_abs;
		}
		if (inSamples_ > 0)
			rms = sqrt( rms/inSamples_);

		mrs_real par = 0.0;
		if (rms != 0)
			par = samp_max / rms;

		out(o, 0) = par;
	}
}
