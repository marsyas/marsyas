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
	\ingroup none
    \brief Downsample the input signal

    Downsample the signal by an integer factor.
*/

#include "DownSampler.h"

using namespace std;
using namespace Marsyas;


DownSampler::DownSampler(string name):MarSystem("DownSampler",name)
{
  //type_ = "DownSampler";
  //name_ = name;

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
  addctrl("mrs_natural/factor", 2);
  setctrlState("mrs_natural/factor", true);
}

void
DownSampler::myUpdate(MarControlPtr sender)
{
  MRSDIAG("DownSampler.cpp - DownSampler:myUpdate");
  
  setctrl("mrs_natural/onSamples", getctrl("mrs_natural/inSamples")->toNatural() / getctrl("mrs_natural/factor")->toNatural());
  setctrl("mrs_natural/onObservations", getctrl("mrs_natural/inObservations"));
  mrs_natural factor = getctrl("mrs_natural/factor")->toNatural();
  setctrl("mrs_real/osrate", getctrl("mrs_real/israte")->toReal() / factor);
}

void 
DownSampler::myProcess(realvec& in, realvec& out)
{
  //checkFlow(in,out);

  mrs_natural factor = getctrl("mrs_natural/factor")->toNatural();
  for (o=0; o < inObservations_; o++)
    for (t = 0; t < inSamples_ / factor; t++)
    {
			out(o,t) = in(o,t * factor);
    }
}







	

	
