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


#include "Panorama.h"

using std::ostringstream;
using namespace Marsyas;

Panorama::Panorama(mrs_string name):MarSystem("Panorama", name)
{
  addControls();
}

Panorama::Panorama(const Panorama& a) : MarSystem(a)
{
  ctrl_angle_ = getctrl("mrs_real/angle");
}

Panorama::~Panorama()
{
}

MarSystem*
Panorama::clone() const
{
  return new Panorama(*this);
}

void
Panorama::addControls()
{
  addctrl("mrs_real/angle", PI/4, ctrl_angle_);
}

void
Panorama::myUpdate(MarControlPtr sender)
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
Panorama::myProcess(realvec& in, realvec& out)
{
  mrs_natural t,o;
  mrs_real angle = ctrl_angle_->to<mrs_real>();
  mrs_real gl = cos(angle) - sin(angle);
  mrs_real gr = sin(angle) + cos(angle);

  if (inObservations_ == 1) 	// mono2stereo
  {
    for (t = 0; t < inSamples_; t++)
    {
      out(0,t) =  gl * in(0,t);     // left
      out(1,t) =  gr * in(0,t);    // right
    }
  }

  else // pass through
  {
    for (o=0; o < onObservations_; o++)
      for (t = 0; t < inSamples_; t++)
      {
        out(o,t) = in(0,t);
      }
  }


}








