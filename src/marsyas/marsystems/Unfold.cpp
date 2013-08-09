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

#include "Unfold.h"

using namespace std;
using namespace Marsyas;

Unfold::Unfold(mrs_string name):MarSystem("Unfold",name)
{
  addControls();
}

Unfold::Unfold(const Unfold& a): MarSystem(a)
{
}



Unfold::~Unfold()
{
}

void
Unfold::addControls()
{
}

MarSystem*
Unfold::clone() const
{
  return new Unfold(*this);
}

void
Unfold::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)
  ctrl_onObservations_->setValue(ctrl_inSamples_->to<mrs_natural>() * ctrl_inObservations_->to<mrs_natural>(), NOUPDATE);
  ctrl_onSamples_->setValue(1, NOUPDATE);
  ctrl_osrate_->setValue(ctrl_israte_, NOUPDATE);
  ctrl_onObsNames_->setValue("Update_" + ctrl_inObsNames_->to<mrs_string>() , NOUPDATE);
}


void
Unfold::myProcess(realvec& in, realvec& out)
{
  mrs_natural o,t;

  for (o = 0; o < inObservations_; o++) {
    for (t = 0; t < inSamples_; t++) {
      int pos = (o*inSamples_)+t;
      out(pos,0) = in(o,t);
    }
  }

}
