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

*/




#include "MemorySource.h"
using namespace std;



MemorySource::MemorySource(string name)
{
  type_ = "MemorySource";
  name_ = name;
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
  samplesToUse_ = (natural)MRS_DEFAULT_SLICE_NSAMPLES;
  addctrl("natural/samplesToUse", (natural)MRS_DEFAULT_SLICE_NSAMPLES);
  setctrlState("natural/samplesToUse", true);
  addctrl("bool/done", false);
  setctrlState("bool/done", true);
  addDefaultControls();  
}


void
MemorySource::update()
{
  MRSDIAG("MemorySource.cpp - MemorySource:update");
  
  setctrl("natural/onObservations", getctrl("natural/inObservations")  );
  setctrl("real/osrate", getctrl("real/israte").toReal());
  samplesToUse_ = getctrl("natural/samplesToUse").toNatural();
  
  if( getctrl("bool/done").toBool() ){
    count_ = 0;
    setctrl("bool/done", (MarControlValue) false);
  }
  
  defaultUpdate();
}


void 
MemorySource::process(realvec& in, realvec& out)
{
  checkFlow(in,out);
  
  
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
    setctrl("bool/done", (MarControlValue) true);  
}


