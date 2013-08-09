/*
** Copyright (C) 2009 Stefaan Lippens
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

#include "SliceDelta.h"

using namespace std;
using namespace Marsyas;

MarSystem*
SliceDelta::clone() const
{
  return new SliceDelta(*this);
}

void
SliceDelta::myUpdate(MarControlPtr sender)
{
  // Use the default MarSystem setup of equal input/output stream formats.
  MarSystem::myUpdate(sender);

  // Allocate the previousInput_ buffer based on inObservations and inSamples.
  mrs_natural rows = getctrl("mrs_natural/inObservations")->to<mrs_natural>();
  mrs_natural cols = getctrl("mrs_natural/inSamples")->to<mrs_natural>();
  this->previousInputSlice_.stretch(rows, cols);
  this->previousInputSlice_.setval(0.0);
}

void
SliceDelta::myProcess(realvec& in, realvec& out)
{
  mrs_natural t,o;
  // Iterate over observations and samples.
  for (o=0; o < inObservations_; o++)
  {
    for (t = 0; t < inSamples_; t++)
    {
      // Output the difference with the previous slice.
      out(o, t) = in(o, t) - this->previousInputSlice_(o, t);
      // Store as previous input sample for later.
      this->previousInputSlice_(o, t) = in(o, t);
    }
  }
}
