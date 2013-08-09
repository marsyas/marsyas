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

#include "Polar2Cartesian.h"

using std::ostringstream;
using namespace Marsyas;

Polar2Cartesian::Polar2Cartesian(mrs_string name):MarSystem("Polar2Cartesian", name)
{
}

Polar2Cartesian::Polar2Cartesian(const Polar2Cartesian& a) : MarSystem(a)
{
}

Polar2Cartesian::~Polar2Cartesian()
{
}

MarSystem*
Polar2Cartesian::clone() const
{
  return new Polar2Cartesian(*this);
}

void
Polar2Cartesian::addControls()
{
}

void
Polar2Cartesian::myUpdate(MarControlPtr sender)
{
  MarSystem::myUpdate(sender);
}


void
Polar2Cartesian::myProcess(realvec& in, realvec& out)
{
  mrs_natural t,o;
  mrs_natural N = inObservations_/2;

  for (t = 0; t < inSamples_; t++)
  {
    for (o=0; o < N; o++)
    {
      out(2*o,t) = in(o,t)*cos(in(o+N,t));
      out(2*o+1,t) = in(o,t)*sin(in(o+N,t));
    }
  }
}
