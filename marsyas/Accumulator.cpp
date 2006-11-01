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
using namespace Marsyas;
 
Accumulator::Accumulator(string name):Composite("Accumulator", name)
{
  //type_ = "Accumulator";
  //name_ = name;
  
  marsystemsSize_ = 0;
  addControls();
}

Accumulator::~Accumulator()
{
}

MarSystem* 
Accumulator::clone() const 
{
  return new Accumulator(*this);
}

void 
Accumulator::addControls()
{
  addctrl("mrs_natural/nTimes", 5);
  setctrlState("mrs_natural/nTimes", true);
  nTimes_ = 5;
}

void
Accumulator::myUpdate()
{
  MRSDIAG("Accumulator.cpp - Accumulator:myUpdate");
  
  nTimes_ = getctrl("mrs_natural/nTimes")->toNatural();

  string onObsNames;
  
  if (marsystemsSize_ > 0)
    {
      // set input characteristics 
      setctrl("mrs_natural/inSamples", 
	      marsystems_[0]->getctrl("mrs_natural/inSamples")->toNatural());
      setctrl("mrs_natural/inObservations", 
	      marsystems_[0]->getctrl("mrs_natural/inObservations"));
      setctrl("mrs_real/israte", 
	      marsystems_[0]->getctrl("mrs_real/israte"));
			setctrl("mrs_string/inObsNames", 
				marsystems_[0]->getctrl("mrs_string/inObsNames"));
    
      // set output characteristics 
      setctrl("mrs_natural/onSamples", 
	      nTimes_ * marsystems_[0]->getctrl("mrs_natural/onSamples")->toNatural());
      setctrl("mrs_natural/onObservations", 
	      marsystems_[0]->getctrl("mrs_natural/onObservations")->toNatural());
      setctrl("mrs_real/osrate", 
				marsystems_[0]->getctrl("mrs_real/osrate"));

      onObsNames = marsystems_[0]->getctrl("mrs_string/onObsNames")->toString();
    }

  //defaultUpdate();
  onObservations_ = getctrl("mrs_natural/onObservations")->toNatural();
  onSamples_ = getctrl("mrs_natural/onSamples")->toNatural();

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
  setctrl("mrs_string/onObsNames", oss.str());
  
  tout_.create(onObservations_, onSamples_ / nTimes_);
}

void 
Accumulator::myProcess(realvec& in, realvec& out)
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







	

	
