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

#include "PeakPeriods2BPM.h"
#include "../common_source.h"


using std::ostringstream;
using namespace Marsyas;

PeakPeriods2BPM::PeakPeriods2BPM(mrs_string name):MarSystem("PeakPeriods2BPM",name)
{
  addControls();
}


PeakPeriods2BPM::~PeakPeriods2BPM()
{
}


void
PeakPeriods2BPM::addControls()
{
  addctrl("mrs_real/factor", 1.0);
}

MarSystem*
PeakPeriods2BPM::clone() const
{
  return new PeakPeriods2BPM(*this);
}

void
PeakPeriods2BPM::myUpdate(MarControlPtr sender)
{
  MRSDIAG("PeakPeriods2BPM.cpp - PeakPeriods2BPM:myUpdate");

//   setctrl("mrs_natural/onSamples", getctrl("mrs_natural/inSamples"));
//   setctrl("mrs_natural/onObservations", getctrl("mrs_natural/inObservations"));
//   setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));
  MarSystem::myUpdate(sender);

  srate_ = getctrl("mrs_real/israte")->to<mrs_real>();

}

void
PeakPeriods2BPM::myProcess(realvec& in, realvec& out)
{
  mrs_natural t,o;
  //checkFlow(in,out);
  factor_ = getctrl("mrs_real/factor")->to<mrs_real>();
  factor_ = 4.0;

  for (o=0; o < inObservations_; o++)
    for (t = 0; t < inSamples_/2; t++)
    {
      out(o,2*t) = in(o,2*t);
      // out(o,2*t+1) = (mrs_real)(srate_ * 60.0 / in(o, 2*t+1));
      out(o,2*t+1) = (mrs_real)(srate_ * 60.0 * factor_ / in(o, 2*t+1));
// 			cout << in(o,2*t+1) << "-" << out(o,2*t+1) << endl;
    }

}










