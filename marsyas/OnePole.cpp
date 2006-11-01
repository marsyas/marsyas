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
    \class OnePole
    \brief Simple one-pole filter

    OnePole Filter. 
    Output is normalized by setting the
    gain to (1.0-a). 
*/

#include "OnePole.h"

using namespace std;
using namespace Marsyas;


OnePole::OnePole(string name):MarSystem("OnePole",name)
{
  //type_ = "OnePole";
  //name_ = name;

	addControls();
}


OnePole::~OnePole()
{
}


MarSystem* 
OnePole::clone() const 
{
  return new OnePole(*this);
}

void 
OnePole::addControls()
{
  addctrl("mrs_real/alpha", 0.9);
}


void
OnePole::myUpdate()
{
  MRSDIAG("OnePole.cpp - OnePole:myUpdate");
  
//   setctrl("mrs_natural/onSamples", getctrl("mrs_natural/inSamples"));
//   setctrl("mrs_natural/onObservations", getctrl("mrs_natural/inObservations"));
//   setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));
	MarSystem::myUpdate();
  
  alpha_ = getctrl("mrs_real/alpha")->toReal();
  gain_ = (mrs_real)(1.0 - alpha_);
}

void 
OnePole::myProcess(realvec& in, realvec& out)
{
  checkFlow(in,out);

  for (o=0; o < inObservations_; o++)
    for (t = 1; t < inSamples_; t++)
      {
	out(o,t) = gain_ * in(o,t) + alpha_ * out(o, t-1);
      }
}







	
	
