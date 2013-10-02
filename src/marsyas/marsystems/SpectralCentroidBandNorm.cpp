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
#include "SpectralCentroidBandNorm.h"

using std::ostringstream;
using namespace Marsyas;

SpectralCentroidBandNorm::SpectralCentroidBandNorm(mrs_string name) : MarSystem("SpectralCentroidBandNorm", name)
{
  /// Add any specific controls needed by this MarSystem.
  addControls();
}

SpectralCentroidBandNorm::SpectralCentroidBandNorm(const SpectralCentroidBandNorm& a) : MarSystem(a)
{
}


SpectralCentroidBandNorm::~SpectralCentroidBandNorm()
{
}

MarSystem*
SpectralCentroidBandNorm::clone() const
{
  return new SpectralCentroidBandNorm(*this);
}

void
SpectralCentroidBandNorm::addControls()
{
  /// Add any specific controls needed by this MarSystem.
  addctrl("mrs_real/expected_peak", 100.0);
}

void
SpectralCentroidBandNorm::myUpdate(MarControlPtr sender)
{
  MRSDIAG("SpectralCentroidBandNorm.cpp - SpectralCentroidBandNorm:myUpdate");

  /// Use the default MarSystem setup with equal input/output stream format.
  MarSystem::myUpdate(sender);

  //ctrl_onSamples_->setValue((mrs_natural)1, NOUPDATE);
  ctrl_onObservations_->setValue((mrs_natural)1, NOUPDATE);
  ctrl_onObsNames_->setValue("SCN_" + ctrl_inObsNames_->to<mrs_string>(),
                             NOUPDATE);
}

void
SpectralCentroidBandNorm::myProcess(realvec& in, realvec& out)
{
  mrs_natural t,o;
  expected_peak_ = getctrl("mrs_real/expected_peak")->to<mrs_real>();

  /// Iterate over the observations and samples and do the processing.
  for (t = 0; t < inSamples_; t++)
  {
    mrs_real m0 = 0.0;
    mrs_real m1 = 0.0;
    mrs_natural low_bin = (mrs_natural) (0.9*expected_peak_
                                         / ((mrs_real) israte_));
    mrs_natural high_bin = (mrs_natural) (1.9*expected_peak_
                                          / ((mrs_real) israte_));
    //printf("%li\t%li\n", low_bin, high_bin);
    for (o=low_bin; o < high_bin; o++)
    {
      m1 += o * in(o,t);
      m0 += in(o,t);
    }
    mrs_real sc = 0.0;
    if (m0 != 0.0) {
      //sc = (m1 / m0) / (high_bin - low_bin);
      sc = (m1 / m0) * israte_;
    }
    //printf("%li\t%li\t%.3f\n", low_bin, high_bin, sc);
    out(0, t) = fabs(expected_peak_ - sc) / expected_peak_;
    //printf("%.3f\t%.3f\t%.3f\n", expected_peak_, sc, out(0,t));
  }
}
