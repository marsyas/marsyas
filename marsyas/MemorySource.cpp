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
    \class MemorySource
	\ingroup none

*/

#include "MemorySource.h"

using namespace std;
using namespace Marsyas;

MemorySource::MemorySource(string name):MarSystem("MemorySource",name)
{
  //type_ = "MemorySource";
  //name_ = name;
  
	count_= 0;

	addControls();
}


MemorySource::~MemorySource()
{
}


MarSystem* 
MemorySource::clone() const 
{
  return new MemorySource(*this);
}

void 
MemorySource::addControls()
{
  samplesToUse_ = (mrs_natural)MRS_DEFAULT_SLICE_NSAMPLES;
  addctrl("mrs_natural/samplesToUse", (mrs_natural)MRS_DEFAULT_SLICE_NSAMPLES);
  setctrlState("mrs_natural/samplesToUse", true);
  addctrl("mrs_bool/done", false);
  setctrlState("mrs_bool/done", true);
}


void
MemorySource::myUpdate(MarControlPtr sender)
{
  MRSDIAG("MemorySource.cpp - MemorySource:myUpdate");
  
  setctrl("mrs_natural/onObservations", getctrl("mrs_natural/inObservations")  );
  setctrl("mrs_real/osrate", getctrl("mrs_real/israte")->toReal());
  samplesToUse_ = getctrl("mrs_natural/samplesToUse")->toNatural();
  
  if( getctrl("mrs_bool/done")->isTrue()){
    count_ = 0;
    setctrl("mrs_bool/done", false);
  }
}

void 
MemorySource::myProcess(realvec& in, realvec& out)
{
  //checkFlow(in,out);

  if( count_ < 1 + (samplesToUse_ -1) / onSamples_ ){
    
    for (o=0; o < inObservations_; o++){
      for (t = 0 ; t < onSamples_ && count_*onSamples_+t < samplesToUse_ ; t++)
		{
		  out(o,t) = in(o,count_*onSamples_+t);
		}
      
      for( ; t < onSamples_ ; t++ )
		{
		  out(o,t) = 0.;
		}
    }
    count_++;
  }
  
  if( count_ >= 1 + (samplesToUse_ -1) / onSamples_ )
    setctrl("mrs_bool/done", true);  
}


