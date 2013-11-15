/*
** Copyright (C) 1998-2010 George Tzanetakis <gtzan@cs.uvic.ca>
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

#include "OverlapAdd.h"
#include <algorithm>

using std::ostringstream;
using std::max;

using namespace Marsyas;

OverlapAdd::OverlapAdd(mrs_string name):MarSystem("OverlapAdd",name)
{
  this->addControls ();
}

OverlapAdd::~OverlapAdd()
{
}

void
OverlapAdd::addControls()
{
  addctrl("mrs_natural/ratioBlock2Hop", 2);
}

MarSystem*
OverlapAdd::clone() const
{
  return new OverlapAdd(*this);
}

void
OverlapAdd::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)
  mrs_natural ratio = max((mrs_natural)1,getctrl("mrs_natural/ratioBlock2Hop")->to<mrs_natural>());
  setctrl("mrs_natural/onSamples", ctrl_inSamples_->to<mrs_natural>()/ratio);
  setctrl("mrs_natural/onObservations", ctrl_inObservations_->to<mrs_natural>());
  setctrl("mrs_real/osrate", getctrl("mrs_real/israte")->to<mrs_real>());

  back_.stretch(ctrl_onObservations_->to<mrs_natural>(), ctrl_inSamples_->to<mrs_natural>()-ctrl_onSamples_->to<mrs_natural>());
}

void
OverlapAdd::myProcess(realvec& in, realvec& out)
{
  mrs_natural t,o;
  for(o=0 ; o<onObservations_; o++)
  {
    // compute output
    for(t=0; t<onSamples_; t++)
      out(o, t) = back_(o, t)+in(o, t);

    // old code
    //for(t=0;t<onSamples_;t++)
    //	back_(o, t) = in(o, t+onSamples_);

    // shift internal buffer and add new samples
    for (t = onSamples_; t < inSamples_ - onSamples_; t++)
      back_(o,t-onSamples_)	=  back_(o,t) + in(o,t);

    // copy the non-overlapping samples
    for (t = inSamples_-onSamples_; t < inSamples_; t++)
      back_(o,t-onSamples_)	=  in(o,t);
  }
}













