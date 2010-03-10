/*
** Copyright (C) 1998-2007 George Tzanetakis <gtzan@cs.uvic.ca>
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

#include "BeatHistogram.h"

using namespace std;
using namespace Marsyas;


BeatHistogram::BeatHistogram(string name):MarSystem("BeatHistogram",name)
{
	addControls();
}


BeatHistogram::~BeatHistogram()
{
}


MarSystem* 
BeatHistogram::clone() const 
{
  return new BeatHistogram(*this);
}

void 
BeatHistogram::addControls()
{
  addctrl("mrs_real/gain", 1.0);
  addctrl("mrs_bool/reset", false);
  setctrlState("mrs_bool/reset", true);
  addctrl("mrs_natural/startBin", 0);
  setctrlState("mrs_natural/startBin", true);
  addctrl("mrs_natural/endBin", 100);
  setctrlState("mrs_natural/endBin", true);
}

void
BeatHistogram::myUpdate(MarControlPtr sender)
{
	(void) sender;
  MRSDIAG("BeatHistogram.cpp - BeatHistogram:myUpdate");
  
  startBin_ = getctrl("mrs_natural/startBin")->to<mrs_natural>();
  endBin_ = getctrl("mrs_natural/endBin")->to<mrs_natural>();
  reset_ = getctrl("mrs_bool/reset")->to<mrs_bool>();  
  
  setctrl("mrs_natural/onSamples", endBin_ - startBin_);
  setctrl("mrs_natural/onObservations", getctrl("mrs_natural/inObservations"));
  setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));
}


void 
BeatHistogram::myProcess(realvec& in, realvec& out)
{
	
  
  //checkFlow(in,out);

  if (reset_) 
    {
      out.setval(0.0);
      reset_ = false;
      setctrl("mrs_bool/reset", false);
    }

  mrs_natural bin=0;
  mrs_real amp;
  mrs_real sum_amp;
  mrs_real factor; 
  mrs_natural index=0;
  mrs_real srate = getctrl("mrs_real/israte")->to<mrs_real>();
  mrs_natural count = 1;
  mrs_natural prev_bin =0;
  
  mrs_real sumamp = 0.0;
  
  


  for (o=0; o < inObservations_; o++)
	  for (t = 1; t < inSamples_; t++)
	  {
		  bin = (mrs_natural)(srate * 60.0  / t + 0.5);
		  amp = in(o,t);
		  // amp = in(o,t) / in(o,0); // normalize so that 0-lag is 1 
		 
		  
		  if ((bin > 40)&&(bin < endBin_))
		  {
			  
			  if (prev_bin == bin) 
			  {
				  sumamp += amp;
				  count++;
			  }
			  
			  else 
			  {
				  sumamp += amp;
				  out(0,prev_bin) += sumamp / count;
				  count = 1;
				  sumamp = 0.0;
			  }
			  
			  prev_bin = bin;	  
		  }
		  
	  }
  // out.normMaxMin();
  
}







	

	
