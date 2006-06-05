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
    \class NormMaxMin
    \brief Normalize my mapping min/max range to user specified range


*/


#include "NormMaxMin.h"
using namespace std;



NormMaxMin::NormMaxMin(string name)
{
  type_ = "NormMaxMin";
  name_ = name;
  init_ = false;
  addControls();
}


NormMaxMin::~NormMaxMin()
{
}


MarSystem* 
NormMaxMin::clone() const 
{
  return new NormMaxMin(*this);
}

void 
NormMaxMin::addControls()
{
  addDefaultControls();
  addctrl("real/lower", 0.0);
  addctrl("real/upper", 1.0);
  maximums_.create(1);
  minimums_.create(1);
  addctrl("realvec/maximums", maximums_);
  addctrl("realvec/minimums", minimums_);
  setctrlState("realvec/maximums", true);
  setctrlState("realvec/minimums", true);

  addctrl("bool/train", true);
  setctrlState("bool/train", true);

  addctrl("bool/init", false);
  setctrlState("bool/init", true);
  
}


void
NormMaxMin::update()
{
  MRSDIAG("NormMaxMin.cpp - NormMaxMin:update");
  
  setctrl("natural/onSamples", getctrl("natural/inSamples"));
  setctrl("natural/onObservations", getctrl("natural/inObservations"));
  setctrl("real/osrate", getctrl("real/israte"));
  
  setctrl("string/onObsNames", getctrl("string/inObsNames"));
  defaultUpdate();  
  
  init_ = getctrl("bool/init").toBool();

  natural msize = (getctrl("realvec/maximums").toVec().getSize());
  natural nsize = maximums_.getSize();
  
  
  if (msize != nsize) 
    {
      maximums_.stretch(msize);
      minimums_.stretch(msize);
    }
  

  if (!init_)
    {
      maximums_.stretch(inObservations_);
      minimums_.stretch(inObservations_);

      maximums_.setval(DBL_MIN);
      minimums_.setval(DBL_MAX);
      setctrl("realvec/maximums", maximums_);
      setctrl("realvec/minimums", minimums_);  
    }
  train_ = getctrl("bool/train").toBool();
  
  if (!train_)
    {
      maximums_ = getctrl("realvec/maximums").toVec();
      minimums_ = getctrl("realvec/minimums").toVec();
    }
  
  
  
}


void 
NormMaxMin::process(realvec& in, realvec& out)
{
  init_ = true;
  setctrl("bool/init", (MarControlValue)init_);
  
  checkFlow(in,out);


  lower_ = getctrl("real/lower").toReal();
  upper_ = getctrl("real/upper").toReal();
  train_ = getctrl("bool/train").toBool();
  
  if (lower_ > upper_) 
    {
      MRSWARN("Lower is greater than upper");
      return;
    }
  
  
  range_ = upper_ - lower_;
  
  if (train_)
    {
      // first pass calculate min/max limits
      for (o=0; o < inObservations_; o++)
	for (t = 0; t < inSamples_; t++)
	  {
	    if (in(o,t) > maximums_(o))
	      maximums_(o) = in(o,t);
	    if (in(o,t) < minimums_(o))	
	      minimums_(o) = in(o,t);
	    out(o,t) = in(o,t);
	    
	  }
      
      setctrl("realvec/maximums", maximums_);
      setctrl("realvec/minimums", minimums_);  
    }
  else
    {
      // second pass for normalization 
      for (o=0; o < inObservations_; o++)
	for (t = 0; t < inSamples_; t++)
	  {
	    out(o,t) = lower_ + range_ * ((in(o,t) - minimums_(o)) / (maximums_(o) - minimums_(o)));
	  }
      
    }
  
	
  

}







	
	
