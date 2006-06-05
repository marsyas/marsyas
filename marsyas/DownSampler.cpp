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
    \class DownSampler
    \brief Downsample the input signal

    Downsample the signal by an integer factor.
*/




#include "DownSampler.h"
using namespace std;



DownSampler::DownSampler(string name)
{
  type_ = "DownSampler";
  name_ = name;
  addControls();
}


DownSampler::~DownSampler()
{
}


MarSystem* 
DownSampler::clone() const 
{
  return new DownSampler(*this);
}

void 
DownSampler::addControls()
{
  addDefaultControls();
  addctrl("natural/factor", 2);
  setctrlState("natural/factor", true);
}


void
DownSampler::update()
{
  MRSDIAG("DownSampler.cpp - DownSampler:update");
  
  setctrl("natural/onSamples", getctrl("natural/inSamples").toNatural() / getctrl("natural/factor").toNatural());
  
  setctrl("natural/onObservations", getctrl("natural/inObservations"));
  natural factor = getctrl("natural/factor").toNatural();
  setctrl("real/osrate", getctrl("real/israte").toReal() / factor);

  
  defaultUpdate();
}


void 
DownSampler::process(realvec& in, realvec& out)
{
  checkFlow(in,out);

  natural factor = getctrl("natural/factor").toNatural();
  for (o=0; o < inObservations_; o++)
    for (t = 0; t < inSamples_ / factor; t++)
      {
	out(o,t) = in(o,t * factor);
      }

}







	

	
