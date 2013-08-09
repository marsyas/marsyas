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


#include "MaxMin.h"
using std::ostringstream;
using namespace Marsyas;

MaxMin::MaxMin(mrs_string name): MarSystem("MaxMin",name)
{
}

MaxMin::~MaxMin()
{
}

MarSystem*
MaxMin::clone() const
{
  return new MaxMin(*this);
}

void
MaxMin::myUpdate(MarControlPtr sender)
{
  // Start with the default MarSystem setup with equal input/output
  // stream format ...
  MarSystem::myUpdate(sender);

  // ... but change the number and rate of output samples.
  setControl("mrs_natural/onSamples",  (mrs_natural)2);
  setControl("mrs_real/osrate",
             getControl("mrs_real/israte")->to<mrs_real>() / getControl("mrs_natural/inSamples")->to<mrs_natural>()
            );
}

void
MaxMin::myProcess(realvec& in, realvec& out)
{
  mrs_natural t,o;
  for (o=0; o < inObservations_; o++)
  {
    max_ = -1.0 * DBL_MAX;
    min_ = DBL_MAX;
    for (t=0; t < inSamples_; t++)
    {
      if (in(o,t) > max_)
      {
        max_ = in(o,t);
      }
      if (in(o,t) < min_)
      {
        min_ = in(o,t);
      }
    }
    out(o,0) = max_;
    out(o,1) = min_;
  }
}
