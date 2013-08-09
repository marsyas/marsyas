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

#include "Cartesian2Polar.h"

using std::ostringstream;
using namespace Marsyas;

Cartesian2Polar::Cartesian2Polar(mrs_string name):MarSystem("Cartesian2Polar", name)
{
}

Cartesian2Polar::Cartesian2Polar(const Cartesian2Polar& a) : MarSystem(a)
{
}

Cartesian2Polar::~Cartesian2Polar()
{
}

MarSystem*
Cartesian2Polar::clone() const
{
  return new Cartesian2Polar(*this);
}

void
Cartesian2Polar::addControls()
{
}

void
Cartesian2Polar::myUpdate(MarControlPtr sender)
{
  MarSystem::myUpdate(sender);
}


void
Cartesian2Polar::myProcess(realvec& in, realvec& out)
{
  mrs_natural o,t;
  mrs_natural N = inObservations_/2;

  for (t = 0; t < inSamples_; t++)
  {
    for (o=0; o < N; o++)
    {
      out(o,t) = sqrt(in(2*o,t)*in(2*o,t)+in(2*o+1,t)*in(2*o+1,t));
      out(o+N,t) = atan2(in(2*o+1,t), in(2*o,t));
    }
  }
}
