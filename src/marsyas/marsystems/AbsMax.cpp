/*
** Copyright (C) 1998-2010 George Tzanetakis <gtzan@cs.uvic.ca>
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


#include "AbsMax.h"

using std::ostringstream;
using namespace Marsyas;

AbsMax::AbsMax(mrs_string name): MarSystem("AbsMax",name)
{
}

AbsMax::~AbsMax()
{
}

MarSystem*
AbsMax::clone() const
{
  return new AbsMax(*this);
}

void
AbsMax::myUpdate(MarControlPtr sender)
{
  // Start with the default MarSystem setup with equal input/output
  // stream format ...
  MarSystem::myUpdate(sender);

  // ... but change the number and rate of output samples.
  setControl("mrs_natural/onSamples",  (mrs_natural)1);
  setControl("mrs_real/osrate",
             getControl("mrs_real/israte")->to<mrs_real>() / getControl("mrs_natural/inSamples")->to<mrs_natural>()
            );

  mrs_string inObsNames = ctrl_inObsNames_->to<mrs_string>();
  ctrl_onObsNames_->setValue(obsNamesAddPrefix(inObsNames, "AbsMax_"), NOUPDATE);
}

void
AbsMax::myProcess(realvec& in, realvec& out)
{
  mrs_natural o,t;
  for (o=0; o < inObservations_; ++o)
  {
    max_ = 0.0;
    for (t=0; t < inSamples_; ++t)
    {
      if (fabs(in(o,t)) > max_)
      {
        max_ = fabs(in(o,t));
      }
    }
    out(o,0) = max_;
  }
}
