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

#include "Daub4.h"
#include "../common_source.h"

using std::ostringstream;
using namespace Marsyas;

Daub4::Daub4(mrs_string name):MarSystem("Daub4",name)
{
  addControls();
}

Daub4::~Daub4()
{
}

MarSystem*
Daub4::clone() const
{
  return new Daub4(*this);
}

void
Daub4::addControls()
{
  addctrl("mrs_bool/forward", true, ctrl_forward_);
  addctrl("mrs_natural/processSize", 0, ctrl_processSize_);
}

void
Daub4::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)
  MRSDIAG("Daub4.cpp - Daub4:myUpdate");

  setctrl("mrs_natural/onSamples", getctrl("mrs_natural/inSamples"));
  setctrl("mrs_natural/onObservations", getctrl("mrs_natural/inObservations"));
  setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));
  c0_ = 0.4829629131445341f;
  c1_ = 0.8365163037378079f;
  c2_ = 0.2241438680420143f;
  c3_ = -0.1294095225512604f;
  workspace_.create(getctrl("mrs_natural/inSamples")->to<mrs_natural>());
}

void
Daub4::myProcess(realvec& in, realvec& out)
{
  //checkFlow(in,out);
  mrs_natural t;
  const mrs_natural& n = ctrl_processSize_->to<mrs_natural>();
  const mrs_bool& forward = ctrl_forward_->to<mrs_bool>();

  if (n < 4) return;
  nh = n >> 1;
  nh1 = nh + 1;

  // Apply Filter
  if (forward)
  {
    for (i=0, j=0; j <= n-4; j+=2, ++i)
    {
      workspace_(i) = c0_ * in(0,j) + c1_ * in(0,j+1) +
                      c2_ * in(0,j+2) + c3_ * in(0,j+3);
      workspace_(i+nh) = c3_ * in(0,j) - c2_ * in(0,j+1) +
                         c1_ * in(0,j+2) - c0_ * in(0,j+3);
    }
    workspace_(i) = c0_ * in(0,n-2) + c1_ * in(0,n-1) +
                    c2_ * in(0,0) + c3_ * in(0,1);
    workspace_(i+nh) = c3_ * in(0,n-2) - c2_ * in(0,n-1) +
                       c1_ * in(0,0) - c0_ * in(0,1);
  }
  // Apply transpose filter
  else if (!forward)
  {
    workspace_(0) =  c2_ * in(0,nh-1) + c1_ * in(0,n-1) +
                     c0_ * in(0,0) + c3_ * in(0,nh1-1);
    workspace_(1) = c3_ * in(0,nh-1) - c0_ * in(0,n-1) +
                    c1_ * in(0,0) - c2_ * in(0,nh1-1);
    for (i=0, j=2; i < nh-1; ++i)
    {
      workspace_(j++) = c2_ * in(0,i) + c1_ * in(0,i+nh) +
                        c0_ * in(0,i+1) + c3_ * in(0,i+nh1);
      workspace_(j++) = c3_ * in(0,i) - c0_ * in(0,i+nh) +
                        c1_ * in(0,i+1) - c2_ * in(0,i+nh1);
    }
  }

  for (t=0; t < n; t++)
  {
    out(0,t) = workspace_(t);
  }
}




