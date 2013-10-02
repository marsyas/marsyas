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

#include <cmath>

#include "Compressor.h"
#include "../common_source.h"


using std::ostringstream;
using namespace Marsyas;

Compressor::Compressor(mrs_string name):MarSystem("Compressor", name)
{
  addControls();
}

Compressor::Compressor(const Compressor& a) : MarSystem(a)
{
}


Compressor::~Compressor()
{
}

MarSystem*
Compressor::clone() const
{
  return new Compressor(*this);
}

void
Compressor::addControls()
{

}

void
Compressor::myUpdate(MarControlPtr sender)
{
  MRSDIAG("Compressor.cpp - Compressor:myUpdate");

  MarSystem::myUpdate(sender);
}

void
Compressor::myProcess(realvec& in, realvec& out)
{
  mrs_natural o,t;
  for (o=0; o < inObservations_; o++)
    for (t = 0; t < inSamples_; t++) {
      if(in(o,t) > 0.0001)
        out(o,t) = 5.213 * log(1 + 10 * sqrt(in(o,t)))/12.5;
      else
        out(o,t) = 5.213 * log(1.1)/12.5;

      //cout << "Compressor:" << in(o,t) << "\t-->\t" << out(o,t) << endl;
    }
}
