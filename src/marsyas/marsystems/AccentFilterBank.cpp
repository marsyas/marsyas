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

#include "AccentFilterBank.h"
#include "../common_source.h"

using std::ostringstream;
using namespace Marsyas;

AccentFilterBank::AccentFilterBank(mrs_string name):MarSystem("AccentFilterBank", name)
{
  addControls();
}

AccentFilterBank::AccentFilterBank(const AccentFilterBank& a) : MarSystem(a)
{
  ctrl_gain_EXAMPLE_ = getctrl("mrs_real/gain");
}


AccentFilterBank::~AccentFilterBank()
{
}

MarSystem*
AccentFilterBank::clone() const
{
  return new AccentFilterBank(*this);
}

void
AccentFilterBank::addControls()
{

  addctrl("mrs_bool/dummyEXAMPLE", false);
  setctrlState("mrs_bool/dummyEXAMPLE", true);

  addctrl("mrs_real/gain", 1.0, ctrl_gain_EXAMPLE_);

}

void
AccentFilterBank::myUpdate(MarControlPtr sender)
{
  MRSDIAG("AccentFilterBank.cpp - AccentFilterBank:myUpdate");

  MarSystem::myUpdate(sender);
}

void
AccentFilterBank::myProcess(realvec& in, realvec& out)
{
  mrs_natural o,t;
  const mrs_real& gainValueEXAMPLE = ctrl_gain_EXAMPLE_->to<mrs_real>();

  for (o=0; o < inObservations_; o++)
    for (t = 0; t < inSamples_; t++)
      out(o,t) = gainValueEXAMPLE * in(o,t);
}
