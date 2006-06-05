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
    \class Fanout
    \brief Fanout of MarSystem objects
    
    Combines a series of MarSystem objects to a single MarSystem 
corresponding to executing the System objects one after the other 
in sequence. 
*/


#include "Fanout.h"
using namespace std;

Fanout::Fanout():Composite()
{
  type_ = "Fanout";
}



Fanout::Fanout(string name)
{
  type_ = "Fanout";
  name_ = name;
  addControls();
}



Fanout::~Fanout()
{
  deleteSlices();
}


Fanout::Fanout(const Fanout& a)
{
  type_ = a.type_;
  name_ = a.name_;
  ncontrols_ = a.ncontrols_; 		
  synonyms_ = a.synonyms_;
  
  for (natural i=0; i< a.marsystemsSize_; i++)
    {
      addMarSystem((*a.marsystems_[i]).clone());
    }
  
  dbg_ = a.dbg_;
  mute_ = a.mute_;
}



void 
Fanout::deleteSlices()
{
  vector<realvec *>::const_iterator iter;
  for (iter= slices_.begin(); iter != slices_.end(); iter++)
    {
      delete *(iter);
    }
  slices_.clear();
}


MarSystem* 
Fanout::clone() const 
{
  return new Fanout(*this);
}


void 
Fanout::addControls()
{
  addDefaultControls();
  addctrl("bool/probe", false);
  setctrlState("bool/probe", true);
  addctrl("natural/disable", -1);
  setctrlState("natural/disable", true);
}

void 
Fanout::update()
{
  probe_ = getctrl("bool/probe").toBool();
  if (enabled_.getSize() != marsystemsSize_)
    {
      enabled_.create(marsystemsSize_);
      enabled_.setval(1.0);
    }
  

  disable_ = getctrl("natural/disable").toNatural();
  if (disable_ != -1) 
    enabled_(disable_) = 0.0;
  
  
  natural onObservations = 0;
  

  if (marsystemsSize_ != 0)
    {

      marsystems_[0]->update();
      setctrl("natural/inSamples", marsystems_[0]->getctrl("natural/inSamples"));
      setctrl("natural/inObservations", marsystems_[0]->getctrl("natural/inObservations"));
      setctrl("real/israte", marsystems_[0]->getctrl("real/israte"));  
      setctrl("string/inObsNames", marsystems_[0]->getctrl("string/inObsNames"));
      ostringstream oss;
      oss << marsystems_[0]->getctrl("string/onObsNames");
	  
      
      if (enabled_(0))
	onObservations += marsystems_[0]->getctrl("natural/onObservations").toNatural();

      
      
      for (natural i=1; i < marsystemsSize_; i++)
	{
	  oss << marsystems_[i]->getctrl("string/onObsNames");
	  marsystems_[i]->updctrl("natural/inSamples", 
				  marsystems_[i-1]->getctrl("natural/inSamples"));
	  
	  
	  marsystems_[i]->updctrl("natural/inObservations", 
				  marsystems_[i-1]->getctrl("natural/inObservations"));
	  marsystems_[i]->updctrl("real/israte", 
				  marsystems_[i-1]->getctrl("real/israte"));
	  
	  marsystems_[i]->updctrl("string/inObsNames", marsystems_[0]->getctrl("string/inObsNames"));
	  


	  if (enabled_(i))
	    onObservations += (marsystems_[i]->getctrl("natural/onObservations").toNatural());
	}

      

      setctrl("natural/onSamples", marsystems_[0]->getctrl("natural/onSamples").toNatural());
      setctrl("natural/onObservations", onObservations);
      setctrl("real/osrate", marsystems_[0]->getctrl("real/osrate").toReal());
      
      setctrl("string/onObsNames", oss.str());
      

      // update buffers between components 
      if ((natural)slices_.size() < marsystemsSize_) 
	slices_.resize(marsystemsSize_, NULL);
      


      for (natural i=0; i< marsystemsSize_; i++)
	{
	  if (slices_[i] != NULL) 
	    {
	      if ((slices_[i])->getRows() != marsystems_[i]->getctrl("natural/onObservations").toNatural()  ||
		  (slices_[i])->getCols() != marsystems_[i]->getctrl("natural/onSamples").toNatural())
		{
		  delete slices_[i];
		  slices_[i] = new realvec(marsystems_[i]->getctrl("natural/onObservations").toNatural(), 
					   marsystems_[i]->getctrl("natural/onSamples").toNatural());
		}
	    }
	  else 
	    {
	      slices_[i] = new realvec(marsystems_[i]->getctrl("natural/onObservations").toNatural(), 
				       marsystems_[i]->getctrl("natural/onSamples").toNatural());
	    }
	  (slices_[i])->setval(0.0);

	}
      
      defaultUpdate();
    }
  
}

      
void
Fanout::process(realvec& in, realvec& out)
{
  checkFlow(in, out);
  natural outIndex = 0;
  natural localIndex = 0;
  
  for (i = 0; i < marsystemsSize_; i++)
    {
      if (enabled_(i))
	{
	  marsystems_[i]->process(in, *(slices_[i]));
	  localIndex = marsystems_[i]->getctrl("natural/onObservations").toNatural();
	  
	  for (o=0; o < localIndex; o++)
	    for (t=0; t < onSamples_; t++)
	      out(outIndex + o,t) = (*(slices_[i]))(o,t);
	  
	  outIndex += localIndex;      
	}
      

    }
}
