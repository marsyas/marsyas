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

#include "TimeStretch.h"

using namespace std;
using namespace Marsyas;

TimeStretch::TimeStretch(mrs_string name):MarSystem("TimeStretch",name)
{
  //type_ = "TimeStretch";
  //name_ = name;

  addControls();
}

TimeStretch::~TimeStretch()
{
}

MarSystem*
TimeStretch::clone() const
{
  return new TimeStretch(*this);
}

void
TimeStretch::addControls()
{
  addctrl("mrs_real/factor", 1.0);
}

void
TimeStretch::myProcess(realvec& in, realvec& out)
{
  //checkFlow(in,out);
  mrs_real factor = getctrl("mrs_real/factor")->to<mrs_real>();
  mrs_natural inSamples = getctrl("mrs_natural/inSamples")->to<mrs_natural>();

  for (mrs_natural t=0; t < inSamples; t++)
  {
    // linear interpolation
    ni = t* factor;
    li = ((unsigned long)ni) % inSamples;
    ri = li + 1;

    w_ = ni - li;
    if (ri < inSamples)
      out(0,t) = in(li) + w_ * (in(0,ri) - in(0,li));
    else
      out(0,t) = in(li);
  }
}









