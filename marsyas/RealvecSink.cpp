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
    \class RealvecSink

*/

#include "RealvecSink.h"

using namespace std;
using namespace Marsyas;

RealvecSink::RealvecSink(string name):MarSystem("RealvecSink",name)
{
  //type_ = "RealvecSink";
  //name_ = name;
  
	count_= 0;

	addControls();
}

RealvecSink::RealvecSink(const RealvecSink& a):MarSystem(a)
{
ctrl_data_ = getctrl("mrs_realvec/data");
}


RealvecSink::~RealvecSink()
{
}


MarSystem* 
RealvecSink::clone() const 
{
  return new RealvecSink(*this);
}

void 
RealvecSink::addControls()
{
  addctrl("mrs_natural/samplesToUse", (mrs_natural)MRS_DEFAULT_SLICE_NSAMPLES);
  setctrlState("mrs_natural/samplesToUse", true);
  addctrl("mrs_bool/done", false);
  setctrlState("mrs_bool/done", true);
  addctrl("mrs_realvec/data", realvec(), ctrl_data_);
  setctrlState("mrs_realvec/data", true);
	setctrlState("mrs_real/israte", true);
}


void
RealvecSink::myUpdate(MarControlPtr sender)
{
  MRSDIAG("RealvecSink.cpp - RealvecSink:myUpdate");
  
  setctrl("mrs_natural/onObservations", getctrl("mrs_natural/inObservations")->toNatural());
	setctrl("mrs_natural/onSamples", getctrl("mrs_natural/inSamples")->toNatural());
  setctrl("mrs_real/osrate", getctrl("mrs_real/israte")->toReal());
 
 count_ = 0;

		if( getctrl("mrs_bool/done")->isTrue()){
    ctrl_data_->stretch(0);
		count_=0;
    setctrl("mrs_bool/done", false);
  }
}

void 
RealvecSink::myProcess(realvec& in, realvec& out)
{
	//checkFlow(in,out);

	ctrl_data_->stretch(inObservations_, count_+inSamples_);
		for (o=0; o < inObservations_; o++)
	  	for (t=0; t < inSamples_; o++)
			{
				out(o, t) = in(o, t);
			  ctrl_data_->setValue(o, count_+t, in(o, t));
			}
				count_+=inSamples_;
	//out.dump();
	}


