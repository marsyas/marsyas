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

#include "DeltaFirstOrderRegression.h"

using namespace std;
using namespace Marsyas;

DeltaFirstOrderRegression::DeltaFirstOrderRegression(mrs_string name) :
  MarSystem("DeltaFirstOrderRegression", name)
{
  /// Add any specific controls needed by this MarSystem.
  addControls();
}

DeltaFirstOrderRegression::DeltaFirstOrderRegression(
  const DeltaFirstOrderRegression& a) :
  MarSystem(a)
{
}

DeltaFirstOrderRegression::~DeltaFirstOrderRegression()
{
}

MarSystem*
DeltaFirstOrderRegression::clone() const
{
  return new DeltaFirstOrderRegression(*this);
}

void DeltaFirstOrderRegression::addControls()
{
}

void DeltaFirstOrderRegression::myUpdate(MarControlPtr sender)
{
  /// Use the default MarSystem setup with equal input/output stream format.
  MarSystem::myUpdate(sender);
  // prefix obsnames with "DeltaR1"
  mrs_string inObsNames = ctrl_inObsNames_->to<mrs_string> ();
  mrs_string onObsNames = obsNamesAddPrefix(inObsNames, "DeltaR1_");
  ctrl_onObsNames_->setValue(onObsNames, NOUPDATE);

  // Allocate and initialize the buffers.
  this->memory_.stretch(inObservations_, 2);
  this->memory_.setval(0.0);
}

void DeltaFirstOrderRegression::myProcess(realvec& in, realvec& out)
{
  /// Iterate over the observations and samples and do the processing.
  for (mrs_natural o = 0; o < inObservations_; o++)
  {
    // Calculate delta.
    out(o, 0) = 0.5 * (in(o, 0) - memory_(o, 0));
    if (inSamples_ > 1)
    {
      out(o, 1) = 0.5 * (in(o, 1) - memory_(o, 1));
      for (mrs_natural t = 2; t < inSamples_; t++)
      {
        out(o, t) = 0.5 * (in(o, t) - in(o, t - 2));
      }
    }
    // Update memory
    memory_(o, 0) = (inSamples_ >= 2 ? in(o, inSamples_ - 2)
                     : memory_(o, 1));
    memory_(o, 1) = in(o, inSamples_ - 1);
  }
}
