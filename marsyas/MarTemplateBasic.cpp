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
\class MarTemplateBasic
\brief Multiply input realvec with MarTemplateBasic

Simple MarSystem example. Just multiply the values of the input realvec
with MarTemplateBasic and put them in the output vector. This object can be used 
as a prototype template for building more complicated MarSystems. 
*/

#include "MarTemplateBasic.h"

using namespace std;
using namespace Marsyas;

MarTemplateBasic::MarTemplateBasic(string name):MarSystem("MarTemplateBasic", name)
{
	//Add any specific controls needed by this MarSystem
	//(default controls all MarSystems should have
	//were already added by MarSystem::addControl(), 
	//called by :MarSystem(name) constructor).
	//If no specific controls are needed by a MarSystem
	//there is no need to implement and call this addControl()
	//method (see for e.g. Rms.cpp)
	addControls();
}

MarTemplateBasic::MarTemplateBasic(const MarTemplateBasic& a) : MarSystem(a)
{
	//IMPORTANT!
	//all member "pointers" to controls have to be
	//explicitly reassigned here, at the copy ctor.
	//Otherwise these member "pointers" would be invalid!
	ctrl_myCtrl1_ = getctrl("mrs_real/myCtrl1");
	ctrl_myCtrl2_ = getctrl("mrs_real/myCtrl2");
}


MarTemplateBasic::~MarTemplateBasic()
{
}

MarSystem* 
MarTemplateBasic::clone() const 
{
	return new MarTemplateBasic(*this);
}

void 
MarTemplateBasic::addControls()
{
	//Add any specific controls needed by this MarSystem.
	
	addControl("mrs_natural/myCtrl0", 1);
	//if needed, the control can have state
	//(i.e. any change to it will call MarSystem::update()
	//which then calls myUpdate())
	setctrlState("mrs_natural/myCtrl0", true);

	//if a "pointer" to a control is needed (for efficiency purposes 
	//- see myProcess() bellow), it should be passed as the last
	//argument to addctrl()
	addctrl("mrs_real/myCtrl1", 1.0, ctrl_myCtrl1_);
	//WARNING!! [!!]
	//in the above case, since the control value is supposed to 
	//be a mrs_real, the default value also has to be a mrs_real!
	//if not (e.g. initialiting with "1" instead of "1.0"), the
	//control will in fact have a mrs_natural value despite of
	//the "mrs_real/xxx" string... => FIX THIS!!

	addctrl("mrs_bool/myCtrl2", false, ctrl_myCtrl2_);
	//if we have a "pointer" to a control, we can set its state
	//in a different and more efficient way
	ctrl_myCtrl2_->setState(true);
}

void
MarTemplateBasic::myUpdate()
{
// lmartins: since this is the default MarSystem::myUpdate()
// (i.e. does not alters input data format) it's not needed to
// override it here!
// see also Limiter.cpp for another example
  
  MRSDIAG("MarTemplateBasic.cpp - MarTemplateBasic:myUpdate");
  
  //still valid but inefficcient way to set controls
	setctrl("mrs_natural/onSamples", getctrl("mrs_natural/inSamples"));
  setctrl("mrs_natural/onObservations", getctrl("mrs_natural/inObservations"));
  setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));
  setctrl("mrs_string/onObsNames", getctrl("mrs_string/inObsNames"));

	//more efficient way to set the value of a control 
	//(only if we have a "pointer to it" - see addControls(), above)
	ctrl_myCtrl1_->setValue(ctrl_inSamples_, NOUPDATE);

 
}

void 
MarTemplateBasic::myProcess(realvec& in, realvec& out)
{
	//get a local copy of the current MarTemplateBasic control value
	//(it will be used for this entire processing, even if it's
	//changed by someone else, e.g. by a different thread)

	for (o=0; o < inObservations_; o++)
		for (t = 0; t < inSamples_; t++)
		{
			out(o,t) = in(o,t);
		}
}








