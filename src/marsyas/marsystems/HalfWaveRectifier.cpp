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

#include "HalfWaveRectifier.h"

using std::ostringstream;
using namespace Marsyas;

HalfWaveRectifier::HalfWaveRectifier(mrs_string name): MarSystem("HalfWaveRectifier",name)
{
}


HalfWaveRectifier::~HalfWaveRectifier()
{
}

MarSystem*
HalfWaveRectifier::clone() const
{
  return new HalfWaveRectifier(*this);
}

void
HalfWaveRectifier::myUpdate(MarControlPtr sender)
{
  /// Use the default MarSystem setup with equal input/output stream format.
  MarSystem::myUpdate(sender);
}

void
HalfWaveRectifier::myProcess(realvec& in, realvec& out)
{
  mrs_natural o,t;
  // Positive values are kept, negative values are trimmed to zero.
  for (t = 0; t < inSamples_; t++)
  {
    for (o = 0; o < inObservations_; o++)
    {
      out(o, t) = (in(o, t) > 0) ? in(o, t) : 0;
    }
  }
}
