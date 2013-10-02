/*
 ** Copyright (C) 2010 Stefaan Lippens
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

#include "SubtractMean.h"
#include "../common_source.h"

using namespace std;
using namespace Marsyas;

SubtractMean::SubtractMean(mrs_string name) :
  MarSystem("SubtractMean", name)
{
  // No controls to add
}

SubtractMean::SubtractMean(const SubtractMean& a) :
  MarSystem(a)
{
  // No controls to add
}

SubtractMean::~SubtractMean()
{
}

MarSystem*
SubtractMean::clone() const
{
  return new SubtractMean(*this);
}

void SubtractMean::myUpdate(MarControlPtr sender)
{
  MRSDIAG("SubtractMean.cpp - SubtractMean:myUpdate");

  /// Use the default MarSystem setup with equal input/output stream format.
  MarSystem::myUpdate(sender);
}

void SubtractMean::myProcess(realvec& in, realvec& out)
{
  mrs_natural t,o;
  mrs_real mean;
  /// Iterate over the observations and samples and do the processing.
  for (o = 0; o < inObservations_; o++)
  {
    // Calculate channel mean
    mean = 0;
    for (t = 0; t < inSamples_; t++)
    {
      mean += in(o, t);
    }
    mean /= inSamples_;
    // Subtract mean
    for (t = 0; t < inSamples_; t++)
    {
      out(o, t) = in(o, t) - mean;
    }
  }
}
