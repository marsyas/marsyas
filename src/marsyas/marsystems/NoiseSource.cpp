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

#include "NoiseSource.h"

using std::ostringstream;
using namespace Marsyas;

NoiseSource::NoiseSource(mrs_string name):MarSystem("NoiseSource",name)
{
  addControls();
}

NoiseSource::~NoiseSource()
{
}

MarSystem*
NoiseSource::clone() const
{
  return new NoiseSource(*this);
}

void
NoiseSource::addControls()
{
}



void
NoiseSource::myUpdate(MarControlPtr sender)
{
  MarSystem::myUpdate(sender);

}

void
NoiseSource::myProcess(realvec &in, realvec &out)
{
  (void) in;
  for (mrs_natural t=0; t < onSamples_; ++t)
      for (mrs_natural o=0; o < onObservations_; ++o)
        out(o,t) = (mrs_real)(2.0 * rand() / (RAND_MAX + 1.0) )-1;
}





