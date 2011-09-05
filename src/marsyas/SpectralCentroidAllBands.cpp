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
#include "SpectralCentroidAllBands.h"

using namespace std;
using namespace Marsyas;

SpectralCentroidAllBands::SpectralCentroidAllBands(mrs_string name) : MarSystem("SpectralCentroidAllBands", name)
{
	/// Add any specific controls needed by this MarSystem.
	addControls();
}

SpectralCentroidAllBands::SpectralCentroidAllBands(const SpectralCentroidAllBands& a) : MarSystem(a)
{
}


SpectralCentroidAllBands::~SpectralCentroidAllBands()
{
}

MarSystem*
SpectralCentroidAllBands::clone() const
{
	return new SpectralCentroidAllBands(*this);
}

void
SpectralCentroidAllBands::addControls()
{
}

void
SpectralCentroidAllBands::myUpdate(MarControlPtr sender)
{
	MRSDIAG("SpectralCentroidAllBands.cpp - SpectralCentroidAllBands:myUpdate");

	/// Use the default MarSystem setup with equal input/output stream format.
	MarSystem::myUpdate(sender);

	// overwrite default settings as needed
	ctrl_onObservations_->setValue((mrs_natural)1, NOUPDATE);

	// Add prefix to the observation names.
	mrs_string inObsNames = ctrl_inObsNames_->to<mrs_string>();
	ctrl_onObsNames_->setValue(obsNamesAddPrefix(inObsNames,
	                           "SpectralCentroidAllBands_"), NOUPDATE);
}

void
SpectralCentroidAllBands::myProcess(realvec& in, realvec& out)
{
	mrs_natural t,o;

	// swap Observations_ and Samples_ because it's intended
	// for spectral!
	for (t = 0; t < inSamples_; t++)
	{
		mrs_real top_sum = 0.0;
		mrs_real bottom_sum = 0.0;
		for (o = 0; o < inObservations_; o++)
		{
			mrs_real bin_freq = ctrl_israte_->to<mrs_real>() * o;

			top_sum += bin_freq * in(o,t);
			bottom_sum += in(o,t);
		}
		if (bottom_sum > 0)
		{
			out(0, t) = top_sum / bottom_sum;
		}
		else
		{
			out(0, t) = 0.0;
		}
	}


}
