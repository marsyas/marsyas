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

#include "MarSystemTemplateMedium.h"
#include "../common_source.h"

using std::ostringstream;
using namespace Marsyas;

MarSystemTemplateMedium::MarSystemTemplateMedium(mrs_string name):MarSystem("MarSystemTemplateMedium", name)
{
  // Add any specific controls needed by this MarSystem
  // (default controls all MarSystems should have
  // were already added by MarSystem::addControl(),
  // called by :MarSystem(name) constructor).
  // If no specific controls are needed by a MarSystem
  // there is no need to implement and call this addControl()
  // method (see for e.g. Rms.cpp)
  addControls();
}

MarSystemTemplateMedium::MarSystemTemplateMedium(const MarSystemTemplateMedium& a) : MarSystem(a)
{
  // IMPORTANT!
  // All member "pointers" to controls have to be
  // explicitly reassigned here, at the copy ctor.
  // Otherwise these member "pointers" would be invalid!
  ctrl_repeats_EXAMPLE_ = getctrl("mrs_natural/repeats");
  ctrl_gain_EXAMPLE_ = getctrl("mrs_real/gain");
}


MarSystemTemplateMedium::~MarSystemTemplateMedium()
{
}

MarSystem*
MarSystemTemplateMedium::clone() const
{
  return new MarSystemTemplateMedium(*this);
}

void
MarSystemTemplateMedium::addControls()
{
  // Add any specific controls needed by this MarSystem.

  // Let's start by adding a dummy control (to which we will not use
  // a "pointer", just to illustrate the "traditional", yet
  // not so efficient way of using controls)
  addctrl("mrs_bool/dummy", false);
  // in this case this control should have state, since
  // other controls will depend on it.
  // (i.e. any change to it will call MarSystem::update()
  // which then calls myUpdate(MarControlPtr sender))
  setctrlState("mrs_bool/dummy", true);

  // if a "pointer" to a control is to be used (for efficiency purposes
  // - see myProcess() bellow), it should be passed as the last
  // argument to addctrl()
  addctrl("mrs_real/gain", 1.0, ctrl_gain_EXAMPLE_);
  // IMPORTANT NOTE:
  // in the above case, since the control value is supposed to
  // be a mrs_real, the default value also has to be a mrs_real!
  // if not (e.g. initialiting with "1" instead of "1.0"), the
  // control will in fact have a mrs_natural value despite of
  // the "mrs_real/..." name.

  addctrl("mrs_natural/repeats", 1, ctrl_repeats_EXAMPLE_);
  // if we have a "pointer" to a control, we can set its state
  // in a different and more efficient way
  ctrl_repeats_EXAMPLE_->setState(true);
}

void
MarSystemTemplateMedium::myUpdate(MarControlPtr sender)
{
  MRSDIAG("MarSystemTemplateMedium.cpp - MarSystemTemplateMedium:myUpdate");


  MarSystem::myUpdate(sender);
  // As an example, this MarSystem will output a nr of repeated input frames
  //
  // The traditional (and still valid), but inefficient way to set controls:
  //
  //setctrl("mrs_natural/onObservations", getctrl("mrs_natural/inObservations"));
  //setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));
  //setctrl("mrs_string/onObsNames", getctrl("mrs_string/inObsNames"));
  //...
  //
  // more efficient way to set the value of a control
  // (if we have a "pointer" to it - see addControls(), above).
  // setValue() always calls update() if the control in cause has state.
  // To avoid calling update() (and avoid recursion since we are already
  // inside update()/myUpdate(MarControlPtr sender)), setValue() should be called with the
  // NOUPDATE flag, as bellow:
  /*
  	ctrl_onObservations_->setValue(ctrl_inObservations_, NOUPDATE);
  	ctrl_osrate_->setValue(ctrl_israte_, NOUPDATE);
  	ctrl_onObsNames_->setValue(ctrl_inObsNames_, NOUPDATE);
  */
  // here it's possible to see how the "repeats" control
  // affects the onSamples control (i.e. output number of samples)
  //
  // IMPORTANT: Since this changes the onSamples control, if this
  // MarSystem is inside a Composite, the Composite::update() must be
  // called afterwards in the main code, otherwise the size of the
  // corresponding slice will not be updated accordingly!
  // (see marsyasTests.cpp >> test_MarControls() )
  //
  ctrl_onSamples_->setValue(ctrl_inSamples_ * ctrl_repeats_EXAMPLE_, NOUPDATE);

  // NOTE:
  // see Gain.cpp for some more info about output parameter configuration.
}

void
MarSystemTemplateMedium::myProcess(realvec& in, realvec& out)
{
  mrs_natural t,o;
  // FIXME Variable (rep) appears to be unused.
  // static mrs_natural rep = 1;
  static realvec vec;

  const mrs_real& gainValue = ctrl_gain_EXAMPLE_->to<mrs_real>();
  // this is equivalent (although slightly more efficient) to:
  //
  // mrs_real& gainValue = ctrl_gain_EXAMPLE_->to<mrs_real>(); // ::toReal() calls ::to<mrs_real>()
  //
  // This reference will not allow writing directly to the control, but
  // avoids a copy (which can hurt if the control is a big realvec)
  // and even if by some means the control value is modified elsewhere (e.g. by a
  // different thread), it's always in sync with the actual control value.

  // There may be cases where is more adequate to get a copy of the control value,
  // so it does not change event if the actual control value is changed elsewhere:
  mrs_natural repeats = ctrl_repeats_EXAMPLE_->to<mrs_natural>();

  for (o=0; o < inObservations_; o++)
    for (t = 0; t < inSamples_; t++)
      for(mrs_natural r= 0; r < repeats; ++r)
        out(o,t+r*inSamples_) = gainValue * in(o,t);

  // Just as an example, let's assume we want to write to a control
  // (e.g. some weird amplitude modulation effect), this is the way
  // to do it:
  mrs_real g = (((mrs_natural)(gainValue*10)+1)%10)/10.0;
  ctrl_gain_EXAMPLE_->setValue(g);
}
