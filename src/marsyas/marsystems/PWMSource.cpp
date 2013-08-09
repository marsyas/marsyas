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

#include "PWMSource.h"

using namespace std;
using namespace Marsyas;

PWMSource::PWMSource(mrs_string name):MarSystem("PWMSource",name)
{
  //type_ = "PWMSource";
  //name_ = name;

  addControls();


}

PWMSource::~PWMSource()
{
}

MarSystem*
PWMSource::clone() const
{
  return new PWMSource(*this);
}

void
PWMSource::addControls()
{
  addctrl("mrs_real/frequency", 440.0);
  addctrl("mrs_real/duty_cicle", 0.5);
}

void
PWMSource::myUpdate(MarControlPtr sender)
{

//   setctrl("mrs_natural/onSamples", getctrl("mrs_natural/inSamples"));
//   setctrl("mrs_natural/onObservations", getctrl("mrs_natural/inObservations"));
//   setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));
  MarSystem::myUpdate(sender);
}

void
PWMSource::myProcess(realvec &in, realvec &out)
{
  (void) in;

  mrs_real frequency = (getctrl("mrs_real/frequency")->to<mrs_real>());
  mrs_real duty =  getctrl("mrs_real/duty_cicle")->to<mrs_real>();
  mrs_real irate = (getctrl("mrs_real/israte")->to<mrs_real>());
  mrs_real incr = frequency / irate;
  mrs_natural inSamples = getctrl("mrs_natural/inSamples")->to<mrs_natural>();

//	cout << "f=" << frequency;
//	cout << "d=" << duty;
//	cout << "irate=" << irate;
//	cout << "deltaphase=" << incr;
//	cout << "insamples=" << inSamples;


  for (mrs_natural t=0; t < inSamples; t++)
  {
    out(0,t)=0;
    phase += incr;
    if (phase>duty) {
      out(0,t) = 0;
    }	 else {
      out(0,t) = 1;
    }
    if (phase>1) {
      phase=0;
    }
  }
}



//00053 void
//00054 NoiseSource::myProcess(realvec &in, realvec &out)
//00055 {
//00056     for (mrs_natural t=0; t < inSamples_; t++)
//00057         out(t) = (mrs_real)(2.0 * rand() / (RAND_MAX + 1.0) )-1;
//00058 }
//00059
//00060
//00061
