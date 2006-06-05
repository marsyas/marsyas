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
    \class Confidence
    \brief Confidence calculates classification confidence 

    Calculates classification confidence by majority voting. 
*/



#include "Confidence.h"
using namespace std;



Confidence::Confidence(string name)
{
  type_ = "Confidence";
  name_ = name;
  print_ = false;
  forcePrint_ = false;
  
  predictions_ = 0;
  count_ = 0;
  addControls();
}


Confidence::~Confidence()
{
}


MarSystem* 
Confidence::clone() const 
{
  return new Confidence(*this);
}

void 
Confidence::addControls()
{
  addDefaultControls();
  addctrl("natural/memSize", 40);
  addctrl("natural/nLabels", 2);
  setctrlState("natural/nLabels", true);
  addctrl("string/labelNames", "Music,Speech");
  setctrlState("string/labelNames", true);
  addctrl("bool/print", false);
  setctrlState("bool/print", true);
  addctrl("bool/forcePrint", false);
  setctrlState("bool/forcePrint", true);
}


void
Confidence::update()
{
  MRSDIAG("Confidence.cpp - Confidence:update");
  setctrl("natural/onSamples", getctrl("natural/inSamples"));
  setctrl("natural/onObservations", getctrl("natural/inObservations"));
  setctrl("real/osrate", getctrl("real/israte"));
  
  confidences_.stretch(getctrl("natural/nLabels").toNatural());
  string labelNames = getctrl("string/labelNames").toString();
  
  labelNames_.clear();

  print_ = getctrl("bool/print").toBool();
  forcePrint_ = getctrl("bool/forcePrint").toBool();
  
  
  for (int i = 0; i < getctrl("natural/nLabels").toNatural(); i++)
    {
      string labelName;
      string temp;
      
      labelName = labelNames.substr(0, labelNames.find(","));
      temp = labelNames.substr(labelNames.find(",")+1, labelNames.length());
      labelNames = temp;
      labelNames_.push_back(labelName);
    }  


  defaultUpdate();
}


void 
Confidence::process(realvec& in, realvec& out)
{
  
  checkFlow(in,out);
  bool mute = getctrl("bool/mute").toBool();  
  
  natural memSize = getctrl("natural/memSize").toNatural();
  natural nLabels = getctrl("natural/nLabels").toNatural();
  
  natural label;
  natural l;
  

  if (mute == false) 
    {
      for (o=0; o < inObservations_; o++)
	for (t = 0; t < inSamples_; t++)
	  {
	    
	    
	    out(o,t) = in(o,t);
	    if (o==0) 
	      {
		label = (natural)in(o,t);
		confidences_(label) = confidences_(label) + 1;
	      }
	    
	  }
      count_++;
      

      
      bool cond = ((count_ % memSize) == 0);
      if (cond || forcePrint_)
	{
	  
	  real max_conf = 0;
	  natural max_l = 0;
	  for (l=0; l < nLabels; l++)
	    {
	      real conf = ((confidences_(l)) / count_);
	      if (conf > max_conf) 
		{
		  max_conf = conf;
		  max_l = l;
		}
	    }
	  if (print_) 
	    cout << labelNames_[max_l] << "\t" << 
	      ((confidences_(max_l) / count_)) * 100.0 << endl;

	  if (cond || forcePrint_)
	    {
	      count_ = 0;
	    }
	  
	  
	  
	  confidences_.setval(0.0);
	  
	}
    }
  
}







	

	
