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

#include "PvUnconvert.h"

using namespace std;
using namespace Marsyas;

PvUnconvert::PvUnconvert(string name):MarSystem("PvUnconvert",name)
{
  //type_ = "PvUnconvert";
  //name_ = name;

	addControls();
}


PvUnconvert::~PvUnconvert()
{
  
}


MarSystem* 
PvUnconvert::clone() const 
{
  return new PvUnconvert(*this);
}


void 
PvUnconvert::addControls()
{
  addctrl("mrs_natural/Interpolation", MRS_DEFAULT_SLICE_NSAMPLES/4);
}

void
PvUnconvert::myUpdate(MarControlPtr sender)
{
	(void) sender;
	setctrl("mrs_natural/onSamples", getctrl("mrs_natural/inSamples"));
	setctrl("mrs_natural/onObservations", getctrl("mrs_natural/inObservations")->to<mrs_natural>() - 2);
	setctrl("mrs_real/osrate", getctrl("mrs_real/israte")->to<mrs_real>() / getctrl("mrs_natural/onObservations")->to<mrs_natural>());  
	
	mrs_natural inObservations = getctrl("mrs_natural/inObservations")->to<mrs_natural>();
	mrs_natural onObservations = getctrl("mrs_natural/onObservations")->to<mrs_natural>();
	mrs_real israte = getctrl("mrs_real/israte")->to<mrs_real>();
	
	N2_ = onObservations/2;
	lastphase_.create(N2_+1);
	
	
	fundamental_ = (mrs_real) (israte  / onObservations);
	factor_ = (((getctrl("mrs_natural/Interpolation")->to<mrs_natural>()* TWOPI)/(israte)));
	
	//lmartins: This was missing the defaultUpdate() call which could be havoc!! [!]
	//with the MarSystem refactoring the chances for such a issue are greatly reduced now!
	
}

void 
PvUnconvert::myProcess(realvec& in, realvec& out)
{
	
  //checkFlow(in,out);
  
  mrs_real phase;
  mrs_natural re, amp, im, freq;
  mrs_real magn;


  mrs_realvec mag;
  mag.create(N2_+1);
  
  for (t=0; t <= N2_; t++)
  {
      re = amp = 2*t; 
      im = freq = 2*t+1;
      if (t== N2_)
	  {
		  re = 1;
	  }
	  
      magn = in(re, 0);
	  mag(t) = magn;
	  
      lastphase_(t) += (in(freq,0) - t * fundamental_);
      phase = lastphase_(t) * factor_;
	  
	  if (t==N2_)
		  magn = 0.0;
	  
      out(re,0) = magn * cos(phase);
	  
      if (t != N2_)
		  out(im,0) = -magn * sin(phase);

    }

}

 





	

	

	
	

	
