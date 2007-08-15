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

#include "MarFileSink.h"

using namespace std;
using namespace Marsyas;

MarFileSink::MarFileSink(string name):MarSystem("MarFileSink",name)
{
  //type_ = "MarFileSink";
  //name_ = name;
}

MarFileSink::~MarFileSink()
{
}


MarSystem* 
MarFileSink::clone() const 
{
  return new MarFileSink(*this);
}

void 
MarFileSink::myProcess(realvec& in, realvec& out)
{
  
  mrs_natural nObservations = getctrl("mrs_natural/inObservations")->toNatural();
  mrs_natural nSamples = getctrl("mrs_natural/inSamples")->toNatural();

  checkFlow(in, out);
  
  for (o=0; o < nObservations; o++)
    for (t = 0; t < nSamples; t++)
      {
		out(o,t) = in(o,t);
		cout << out(o,t) << " ";
      }
  cout << endl;
}
















	
	
	
