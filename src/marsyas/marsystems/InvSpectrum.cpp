/*
** Copyright (C) 1998-2005 George Tzanetakis <gtzan@cs.uvic.ca>
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

#include "InvSpectrum.h"

using std::ostringstream;
using namespace Marsyas;

InvSpectrum::InvSpectrum(mrs_string name):MarSystem("InvSpectrum",name)
{
  //type_ = "InvSpectrum";
  //name_ = name;
}

InvSpectrum::~InvSpectrum()
{
}


MarSystem*
InvSpectrum::clone() const
{
  return new InvSpectrum(*this);
}


void
InvSpectrum::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)
  setctrl("mrs_natural/onSamples", getctrl("mrs_natural/inObservations"));
  setctrl("mrs_natural/onObservations", getctrl("mrs_natural/inSamples"));
  setctrl("mrs_real/osrate", getctrl("mrs_real/israte")->to<mrs_real>() * getctrl("mrs_natural/inObservations")->to<mrs_natural>());

  tempVec_.create(getctrl("mrs_natural/onSamples")->to<mrs_natural>());
}

void
InvSpectrum::myProcess(realvec& in, realvec& out)
{
  mrs_natural o,t;

  for(o=0 ; o<onObservations_; o++)
  {
    for (t=0; t < onSamples_; t++)
      tempVec_(t) = in(t,o);

    mrs_real *tmp = tempVec_.getData();
    myfft_.rfft(tmp, onSamples_/2, FFT_INVERSE);
    for (t=0; t < onSamples_; t++)
      out(o,t) = tempVec_(t);

  }
}









