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
    \class FullWaveRectifier
    \brief FullWaveRectifier


*/

#include "FullWaveRectifier.h"

using namespace std;
using namespace Marsyas;


FullWaveRectifier::FullWaveRectifier(string name):MarSystem("FullWaveRectifier",name)
{
  //type_ = "FullWaveRectifier";
  //name_ = name;
}


FullWaveRectifier::~FullWaveRectifier()
{
}


MarSystem* 
FullWaveRectifier::clone() const 
{
  return new FullWaveRectifier(*this);
}

void
FullWaveRectifier::myUpdate()
{
  MRSDIAG("FullWaveRectifier.cpp - FullWaveRectifier:myUpdate");
  
  setctrl("mrs_natural/onSamples", getctrl("mrs_natural/inSamples"));
  setctrl("mrs_natural/onObservations", getctrl("mrs_natural/inObservations"));
  setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));
}


void 
FullWaveRectifier::myProcess(realvec& in, realvec& out)
{
  
  //checkFlow(in,out);

  for (o=0; o < inObservations_; o++)
    for (t = 0; t < inSamples_; t++)
      {
	if (in(o,t) < 0.0) 
	  out(o,t) = -in(o,t);
	else 
	  out(o,t) = in(o,t);
      }
}







	

	
