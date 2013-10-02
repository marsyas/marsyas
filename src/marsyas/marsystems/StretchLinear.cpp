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

#include "StretchLinear.h"
#include "../common_source.h"

using namespace std;
using namespace Marsyas;

StretchLinear::StretchLinear(mrs_string name):MarSystem("StretchLinear", name)
{
  addControls();
}

StretchLinear::StretchLinear(const StretchLinear& a) : MarSystem(a)
{
  ctrl_stretch_ = getctrl("mrs_real/stretch");
}


StretchLinear::~StretchLinear()
{
}

MarSystem*
StretchLinear::clone() const
{
  return new StretchLinear(*this);
}

void
StretchLinear::addControls()
{
  addctrl("mrs_real/stretch", 1.0, ctrl_stretch_);
}

void
StretchLinear::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)
  MRSDIAG("StretchLinear.cpp - StretchLinear:myUpdate");
  mrs_real alpha = ctrl_stretch_->to<mrs_real>();
  ctrl_onSamples_->setValue((mrs_natural) (alpha * ctrl_inSamples_->to<mrs_natural>()), NOUPDATE);
  ctrl_onObservations_->setValue(ctrl_inObservations_->to<mrs_natural>());
  ctrl_osrate_->setValue(ctrl_israte_->to<mrs_real>());
}

void
StretchLinear::myProcess(realvec& in, realvec& out)
{
  mrs_natural t,o;
  mrs_real tp;
  mrs_natural tl, tr;
  mrs_real alpha = ctrl_stretch_->to<mrs_real>();


  for (o=0; o < onObservations_; o++)
  {
    for (t = 0; t < onSamples_; t++)
    {
      tp = t / alpha;
      tl= (mrs_natural)tp;
      tr = tl + 1;
      if (tl<inSamples_)
      {
        out(o,t) = (tr-tp) * in(o,tl) + (tp-tl) * in(o,tr);
      }
      else // reflect on boundary
      {
        out(o,t) = in(o,inSamples_);
      }
    }
  }
}
