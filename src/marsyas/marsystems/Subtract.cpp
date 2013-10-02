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

#include "Subtract.h"
#include "../common_source.h"


using namespace std;
using namespace Marsyas;

Subtract::Subtract(mrs_string name):MarSystem("Subtract", name)
{
  addControls();
}

Subtract::Subtract(const Subtract& a) : MarSystem(a)
{

}


Subtract::~Subtract()
{
}

MarSystem*
Subtract::clone() const
{
  return new Subtract(*this);
}

void
Subtract::addControls()
{

}

void
Subtract::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)
  MRSDIAG("Subtract.cpp - Subtract:myUpdate");

  ctrl_onSamples_->setValue(ctrl_inSamples_, NOUPDATE);
  ctrl_onObservations_->setValue(1, NOUPDATE);
  ctrl_osrate_->setValue(ctrl_israte_, NOUPDATE);
  ctrl_onObsNames_->setValue(ctrl_inObsNames_, NOUPDATE);
}

void
Subtract::myProcess(realvec& in, realvec& out)
{
  for (mrs_natural t = 0; t < inSamples_; t++)
    out(0,t) = in(0,t) - in(1,t);
}
