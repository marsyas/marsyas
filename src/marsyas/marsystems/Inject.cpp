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

#include "Inject.h"

using std::ostringstream;
using namespace Marsyas;

Inject::Inject(mrs_string name) : MarSystem("Inject", name)
{
  /// Add any specific controls needed by this MarSystem.
  addControls();
  prev_injectSize_ = 0;
}

Inject::Inject(const Inject& a) : MarSystem(a)
{
  ctrl_inject_ = getctrl("mrs_realvec/inject");
  ctrl_injectSize_ = getctrl("mrs_natural/injectSize");
  ctrl_injectNames_ = getctrl("mrs_string/injectNames");
  prev_injectSize_ = 0;
}



Inject::~Inject()
{
}

MarSystem*
Inject::clone() const
{
  return new Inject(*this);
}

void
Inject::addControls()
{
  addctrl("mrs_realvec/inject", realvec(), ctrl_inject_);
  addctrl("mrs_natural/injectSize", 1, ctrl_injectSize_);
  setctrlState("mrs_natural/injectSize", true);
  addctrl("mrs_string/injectNames", "t1,t2,t3,t4,t5,t6,t7,t8", ctrl_injectNames_);
}



void
Inject::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)
  ctrl_onSamples_->setValue(ctrl_inSamples_, NOUPDATE);
  ctrl_onObservations_->setValue(ctrl_inObservations_->to<mrs_natural>() + ctrl_injectSize_->to<mrs_natural>(), NOUPDATE);
  ctrl_osrate_->setValue(ctrl_israte_->to<mrs_real>(), NOUPDATE);
  mrs_string onObsNames = ctrl_inObsNames_->to<mrs_string>();
  mrs_string injectNames = ctrl_injectNames_->to<mrs_string>();
  ctrl_onObsNames_->setValue(onObsNames + injectNames, NOUPDATE);


  mrs_natural injectSize = ctrl_injectSize_->to<mrs_natural>();

  if (prev_injectSize_ != injectSize)
  {
    MarControlAccessor acc(ctrl_inject_);
    mrs_realvec& inject = acc.to<mrs_realvec>();


    inject.stretch(injectSize);
  }




  prev_injectSize_ = injectSize;

}

void
Inject::myProcess(realvec& in, realvec& out)
{
  mrs_natural t,o;
  MarControlAccessor acc(ctrl_inject_);
  mrs_realvec& inject = acc.to<mrs_realvec>();

  /// Iterate over the observations and samples and do the processing.
  for (o = 0; o < inObservations_; o++)
  {
    for (t = 0; t < inSamples_; t++)
    {

      out(o, t) = in(o, t);
    }
  }



  for (o = inObservations_; o < onObservations_; o++)
  {
    for (t = 0; t < inSamples_; t++)
    {
      out(o, t) = inject(o-inObservations_);
    }
  }

}
