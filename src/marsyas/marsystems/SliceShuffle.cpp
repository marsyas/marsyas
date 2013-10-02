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

#include "SliceShuffle.h"
#include "../common_source.h"

using namespace std;
using namespace Marsyas;

SliceShuffle::SliceShuffle(mrs_string name) : MarSystem("SliceShuffle", name)
{
  /// Add any specific controls needed by this MarSystem.
  addControls();
}

SliceShuffle::SliceShuffle(const SliceShuffle& a) : MarSystem(a)
{
  /// All member MarControlPtr have to be explicitly reassigned in
  /// the copy constructor.
  ctrl_bufferSize_ = getControl("mrs_natural/bufferSize");
}


SliceShuffle::~SliceShuffle()
{
}

MarSystem*
SliceShuffle::clone() const
{
  return new SliceShuffle(*this);
}

void
SliceShuffle::addControls()
{
  /// Add any specific controls needed by this MarSystem.

  addControl("mrs_natural/bufferSize", 10, ctrl_bufferSize_);
  setctrlState("mrs_natural/bufferSize", true);
}

void
SliceShuffle::myUpdate(MarControlPtr sender)
{
  MRSDIAG("SliceShuffle.cpp - SliceShuffle:myUpdate");

  /// Use the default MarSystem setup with equal input/output stream format.
  MarSystem::myUpdate(sender);

  // Cache the buffer size
  bufferSize_ = getControl("mrs_natural/bufferSize")->to<mrs_natural>();

  // Allocate the slice buffer.
  sliceBuffer_.stretch(inObservations_, inSamples_ * bufferSize_);
  sliceBuffer_.setval(0.0);
}

void
SliceShuffle::myProcess(realvec& in, realvec& out)
{
  mrs_natural t,o;
  /// Pick a random slice.
  mrs_natural slice_t = (rand() % bufferSize_) * inSamples_;

  /// Get slice from buffer and store the new input.
  for (o = 0; o < inObservations_; o++)
  {
    for (t = 0; t < inSamples_; t++)
    {
      out(o, t) = sliceBuffer_(o, slice_t + t);
      sliceBuffer_(o, slice_t + t) = in(o, t);
    }
  }
}
