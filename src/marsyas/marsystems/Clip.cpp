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

#include "Clip.h"

using std::ostringstream;
using namespace Marsyas;

Clip::Clip(mrs_string name):MarSystem("Clip",name)
{
  addControls();
}


Clip::~Clip()
{
}


Clip::Clip(const Clip& a) : MarSystem(a)
{
  ctrl_range_ = getctrl("mrs_real/range");
}




MarSystem*
Clip::clone() const
{
  return new Clip(*this);
}

void
Clip::addControls()
{
  //Add specific controls needed by this MarSystem.
  addctrl("mrs_real/range", 1.0, ctrl_range_);
}


void
Clip::myUpdate(MarControlPtr sender)
{
  MarSystem::myUpdate(sender);
}

void
Clip::myProcess(realvec& in, realvec& out)
{
  mrs_natural o,t;
  mrs_real range = ctrl_range_->to<mrs_real>();

  for (o=0; o < inObservations_; o++)
  {
    for (t = 0; t < inSamples_; t++)
    {
      if (in(o,t) > range)
        out(o,t) = range;
      else if (in(o,t) < -range)
        out(o,t) = -range;
      else
        out(o,t) = in(o,t);

    }
  }
}










