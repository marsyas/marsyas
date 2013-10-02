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

#include "Differentiator.h"
#include "../common_source.h"

using std::ostringstream;
using namespace Marsyas;

Differentiator::Differentiator(mrs_string name):MarSystem("Differentiator", name)
{
}

Differentiator::Differentiator(const Differentiator& a) : MarSystem(a)
{
}


Differentiator::~Differentiator()
{
}

MarSystem*
Differentiator::clone() const
{
  return new Differentiator(*this);
}

void
Differentiator::myUpdate(MarControlPtr sender)
{
  MRSDIAG("Differentiator.cpp - Differentiator:myUpdate");

  // Use the default matching of the output slice format and rate
  // with the input slice format and rate.
  MarSystem::myUpdate(sender);

  // Allocate the buffer_ based on inObservations.
  buffer_.stretch(inObservations_);
  buffer_.setval(0);
}

void
Differentiator::myProcess(realvec& in, realvec& out)
{
  mrs_natural t,o;
  for (o=0; o < inObservations_; o++)
  {
    // Calculate the difference of the first sample with the last sample of
    // the previous slice.
    out(o,0) = in(o,0) - buffer_(o);
    // Calculate the differences for the other samples.
    for (t = 1; t < inSamples_; t++)
    {
      out(o, t) = in(o, t) - in(o, t-1);
    }
    // Store the last sample in the buffer.
    buffer_(o) = in(o, inSamples_-1);
  }
}
