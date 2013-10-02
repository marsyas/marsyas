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

#include "Reverse.h"
#include "../common_source.h"

using namespace std;
using namespace Marsyas;
using std::cout;
using std::endl;


Reverse::Reverse(mrs_string name):MarSystem("Reverse", name)
{
  addControls();
}

Reverse::Reverse(const Reverse& a) : MarSystem(a)
{

}

Reverse::~Reverse()
{
}

MarSystem*
Reverse::clone() const
{
  return new Reverse(*this);
}

void
Reverse::addControls()
{

}

void
Reverse::myUpdate(MarControlPtr sender)
{
  MRSDIAG("Reverse.cpp - Reverse:myUpdate");

  MarSystem::myUpdate(sender);
}

void
Reverse::myProcess(realvec& in, realvec& out)
{
  mrs_natural o,t;
  for (o=0; o < inObservations_; o++)
    for (t = 0; t < inSamples_; t++)
      out(o,t) = in(o,inSamples_-1-t);
}
