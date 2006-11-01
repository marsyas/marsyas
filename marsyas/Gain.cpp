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
#ifdef _MATLAB_ENGINE_
#include "MATLABengine.h"
#endif 
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
  addctrl("mrs_real/gain", 1.0);
}

// void
// Gain::localUpdate()
// {
//   
// lmartins: since this is the default MarSystem::localUpdate()
// (i.e. does not alters input data format) it's not needed to
// override it here!
// see also Limiter.cpp for another example
//   
//   MRSDIAG("Gain.cpp - Gain:localUpdate");
//   
//   setctrl("mrs_natural/onSamples", getctrl("mrs_natural/inSamples"));
//   setctrl("mrs_natural/onObservations", getctrl("mrs_natural/inObservations"));
//   setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));
//   setctrl("mrs_string/onObsNames", getctrl("mrs_string/inObsNames"));
//   
// 	//defaultUpdate(); no longer needed here. Done at MarSystem::update()  
//}

void 
Gain::process(realvec& in, realvec& out)
{
  checkFlow(in,out);

  mrs_real gain = getctrl("mrs_real/gain").toReal();
  
  for (o=0; o < inObservations_; o++)
    for (t = 0; t < inSamples_; t++)
    {
			out(o,t) =  gain * in(o,t);
    }

}







	
