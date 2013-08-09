/*
** Copyright (C) 2009 Stefaan Lippens
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

#include "MixToMono.h"

using std::ostringstream;
using namespace Marsyas;

MixToMono::MixToMono(mrs_string name) : MarSystem("MixToMono", name)
{
  // Nothing special here, no controls to add.
}

MixToMono::MixToMono(const MixToMono& a) : MarSystem(a)
{
  // Nothing special here, no explicit control reassigning.
}


MixToMono::~MixToMono()
{
}

MarSystem*
MixToMono::clone() const
{
  return new MixToMono(*this);
}


void
MixToMono::myUpdate(MarControlPtr sender)
{
  // Start with the default MarSystem setup with equal input/output
  // stream format ...
  MarSystem::myUpdate(sender);

  /// ... but set the number of output observations to one.
  ctrl_onObservations_->setValue(1, NOUPDATE);

  // Calculate the weight factor to use during averaging.
  weight_ = (inObservations_ > 0) ? (1.0 / inObservations_) : 1.0;
}

void
MixToMono::myProcess(realvec& in, realvec& out)
{
  mrs_natural t,o;
  // Average the observation channels per sample.
  for (t = 0; t < inSamples_; t++)
  {
    out(0, t) = 0;
    for (o = 0; o < inObservations_; o++)
    {
      out(0, t) += (weight_ * in(o, t));
    }
  }
}
