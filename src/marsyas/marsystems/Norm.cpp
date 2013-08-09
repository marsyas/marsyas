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

#include "Norm.h"

using std::ostringstream;
using namespace Marsyas;

Norm::Norm(mrs_string name):MarSystem("Norm",name)
{
}

Norm::~Norm()
{
}

MarSystem*
Norm::clone() const
{
  return new Norm(*this);
}

void
Norm::myProcess(realvec& in, realvec& out)
{
  realvec row(inSamples_);
  mrs_real mean;
  mrs_real std;
  mrs_natural t,o;

  for (o=0; o < inObservations_; o++)
  {
    // Calculate the mean and standard deviation of each row aka observation.
    for (t = 0; t < inSamples_; t++)
    {
      row(t) = in(o,t);
    }
    mean = row.mean();
    std =  row.std();
    // If standard deviation is zero, the input is constant, so
    // subtracting the mean will give zero output and we can just
    // set the standard deviation to 1.0 to avoid zero division woes.
    if (std == 0.0)
    {
      std = 1.0;
    }

    for (t = 0; t < inSamples_; t++)
    {
      out(o, t) = (in(o, t) - mean) / std;
    }
  }
}
