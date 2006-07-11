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
using namespace Marsyas;

NormMaxMin::NormMaxMin(string name):MarSystem("NormMaxMin",name)
{
  //type_ = "NormMaxMin";
  //name_ = name;

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
  addctrl("mrs_real/lower", 0.0);
  addctrl("mrs_real/upper", 1.0);
  maximums_.create(1);
  minimums_.create(1);
  addctrl("mrs_realvec/maximums", maximums_);
  addctrl("mrs_realvec/minimums", minimums_);
  setctrlState("mrs_realvec/maximums", true);
  setctrlState("mrs_realvec/minimums", true);

  addctrl("mrs_bool/train", true);
  setctrlState("mrs_bool/train", true);

  addctrl("mrs_bool/init", false);
  setctrlState("mrs_bool/init", true);
}


void
NormMaxMin::localUpdate()
{
  MRSDIAG("NormMaxMin.cpp - NormMaxMin:localUpdate");
  
//   setctrl("mrs_natural/onSamples", getctrl("mrs_natural/inSamples"));
//   setctrl("mrs_natural/onObservations", getctrl("mrs_natural/inObservations"));
//   setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));
//   setctrl("mrs_string/onObsNames", getctrl("mrs_string/inObsNames"));
	MarSystem::localUpdate();
  	
	//defaultUpdate();  [!]
	inObservations_ = getctrl("mrs_natural/inObservations").toNatural();
  
  init_ = getctrl("mrs_bool/init").toBool();

  mrs_natural msize = (getctrl("mrs_realvec/maximums").toVec().getSize());
  mrs_natural nsize = maximums_.getSize();
    
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
      setctrl("mrs_realvec/maximums", maximums_);
      setctrl("mrs_realvec/minimums", minimums_);  
    }
  train_ = getctrl("mrs_bool/train").toBool();
  
  if (!train_)
    {
      maximums_ = getctrl("mrs_realvec/maximums").toVec();
      minimums_ = getctrl("mrs_realvec/minimums").toVec();
    } 
}


void 
NormMaxMin::process(realvec& in, realvec& out)
{
  init_ = true;
  setctrl("mrs_bool/init", (MarControlValue)init_);
  
  checkFlow(in,out);


  lower_ = getctrl("mrs_real/lower").toReal();
  upper_ = getctrl("mrs_real/upper").toReal();
  train_ = getctrl("mrs_bool/train").toBool();
  
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
      
      setctrl("mrs_realvec/maximums", maximums_);
      setctrl("mrs_realvec/minimums", minimums_);  
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







	
	
