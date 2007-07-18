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

/**
	\class MarSystemTemplateBasic
	\ingroup Special
	\brief Basic example on how to use controls efficiently in MarSystems

	This example is the same as Gain; it scales the output by
multiplying each sample with a real number.

	Controls:
	- \b mrs_real/gain [w] : sets the gain multiplier.
	- \b mrs_bool/dummy [rw] : does nothing.
*/
#include "MarSystemTemplateBasic.h"

using namespace std;
using namespace Marsyas;

MarSystemTemplateBasic::MarSystemTemplateBasic(string name):MarSystem("MarSystemTemplateBasic", name)
{
	// Add any specific controls needed by this MarSystem
	// (default controls all MarSystems should have were
	// already added by MarSystem::addControl(), called by
	// :MarSystem(name) constructor).
	// If no specific controls are needed by a MarSystem
	// there is no need to implement and call this
	// addControl() method (see for e.g. Rms.cpp)
	addControls();
}

MarSystemTemplateBasic::MarSystemTemplateBasic(const MarSystemTemplateBasic& a) : MarSystem(a)
{
	// IMPORTANT!
	// All member pointers to controls have to be explicitly
	// reassigned here, at the copy constructor.
	// Otherwise these member pointers would be invalid,
	// which will result in trying to de-allocate them
	// twice!
	ctrl_gain_EXAMPLE_ = getctrl("mrs_real/gain");
}


MarSystemTemplateBasic::~MarSystemTemplateBasic()
{
}

MarSystem*
MarSystemTemplateBasic::clone() const
{
	// every MarSystem should do this
	return new MarSystemTemplateBasic(*this);
}

void
MarSystemTemplateBasic::addControls()
{
	// Add any specific controls needed by this MarSystem.

	// Let's start by adding a dummy control (for which we
	// will not use a pointer, just to illustrate the
	// "traditional", yet not so efficient way of using
	// controls)
	addctrl("mrs_bool/dummyEXAMPLE", false);
	// in this case this control should have state, since
	// other controls will depend on it.  (i.e. any change
	// to it will call MarSystem::update() which then calls
	// myUpdate(MarControlPtr sender))
	setctrlState("mrs_bool/dummyEXAMPLE", true);

	// if a pointer to a control is to be used (for
	// efficiency purposes - see myProcess() bellow), it
	// should be passed as the last argument to addctrl()
	addctrl("mrs_real/gain", 1.0, ctrl_gain_EXAMPLE_);

	// IMPORTANT NOTE:
	// in the above case, since the control value is
	// supposed to be a mrs_real, the default value also has
	// to be a mrs_real!  if not (e.g. initialiting with "1"
	// instead of "1.0"), the control will in fact have a
	// mrs_natural value despite of the "mrs_real/..." name.
}

void
MarSystemTemplateBasic::myUpdate(MarControlPtr sender)
{
	MRSDIAG("MarSystemTemplateBasic.cpp - MarSystemTemplateBasic:myUpdate");

	// this initializes the output (samples, observations,
	// sample rate, etc)
	MarSystem::myUpdate(sender);
}

void
MarSystemTemplateBasic::myProcess(realvec& in, realvec& out)
{
	const mrs_real& gainValueEXAMPLE = ctrl_gain_EXAMPLE_->to<mrs_real>();
	// this is equivalent (although slightly more efficient) than:
	//
	// mrs_real& gainValue = ctrl_gain_EXAMPLE_->toReal();
	//   // ::toReal() calls ::to<mrs_real>()
	//
	// This reference will not allow writing directly to the
	// control, but avoids a copy (which can hurt if the
	// control is a big realvec) and even if by some means
	// the control value is modified elsewhere (e.g. by a
	// different thread), it's always in sync with the
	// actual control value.

	for (o=0; o < inObservations_; o++)
		for (t = 0; t < inSamples_; t++)
			out(o,t) = gainValueEXAMPLE * in(o,t);
}


