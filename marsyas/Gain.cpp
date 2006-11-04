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
    \class Gain
    \brief Multiply input realvec with gain

   Simple MarSystem example. Just multiply the values of the input realvec
with gain and put them in the output vector. This object can be used 
as a prototype template for building more complicated MarSystems. 
*/

#include "Gain.h"

using namespace std;
using namespace Marsyas;

Gain::Gain(string name):MarSystem("Gain", name)
{
  //The type_ and name_ values are now passed to the
	//base classes were they are set and used when adding 
	//the corresponding controls (otherwise, it would create
	//problems with the control paths not being all equal for
	//a single MarSystem...).
	//
	//type_ = "Gain";
  //name_ = name;
  
	//Add any specific controls needed by Gain
	//(default controls all MarSystems should have
	//were already added by MarSystem::addControl(), 
	//called by :MarSystem(name) constructor).
	//If no specific controls are needed by a MarSystem
	//there is no need to implement and call this addControl()
	//method (see for e.g. Rms.cpp)
	addControls();
}

Gain::Gain(const Gain& a) : MarSystem(a)
{
	ctrl_gain_ = getctrl("mrs_real/gain");
}

Gain::~Gain()
{
}

MarSystem* 
Gain::clone() const 
{
  return new Gain(*this);
}

void 
Gain::addControls()
{
  //Add specific controls needed by this MarSystem.
  addctrl("mrs_real/gain", 1.0, ctrl_gain_);
}

// void
// Gain::myUpdate()
// {
//   MRSDIAG("Gain.cpp - Gain:myUpdate");
//   
// 	// "Traditional" way of setting control values (not the most efficient)
// 	//
// 	//setctrl("mrs_natural/onSamples", getctrl("mrs_natural/inSamples"));
// 	//setctrl("mrs_natural/onObservations", getctrl("mrs_natural/inObservations"));
// 	//setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));
// 	//setctrl("mrs_string/onObsNames", getctrl("mrs_string/inObsNames"));
// 
// 	// More efficient way of setting control values
// 	//
// 	//ctrl_onObservations_->setValue(ctrl_inObservations_, NOUPDATE);
// 	//ctrl_osrate_->setValue(ctrl_israte_, NOUPDATE);
// 	//ctrl_onObsNames_->setValue(ctrl_inObsNames_, NOUPDATE);
// 	//ctrl_onSamples_->setValue(ctrl_inSamples_, NOUPDATE);
// 
// 	// However, since we are just setting the output parameters equal to
// 	// the input ones, its just easier and less error prone to call:
// 	//
// 	MarSystem::myUpdate();
// 
// 	// ...or even simpler, just do not override this ::myUpdate() method.
// 	// That way, MarSystem::myUpdate() will be called implicitly.
//}

void 
Gain::myProcess(realvec& in, realvec& out)
{
	//get a local copy of the current gain control value
	//(it will be used for this entire processing, even if it's
	//changed by someone else, e.g. by a different thread)
	mrs_real gainValue = ctrl_gain_->to<mrs_real>();

  for (o=0; o < inObservations_; o++)
    for (t = 0; t < inSamples_; t++)
    {
			out(o,t) = gainValue * in(o,t);
    }
}







	
