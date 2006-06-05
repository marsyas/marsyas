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
    \class Accumulator 

    \brief Accumulate result of multiple tick 

    Accumulate result of multiple tick process calls 
    to internal MarSystem. Spit output only once when
    all the results are accumulated. Used to change 
    the rate of process requests
*/




#include "Accumulator.h"
using namespace std;



Accumulator::Accumulator(string name):Composite()
{
  type_ = "Accumulator";
  name_ = name;
  dbg_ = false;
  addControls();
}


Accumulator::~Accumulator()
{
}


Accumulator::Accumulator(const Accumulator& a)
{
  type_ = a.type_;
  name_ = a.name_;
  ncontrols_ = a.ncontrols_; 		
  dbg_ = a.dbg_;
  for (natural i=0; i< a.marsystemsSize_; i++)
    {
      addMarSystem((*a.marsystems_[i]).clone());
    }
}


MarSystem* 
Accumulator::clone() const 
{
  return new Accumulator(*this);
}

void 
Accumulator::addControls()
{
  addDefaultControls();
  addctrl("natural/nTimes", 5);
  nTimes_ = 5;
  setctrlState("natural/nTimes", true);
}






void
Accumulator::update()
{
  MRSDIAG("Accumulator.cpp - Accumulator:update");
  
  nTimes_ = getctrl("natural/nTimes").toNatural();

  string onObsNames;
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
      setctrl("natural/onSamples", 
	      nTimes_ * marsystems_[0]->getctrl("natural/onSamples").toNatural());
      setctrl("natural/onObservations", 
	      marsystems_[0]->getctrl("natural/onObservations").toNatural());
      setctrl("real/osrate", getctrl("real/israte"));
      setctrl("string/inObsNames", marsystems_[0]->getctrl("string/inObsNames"));

      onObsNames = marsystems_[0]->getctrl("string/onObsNames").toString();
    }
  defaultUpdate();


  ostringstream oss;

  for (int i = 0; i < onObservations_; i++)
    {
      string onObsName;
      string temp;
      onObsName = onObsNames.substr(0, onObsNames.find(","));
      temp = onObsNames.substr(onObsNames.find(",")+1, onObsNames.length());
      onObsNames = temp;
      oss << "Acc" << nTimes_ << "_" << onObsName << ",";
    }
  setctrl("string/onObsNames", oss.str());
  
  tout_.create(onObservations_, onSamples_ / nTimes_);
}


void 
Accumulator::process(realvec& in, realvec& out)
{
  checkFlow(in,out);
  
  for (c = 0; c < nTimes_; c++) 
    {
      marsystems_[0]->recvControls(); // HACK STU
      marsystems_[0]->process(in, tout_);
      for (o=0; o < onObservations_; o++)
	for (t = 0; t < onSamples_/nTimes_; t++)
	  {
	    out(o, t + c * (onSamples_/nTimes_)) = tout_(o,t);
	  }
    }
}







	

	
