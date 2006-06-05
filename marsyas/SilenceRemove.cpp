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
    \class SilenceRemove 

    \brief SilenceRemove takes as argument another Marsystem 
and ticks it ignoring the output when it is silent. Effectively 
tick silence remove is the same as playing the sound with 
silences removed. 
*/


#include "SilenceRemove.h"
using namespace std;



SilenceRemove::SilenceRemove(string name):Composite()
{
  type_ = "SilenceRemove";
  name_ = name;
  addControls();
}


SilenceRemove::~SilenceRemove()
{
}


SilenceRemove::SilenceRemove(const SilenceRemove& a)
{
  type_ = a.type_;
  name_ = a.name_;
  dbg_ = a.dbg_;
  ncontrols_ = a.ncontrols_; 		

  for (natural i=0; i< a.marsystemsSize_; i++)
    {
      addMarSystem((*a.marsystems_[i]).clone());
    }
}


MarSystem* 
SilenceRemove::clone() const 
{
  return new SilenceRemove(*this);
}

void 
SilenceRemove::addControls()
{
  addDefaultControls();
  addctrl("real/threshold", 0.0);
  setctrlState("real/threshold", true);
}





void
SilenceRemove::update()
{
  MRSDIAG("SilenceRemove.cpp - SilenceRemove:update");
  
  threshold_ = getctrl("real/threshold").toReal();
  
  if (marsystemsSize_ > 0)
    {
      // set input characteristics 
      setctrl("natural/inSamples", 
	      marsystems_[0]->getctrl("natural/inSamples").toNatural());
      setctrl("natural/inObservations", 
	      marsystems_[0]->getctrl("natural/inObservations"));
      setctrl("real/israte", 
	      marsystems_[0]->getctrl("real/israte"));
      
      // set output characteristics 
      setctrl("natural/onSamples", marsystems_[0]->getctrl("natural/onSamples").toNatural());
      setctrl("natural/onObservations", marsystems_[0]->getctrl("natural/onObservations").toNatural());
      setctrl("real/osrate", getctrl("real/israte"));
      marsystems_[0]->update();
    }

  
  defaultUpdate();
}


void 
SilenceRemove::process(realvec& in, realvec& out)
{
  
  checkFlow(in,out);
  real rms = 0.0;
  natural count = 0;

  threshold_ = (real)0.01;

  
  do 
    {
      
      marsystems_[0]->process(in, out);
      
      for (o=0; o < onObservations_; o++)
	for (t = 0; t < onSamples_; t++)
	  {
	    rms += (out(o,t) * out(o,t));
	    count++;
	  }
      rms /= count;
      rms = sqrt(rms);
      count = 0;
    } while (rms < threshold_ && (marsystems_[0]->getctrl("bool/notEmpty").toBool()));
  
  
}







	

	
	
