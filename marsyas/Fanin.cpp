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
    \class Fanin
    \brief Fanin of MarSystem objects
    
    Combines a series of MarSystem objects to a single MarSystem 
corresponding to executing the System objects one after the other 
in sequence. 
*/


#include "Fanin.h"
using namespace std;


Fanin::Fanin():Composite()
{
  type_ = "Fanin";
}


Fanin::Fanin(string name)
{
  type_ = "Fanin";
  name_ = name;
  addControls();
}



Fanin::~Fanin()
{
  // deleteSlices();
}


Fanin::Fanin(const Fanin& a)
{
  type_ = a.type_;
  name_ = a.name_;
  ncontrols_ = a.ncontrols_; 		

  for (natural i=0; i< a.marsystemsSize_; i++)
    {
      addMarSystem((*a.marsystems_[i]).clone());
    }
  
  dbg_ = a.dbg_;
  mute_ = a.mute_;
}


void 
Fanin::deleteSlices()
{
  vector<realvec *>::const_iterator iter;
  for (iter= slices_.begin(); iter != slices_.end(); iter++)
    {
      delete *(iter);
    }
  slices_.clear();
}



MarSystem* 
Fanin::clone() const 
{
  return new Fanin(*this);
}


void
Fanin::addControls()
{
  addDefaultControls();
}



void 
Fanin::update()
{
  if (marsystemsSize_ != 0) 
    {
      setctrl("natural/inSamples", marsystems_[0]->getctrl("natural/inSamples"));
      natural obs = 0;
      for (natural i=0; i < marsystemsSize_; i++)
	{
	  obs += marsystems_[i]->getctrl("natural/inObservations").toNatural();
	}
      setctrl("natural/inObservations", obs); 
      setctrl("real/israte", marsystems_[0]->getctrl("real/israte"));  
      
      // setctrl("natural/inSamples", getctrl("natural/inSamples"));
      // setctrl("natural/inObservations", getctrl("natural/inObservations"));
      // setctrl("real/israte", getctrl("real/israte"));  
      
      for (natural i=0; i < marsystemsSize_; i++)
	{
	  marsystems_[i]->updctrl("natural/inSamples", getctrl("natural/inSamples"));
	  marsystems_[i]->updctrl("natural/inObservations", 1);
	  marsystems_[i]->updctrl("real/israte", getctrl("real/israte"));
	}
      setctrl("natural/onSamples", marsystems_[0]->getctrl("natural/onSamples").toNatural());
      setctrl("natural/onObservations", (natural)1);
      setctrl("real/osrate", marsystems_[0]->getctrl("real/osrate").toReal());
      
      
      
      deleteSlices();
      
      // SHOULD ADD CHECK THAT THE OUTSLICEINF OF 
      // ALL THE MARSYSTEMS IS THE SAME 
      for (natural i=0; i< marsystemsSize_; i++)
	{
	  slices_.push_back(new realvec(marsystems_[i]->getctrl("natural/onObservations").toNatural() , marsystems_[i]->getctrl("natural/onSamples").toNatural()));			
	  (slices_[i])->setval(0.0);
	}
      
      
      defaultUpdate();
      
    }
  
}

void
Fanin::process(realvec& in, realvec& out)
{
  checkFlow(in,out);
  
  out.setval(0.0);
  
  // Add assertions about sizes
  realvec ob(1,inSamples_);

  for (o=0; o < inObservations_; o++)
    {
      // process each observation 
      for (t=0; t < inSamples_; t++)
	ob(0,t) = in(o,t);
      marsystems_[o]->process(ob, *(slices_[o]));
      for (t=0; t < onSamples_; t++)
	out(0,t) += (*(slices_[o]))(0,t);	  
    }
}



	
