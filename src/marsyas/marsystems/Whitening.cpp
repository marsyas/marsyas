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

#include "Whitening.h"

using std::ostringstream;
using namespace Marsyas;

Whitening::Whitening(mrs_string name):MarSystem("Whitening", name)
{
}

Whitening::Whitening(const Whitening& a) : MarSystem(a)
{
}

Whitening::~Whitening()
{
}

MarSystem*
Whitening::clone() const
{
  return new Whitening(*this);
}

void
Whitening::addControls()
{
}

void
Whitening::myUpdate(MarControlPtr sender)
{
  MarSystem::myUpdate(sender);
}


void
Whitening::myProcess(realvec& in, realvec& out)
{
  mrs_natural o,t;
  mrs_natural inObservations = getctrl("mrs_natural/inObservations")->to<mrs_natural>();
  mrs_real a;

  for (t = 0; t < inSamples_; t++)
  {
    for (o=0; o < inObservations; o=o+2)
    {
      a = sqrt( (in(o,t)*in(o,t)) + (in(o+1,t)*in(o+1,t) ));
      if (a>0) {
        out(o,t) = in(o,t)/a;
        out(o+1,t) = in(o+1,t)/a;
      }
      else {
        out(o,t)=0;
        out(o+1,t)=0;
      }
    }
  }
}
