/*
** Copyright (C) 1998-2006 George Tzanetakis <gtzan@cs.uvic.ca>
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


#include "DownSampler.h"
#include "../common_source.h"

using std::ostringstream;
using namespace Marsyas;


DownSampler::DownSampler(mrs_string name):MarSystem("DownSampler",name)
{
  addControls();
}

DownSampler::~DownSampler()
{
}

DownSampler::DownSampler(const DownSampler& a) : MarSystem(a)
{
  /// All member MarControlPtr have to be explicitly reassigned in
  /// the copy constructor.
  ctrl_factor_ = getControl("mrs_natural/factor");
}

MarSystem*
DownSampler::clone() const
{
  return new DownSampler(*this);
}

void
DownSampler::addControls()
{
  addControl("mrs_natural/factor", 2, ctrl_factor_);
  ctrl_factor_->setState(true);
}

void
DownSampler::myUpdate(MarControlPtr sender)
{
  MRSDIAG("DownSampler.cpp - DownSampler:myUpdate");

  // Use the default MarSystem setup with equal input/output stream format ...
  MarSystem::myUpdate(sender);

  // ... but override the onSamples and osrate settings based on the
  // downsample factor.
  mrs_natural factor = ctrl_factor_->to<mrs_natural>();
  ctrl_osrate_->setValue(ctrl_israte_->to<mrs_real>() / factor, NOUPDATE);
  // When inSamples is divisible by factor, the number of output samples
  // onSamples is just the integer division inSamples / factor.
  // Otherwise, onSamples is the biggest integer obeying
  //   factor * (onSamples - 1) < inSamples
  // which is ceil(inSamples / factor).
  mrs_natural onSamples = (mrs_natural) ceil((mrs_real)(ctrl_inSamples_->to<mrs_natural>()) / factor);
  ctrl_onSamples_->setValue(onSamples, NOUPDATE);
}

void
DownSampler::myProcess(realvec& in, realvec& out)
{
  mrs_natural t,o;
  mrs_natural factor = ctrl_factor_->to<mrs_natural>();

  // Only copy samples at multiples of factor.
  for (o=0; o < inObservations_; o++)
  {
    for (t = 0; t < onSamples_; t++)
    {
      out(o, t) = in(o, t * factor);
    }
  }
}
