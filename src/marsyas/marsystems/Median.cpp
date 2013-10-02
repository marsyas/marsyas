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

#include "Median.h"
#include "../common_source.h"





using std::ostringstream;
using namespace Marsyas;

Median::Median(mrs_string name):MarSystem("Median", name)
{
  addControls();
}

Median::Median(const Median& a) : MarSystem(a)
{
}


Median::~Median()
{
}

MarSystem*
Median::clone() const
{
  return new Median(*this);
}

void
Median::addControls()
{
}

void
Median::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)
  MRSDIAG("Median.cpp - Median:myUpdate");

  ctrl_onSamples_->setValue((mrs_natural)1, NOUPDATE);
  ctrl_onObservations_->setValue(ctrl_inObservations_, NOUPDATE);
  ctrl_osrate_->setValue(ctrl_israte_, NOUPDATE);

  obsrow_.create(ctrl_inSamples_->to<mrs_natural>());

  //defaultUpdate(); [!]
  inObservations_ = ctrl_inObservations_->to<mrs_natural>();

  ostringstream oss;
  mrs_string inObsNames = ctrl_inObsNames_->to<mrs_string>();
  for (int i = 0; i < inObservations_; ++i)
  {
    mrs_string inObsName;
    mrs_string temp;
    inObsName = inObsNames.substr(0, inObsNames.find(","));
    temp = inObsNames.substr(inObsNames.find(",")+1, inObsNames.length());
    inObsNames = temp;
    oss << "Median" << "_" << inObsName << ",";
  }
  ctrl_onObsNames_->setValue(oss.str(),NOUPDATE);
}

void
Median::myProcess(realvec& in, realvec& out)
{
  mrs_natural t,o;
  out.setval(0.0);
  for (o=0; o < inObservations_; o++) {
    for (t = 0; t < inSamples_; t++) {
      obsrow_(t) = in(o,t);
    }
    out(o,0) = obsrow_.median();
  }

}
