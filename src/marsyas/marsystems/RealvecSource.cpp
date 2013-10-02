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

#include "RealvecSource.h"
#include "../common_source.h"

using namespace std;
using namespace Marsyas;

RealvecSource::RealvecSource(mrs_string name):MarSystem("RealvecSource",name)
{
  count_= 0;
  addControls();
}

RealvecSource::RealvecSource(const RealvecSource& a):MarSystem(a)
{
  count_ = 0;
  ctrl_data_ = getctrl("mrs_realvec/data");
}


RealvecSource::~RealvecSource()
{
}


MarSystem*
RealvecSource::clone() const
{
  return new RealvecSource(*this);
}

void
RealvecSource::addControls()
{
  samplesToUse_ = (mrs_natural)MRS_DEFAULT_SLICE_NSAMPLES;
  addctrl("mrs_bool/done", false);
  setctrlState("mrs_bool/done", true);
  addctrl("mrs_realvec/data", realvec(), ctrl_data_);
  setctrlState("mrs_realvec/data", true);
  setctrlState("mrs_real/israte", true);
}


void
RealvecSource::myUpdate(MarControlPtr sender)
{
  (void)sender;
  MRSDIAG("RealvecSource.cpp - RealvecSource:myUpdate");

  inSamples_ = getctrl("mrs_natural/inSamples")->to<mrs_natural>();
  inObservations_ = getctrl("mrs_natural/inObservations")->to<mrs_natural>();
  israte_ = getctrl("mrs_real/israte")->to<mrs_real>();

  const realvec& data = ctrl_data_->to<realvec> ();

  setctrl("mrs_natural/onObservations", data.getRows());
  setctrl("mrs_natural/onSamples", inSamples_);
  setctrl("mrs_real/osrate", israte_);
  samplesToUse_ = data.getCols();

  count_ = 0;

  if( getctrl("mrs_bool/done")->isTrue()) {
    setctrl("mrs_bool/done", false);
  }
}

void
RealvecSource::myProcess(realvec& in, realvec& out)
{
  mrs_natural o,t;
  (void) in;
  //checkFlow(in,out);
  const realvec& data = ctrl_data_->to<realvec> ();

  if( count_ < samplesToUse_)
  {
    for (o=0; o < onObservations_; o++)
    {
      for (t=0; t < onSamples_; t++)
      {
        out(o,t) = data(o,count_ + t);
      }
    }
    count_ += onSamples_;
  }
  else
    setctrl("mrs_bool/done", true);

  if (count_ >= samplesToUse_)
  {
    setctrl("mrs_bool/done", true);
  }

  //out.dump();
}
