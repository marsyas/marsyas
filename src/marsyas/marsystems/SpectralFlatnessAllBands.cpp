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

#include "../common_source.h"
#include "SpectralFlatnessAllBands.h"

using std::ostringstream;
using namespace Marsyas;

SpectralFlatnessAllBands::SpectralFlatnessAllBands(mrs_string name) : MarSystem("SpectralFlatnessAllBands", name)
{
  /// Add any specific controls needed by this MarSystem.
  addControls();
}

SpectralFlatnessAllBands::SpectralFlatnessAllBands(const SpectralFlatnessAllBands& a) : MarSystem(a)
{
}


SpectralFlatnessAllBands::~SpectralFlatnessAllBands()
{
}

MarSystem*
SpectralFlatnessAllBands::clone() const
{
  return new SpectralFlatnessAllBands(*this);
}

void
SpectralFlatnessAllBands::addControls()
{
}

void
SpectralFlatnessAllBands::myUpdate(MarControlPtr sender)
{
  MRSDIAG("SpectralFlatnessAllBands.cpp - SpectralFlatnessAllBands:myUpdate");

  /// Use the default MarSystem setup with equal input/output stream format.
  MarSystem::myUpdate(sender);

  // overwrite default settings as needed
  ctrl_onObservations_->setValue((mrs_natural)1, NOUPDATE);

  // Add prefix to the observation names.
  mrs_string inObsNames = ctrl_inObsNames_->to<mrs_string>();
  ctrl_onObsNames_->setValue(obsNamesAddPrefix(inObsNames,
                             "SpectralFlatnessAllBands_"), NOUPDATE);

}

void
SpectralFlatnessAllBands::myProcess(realvec& in, realvec& out)
{
  mrs_natural t,o;

  // swap Observations_ and Samples_ because it's intended
  // for spectral!

  for (t = 0; t < inSamples_; t++)
  {
    // we use log scale to calculate the geometric
    // mean, to avoid going lower than E-308, which is
    // the lower limit for doubles in C++.
    mrs_real geometric_mean = 0.0; // in log scale
    mrs_real arithmetic_mean = 0.0;
    for (o = 0; o < inObservations_; o++)
    {
      geometric_mean += log( in(o,t) ); // in log scale
      arithmetic_mean += in(o,t);
    }
    if (inObservations_ > 0) {
      geometric_mean /= inObservations_; // in log scale
      arithmetic_mean /= inObservations_;
    }
    geometric_mean = exp( geometric_mean ); // no more log scale
    if (arithmetic_mean > 0) {
      out(0, t) = geometric_mean / arithmetic_mean;
    } else {
      // treat complete silence as white noise
      out(0, t) = 1.0;
    }
  }

}
