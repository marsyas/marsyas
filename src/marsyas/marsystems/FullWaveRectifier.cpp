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

#include "FullWaveRectifier.h"

using std::ostringstream;
using namespace Marsyas;

FullWaveRectifier::FullWaveRectifier(mrs_string name) : MarSystem("FullWaveRectifier",name)
{
}


FullWaveRectifier::~FullWaveRectifier()
{
}

MarSystem*
FullWaveRectifier::clone() const
{
  return new FullWaveRectifier(*this);
}

void
FullWaveRectifier::myUpdate(MarControlPtr sender)
{
  /// Use the default MarSystem setup with equal input/output stream format.
  MarSystem::myUpdate(sender);
}

void
FullWaveRectifier::myProcess(realvec& in, realvec& out)
{
  mrs_natural o,t;
  // Copy the absolute values of the samples from the input to the output.
  for (o = 0; o < inObservations_; o++)
  {
    for (t = 0; t < inSamples_; t++)
    {
      out(o, t) = (in(o, t) > 0) ? in(o, t) : -in(o, t);
    }
  }
}
