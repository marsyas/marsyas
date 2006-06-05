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
    \class Cascade
    \brief Cascade of MarSystem objects
*/


#include "Cascade.h"
using namespace std;




Cascade::Cascade(string name)
{
  type_ = "Cascade";
  name_ = name;
  addControls();
}


Cascade::~Cascade()
{
  deleteSlices();
}


Cascade::Cascade(const Cascade& a)
{
  type_ = a.type_;
  name_ = a.name_;
  ncontrols_ = a.ncontrols_;		
  
  for (natural i=0; i< a.marsystemsSize_; i++) {
    addMarSystem((*a.marsystems_[i]).clone());
  }
  
  dbg_ = a.dbg_;
  mute_ = a.mute_;
}


void 
Cascade::deleteSlices()
{
  vector<realvec *>::const_iterator iter;
  for (iter = slices_.begin(); iter != slices_.end(); iter++) {
    delete *(iter);
  }
  slices_.clear();
}

MarSystem* 
Cascade::clone() const 
{
  return new Cascade(*this);
}

void 
Cascade::addControls()
{
  addDefaultControls();
}


void 
Cascade::update()
{
  if (marsystemsSize_ != 0) {
    marsystems_[0]->update();
    
    setctrl("natural/inSamples", marsystems_[0]->getctrl("natural/inSamples"));
    setctrl("natural/inObservations", marsystems_[0]->getctrl("natural/inObservations"));
    setctrl("real/israte", marsystems_[0]->getctrl("real/israte"));  
    
    natural onObservations = marsystems_[0]->getctrl("natural/onObservations").toNatural();
    
    for (natural i=1; i < marsystemsSize_; i++) {
      marsystems_[i]->setctrl("natural/inSamples", marsystems_[i-1]->getctrl("natural/onSamples"));
      marsystems_[i]->setctrl("natural/inObservations", marsystems_[i-1]->getctrl("natural/onObservations"));
      marsystems_[i]->setctrl("real/israte", marsystems_[i-1]->getctrl("real/osrate"));
      marsystems_[i]->update();
      onObservations += marsystems_[i]->getctrl("natural/onObservations").toNatural();
    }
    
    setctrl("natural/onSamples", marsystems_[0]->getctrl("natural/onSamples"));
    setctrl("natural/onObservations", onObservations);
    setctrl("real/osrate", marsystems_[0]->getctrl("real/osrate"));
    
    // update buffers between components
    
    if ((natural)slices_.size() < marsystemsSize_) {
      slices_.resize(marsystemsSize_, NULL);
    }
    
    for (natural i = 0; i < marsystemsSize_; i++) {
      if (slices_[i] != NULL) {
	if ((slices_[i])->getRows() != marsystems_[i]->getctrl("natural/onObservations").toNatural() || (slices_[i])->getCols() != marsystems_[i]->getctrl("natural/onSamples").toNatural()) {
	  delete slices_[i];
	  slices_[i] = new realvec(marsystems_[i]->getctrl("natural/onObservations").toNatural(), marsystems_[i]->getctrl("natural/onSamples").toNatural());
	}
      }
      else {
	slices_[i] = new realvec(marsystems_[i]->getctrl("natural/onObservations").toNatural(), marsystems_[i]->getctrl("natural/onSamples").toNatural());
      }
      (slices_[i])->setval(0.0);
    }
    defaultUpdate();
  }
}

      
void 
Cascade::process(realvec& in, realvec& out)
{
  checkFlow(in, out);
  
  natural outIndex = 0;
  natural localIndex = 0;
  
  if (marsystemsSize_ == 1) {
    marsystems_[0]->process(in, out);
  }
  else if (marsystemsSize_ > 1) {
    marsystems_[0]->process(in, *(slices_[0]));
    localIndex = marsystems_[0]->getctrl("natural/onObservations").toNatural();
    for (o = 0; o < localIndex; o++) {
      for (t = 0; t < onSamples_; t++) {
	out(outIndex + o,t) = (*(slices_[0]))(o,t);
      }
    }
    outIndex += localIndex;
    for (natural i = 1; i < marsystemsSize_; i++) {
      marsystems_[i]->process(*(slices_[i-1]), *(slices_[i]));
      localIndex = marsystems_[i]->getctrl("natural/onObservations").toNatural();
      for (o = 0; o < localIndex; o++) {
	for (t = 0; t < onSamples_; t++) {
	  out(outIndex + o,t) = (*(slices_[i]))(o,t);
	}
      }
      outIndex += localIndex;
    }
  }
}

