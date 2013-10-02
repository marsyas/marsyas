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

#include "Square.h"
#include "../common_source.h"

using namespace std;
using namespace Marsyas;

Square::Square(mrs_string name):MarSystem("Square", name)
{
}

Square::Square(const Square& a) : MarSystem(a)
{
}


Square::~Square()
{
}

MarSystem*
Square::clone() const
{
  return new Square(*this);
}

void
Square::myUpdate(MarControlPtr sender)
{
  MRSDIAG("Square.cpp - Square:myUpdate");

  // Use the default matching of the output slice format with the input slice format.
  MarSystem::myUpdate(sender);
}

void
Square::myProcess(realvec& in, realvec& out)
{
  mrs_natural t,o;
  for (o=0; o < inObservations_; o++)
  {
    for (t = 0; t < inSamples_; t++)
    {
      out(o, t) = in(o, t) * in(o, t);
    }
  }
}
