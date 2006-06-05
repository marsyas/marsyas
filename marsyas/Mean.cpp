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
    \class Mean
    \brief Mean calculate the mean of each row of observations

*/



#include "Mean.h"
using namespace std;

Mean::Mean():MarSystem()
{
  type_ = "Mean";
}

Mean::Mean(string name)
{
  type_ = "Mean";
  name_ = name;
  addControls();
}


Mean::~Mean()
{
}


MarSystem* 
Mean::clone() const 
{
  return new Mean(*this);
}

void 
Mean::addControls()
{
  addDefaultControls();
}


void
Mean::update()
{
  MRSDIAG("Mean.cpp - Mean:update");
  setctrl("natural/onSamples", (natural)1);
  setctrl("natural/onObservations", getctrl("natural/inObservations").toNatural());
  setctrl("real/osrate", getctrl("real/israte").toReal());

  obsrow_.create(getctrl("natural/inSamples").toNatural());
  defaultUpdate();

  ostringstream oss;
  string inObsNames = getctrl("string/inObsNames").toString();
  for (int i = 0; i < inObservations_; i++)
    {
      string inObsName;
      string temp;
      inObsName = inObsNames.substr(0, inObsNames.find(","));
      temp = inObsNames.substr(inObsNames.find(",")+1, inObsNames.length());
      inObsNames = temp;
      oss << "Mean" << "_" << inObsName << ",";
    }
  setctrl("string/onObsNames", oss.str());



}




void 
Mean::process(realvec& in, realvec& out)
{
  checkFlow(in,out);

  out.setval(0.0);
  for (o=0; o < inObservations_; o++)
    {
      for (t = 0; t < inSamples_; t++)
	{
	  // Calculate mean 
	  obsrow_(t) = in(o,t);
	}
      out(o,0) = obsrow_.mean();
    }
}

      
      







	
	
	

	
