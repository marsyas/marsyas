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

#include "../common_source.h"
#include "MarSystemTemplateBasic.h"

using std::ostringstream;
using namespace Marsyas;

MarSystemTemplateBasic::MarSystemTemplateBasic(mrs_string name) : MarSystem("MarSystemTemplateBasic", name)
{
  /// Add any specific controls needed by this MarSystem.
  // Default controls that all MarSystems should have (like "inSamples"
  // and "onObservations"), are already added by MarSystem::addControl(),
  // which is already called by the constructor MarSystem::MarSystem(name).
  // If no specific controls are needed by a MarSystem there is no need to
  // implement and call this addControl() method (see for e.g. Rms.cpp)
  addControls();
}

MarSystemTemplateBasic::MarSystemTemplateBasic(const MarSystemTemplateBasic& a) : MarSystem(a)
{
  // IMPORTANT!
  /// All member MarControlPtr have to be explicitly reassigned in
  /// the copy constructor.
  // Otherwise this would result in trying to deallocate them twice!
  ctrl_gain_EXAMPLE_ = getctrl("mrs_real/gain");
}


MarSystemTemplateBasic::~MarSystemTemplateBasic()
{
}

MarSystem*
MarSystemTemplateBasic::clone() const
{
  // Every MarSystem should do this.
  return new MarSystemTemplateBasic(*this);
}

void
MarSystemTemplateBasic::addControls()
{
  /// Add any specific controls needed by this MarSystem.

  // Let's start by adding a dummy control (for which we
  // will not use a pointer, just to illustrate the
  // "traditional", yet not so efficient way of using
  // controls)
  addctrl("mrs_bool/dummyEXAMPLE", false);
  // In this case this control should have state, since
  // other controls will depend on it.  (i.e. any change
  // to it will call MarSystem::update() which then calls
  // myUpdate(MarControlPtr sender))
  setctrlState("mrs_bool/dummyEXAMPLE", true);

  // If a pointer to a control is to be used (for
  // efficiency purposes - see myProcess() bellow), it
  // should be passed as the last argument to addctrl()
  addctrl("mrs_real/gain", 1.0, ctrl_gain_EXAMPLE_);

  // IMPORTANT NOTE:
  // in the above case, since the control value is
  // supposed to be a mrs_real, the default value also has
  // to be a mrs_real!  if not (e.g. initializing with "1"
  // instead of "1.0"), the control will in fact have a
  // mrs_natural value despite of the "mrs_real/..." name.
}

void
MarSystemTemplateBasic::myUpdate(MarControlPtr sender)
{
  MRSDIAG("MarSystemTemplateBasic.cpp - MarSystemTemplateBasic:myUpdate");

  /// Use the default MarSystem setup with equal input/output stream format.
  MarSystem::myUpdate(sender);
}

void
MarSystemTemplateBasic::myProcess(realvec& in, realvec& out)
{
  mrs_natural t,o;
  const mrs_real& gainValueEXAMPLE = ctrl_gain_EXAMPLE_->to<mrs_real>();
  // This is equivalent (although slightly more efficient) than:
  //
  // mrs_real& gainValue = ctrl_gain_EXAMPLE_->to<mrs_real>();
  //   // ::toReal() calls ::to<mrs_real>()
  //
  // This reference will not allow writing directly to the
  // control, but avoids a copy (which can hurt if the
  // control is a big realvec) and even if by some means
  // the control value is modified elsewhere (e.g. by a
  // different thread), it's always in sync with the
  // actual control value.

  /// Iterate over the observations and samples and do the processing.
  for (o = 0; o < inObservations_; o++)
  {
    for (t = 0; t < inSamples_; t++)
    {
      out(o, t) = gainValueEXAMPLE * in(o, t);
    }
  }
}
