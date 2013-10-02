/*
** Copyright (C) 1998-2007 George Tzanetakis <gtzan@cs.uvic.ca>
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

#include "SpectralSNR.h"
#include "../common_source.h"

using namespace std;
using namespace Marsyas;

SpectralSNR::SpectralSNR(mrs_string name):MarSystem("SpectralSNR",name)
{
  addControls();
}

SpectralSNR::SpectralSNR(const SpectralSNR& a):MarSystem(a)
{
  N2_ = a.N2_;
}

SpectralSNR::~SpectralSNR()
{
}

void
SpectralSNR::addControls()
{
}

MarSystem*
SpectralSNR::clone() const
{
  return new SpectralSNR(*this);
}

void
SpectralSNR::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)
  ctrl_onSamples_->setValue((mrs_natural)1, NOUPDATE);
  ctrl_onObservations_->setValue((mrs_natural)1, NOUPDATE);
  ctrl_osrate_->setValue(ctrl_israte_->to<mrs_real>(), NOUPDATE);
  ctrl_onObsNames_->setValue(ctrl_inObsNames_, NOUPDATE);

  N2_ = inObservations_/2;
}

void
SpectralSNR::myProcess(realvec& in, realvec& out)
{
  mrs_natural t,o;
  for (t = 0; t < inSamples_; t++)
  {
    sum_ = 0.0;

    for (o=0; o < N2_; o++)
    {
      orig_ = in(o,t);
      extr_ = in(N2_+o, t);
      if (orig_ != 0.0)
        sum_ += (orig_ * orig_) / ((orig_-extr_) * (orig_-extr_));
    }

    if (sum_ != 0.0)
      sum_ /= N2_;
    out(0,t) = 10.0 * log10(sqrt(sum_));

    MRSMSG("sum("<<t<<") = " << sum_ << endl);
    MRSMSG("SpectralSNR (for frame "<<t<<") = " << out(0,t) << endl);
  }
}
