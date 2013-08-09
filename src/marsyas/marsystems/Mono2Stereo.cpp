/*
** Copyright (C) 1998-2007 George Tzanetakis <gtzan@cs.uvic.ca>
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


#include "Mono2Stereo.h"

using std::ostringstream;
using namespace Marsyas;

Mono2Stereo::Mono2Stereo(mrs_string name):MarSystem("Mono2Stereo", name)
{
  //Add any specific controls needed by Mono2Stereo
  //(default controls all MarSystems should have
  //were already added by MarSystem::addControl(),
  //called by :MarSystem(name) constructor).
  //If no specific controls are needed by a MarSystem
  //there is no need to implement and call this addControl()
  //method (see for e.g. Rms.cpp)
  addControls();
}

Mono2Stereo::Mono2Stereo(const Mono2Stereo& a) : MarSystem(a)
{
}

Mono2Stereo::~Mono2Stereo()
{
}

MarSystem*
Mono2Stereo::clone() const
{
  return new Mono2Stereo(*this);
}

void
Mono2Stereo::addControls()
{
}

void
Mono2Stereo::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)
  setctrl("mrs_natural/onSamples", getctrl("mrs_natural/onSamples"));

  mrs_natural inObservations = getctrl("mrs_natural/inObservations")->to<mrs_natural>();

  if (inObservations == 1)
  {
    setctrl("mrs_natural/onObservations", 2);
    mrs_string inObsNames = getctrl("mrs_string/inObsNames")->to<mrs_string>();
    inObsNames += ",";
    inObsNames += inObsNames;
    setctrl("mrs_string/onObsNames", inObsNames);
  }
  else
  {
    setctrl("mrs_natural/onObservations", inObservations);
    setctrl("mrs_string/onObsNames", getctrl("mrs_string/inObsNames"));
  }
  setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));




}


void
Mono2Stereo::myProcess(realvec& in, realvec& out)
{
  mrs_natural t,o;
  if (inObservations_ != 1) 	// not mono - just pass through
  {
    for (o=0; o < inObservations_; o++)
    {
      for (t = 0; t < inSamples_; t++)
      {
        out(o,t) = in(o,t);
      }
    }
  }

  else // mono to stereo
  {
    for (o=0; o < onObservations_; o++)
      for (t = 0; t < inSamples_; t++)
      {
        out(o,t) = in(0,t);
      }

  }


}








