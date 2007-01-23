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
    \class HalfWaveRectifier
    \brief Half Wave Rectify 

*/

#include "HalfWaveRectifier.h"

using namespace std;
using namespace Marsyas;

HalfWaveRectifier::HalfWaveRectifier(string name):MarSystem("HalfWaveRectifier",name)
{
  //type_ = "HalfWaveRectifier";
  //name_ = name;
}


HalfWaveRectifier::~HalfWaveRectifier()
{
}

MarSystem*  
HalfWaveRectifier::clone() const 
{
  return new HalfWaveRectifier(*this);
}

void 
HalfWaveRectifier::myUpdate(MarControlPtr sender)
{
  setctrl("mrs_natural/onSamples", getctrl("mrs_natural/inSamples"));
  setctrl("mrs_natural/onObservations", getctrl("mrs_natural/inObservations"));
  setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));
}

void 
HalfWaveRectifier::myProcess(realvec& in, realvec& out)
{
  
  mrs_natural inSamples = getctrl("mrs_natural/inSamples")->toNatural();
  
  
  for (t=0; t < inSamples; t++)
    {
      if (in(0,t) < 0.0)
		out(0,t) = 0.0;
      else
		out(0,t) = in(0,t);
    }
}






	

	
