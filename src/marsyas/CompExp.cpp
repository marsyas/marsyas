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

#include "CompExp.h"
#include "common_source.h"

using std::ostringstream;
using namespace Marsyas;

CompExp::CompExp(mrs_string name):MarSystem("CompExp",name)
{
  //type_ = "CompExp";
  //name_ = name;

  xdprev_ = 0.0;
  alpha_ = 0.0;

	addControls();
}


CompExp::~CompExp()
{
}


MarSystem* 
CompExp::clone() const 
{
  return new CompExp(*this);
}

void 
CompExp::addControls()
{
  addctrl("mrs_real/thresh", 1.0);
  addctrl("mrs_real/at", 0.0001);
  addctrl("mrs_real/rt", 0.130);
  addctrl("mrs_real/slope", 1.0);
}

void
CompExp::myUpdate(MarControlPtr sender)
{
	(void) sender;  //suppress warning of unused parameter(s)
  MRSDIAG("CompExp.cpp - CompExp:myUpdate");
  
  setctrl("mrs_natural/onSamples", getctrl("mrs_natural/inSamples"));
  setctrl("mrs_natural/onObservations", getctrl("mrs_natural/inObservations"));
  setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));
  
	//defaultUpdate(); [!]
	inSamples_ = getctrl("mrs_natural/inSamples")->to<mrs_natural>();
 
	xd_.create(inSamples_);
  gains_.create(inSamples_);
}


void 
CompExp::myProcess(realvec& in, realvec& out)
{
  //checkFlow(in,out);
  mrs_natural o,t;
  mrs_real thresh = getctrl("mrs_real/thresh")->to<mrs_real>();
  mrs_real at = getctrl("mrs_real/at")->to<mrs_real>();
  mrs_real rt = getctrl("mrs_real/rt")->to<mrs_real>();
  mrs_real slope = getctrl("mrs_real/slope")->to<mrs_real>();
  
  // calculate at and rt time
  at = 1 - exp(-2.2/(22050*at));
  rt = 1 - exp(-2.2/(22050*rt));
	
  for (o = 0; o < inObservations_; o++)
    for (t = 0; t < inSamples_; t++)
      {
		
		// Calculates the current amplitude of signal and incorporates
		// the at and rt times into xd(o,t)
		alpha_ = fabs(in(o,t)) - xdprev_;
		
		if (alpha_<0)
		{
		   alpha_ = 0;
		}
		
		xd_(o,t)=xdprev_*(1-rt)+at*alpha_;
		xdprev_ = xd_(o,t);
		
		if (xd_(o,t) > fabs(thresh))
		{
		  // Compressor
		  if (thresh < 0)
		    {
			gains_(o,t) = pow((mrs_real)10.0,-slope*(log10(xd_(o,t))-log10(fabs(thresh))));
			//  linear calculation of gains_ = 10^(-Compressor Slope * (current value - Compressor Threshold))
			}
		  // Expander
		  else
		  if (thresh >= 0)
		    {
			gains_(o,t) = pow((mrs_real)10.0,slope*(log10(xd_(o,t))-log10(thresh)));
			//  linear calculation of gains_ = 10^(Expander Slope * (current value - Expander Threshold))
			}
		}
		else
		{
		  gains_(o,t) = 1;
		}
		
		// If signal is expanded past the maximum amplitude of 1.0
		// The singal is reduced to elimiate any clipping
		while (fabs(gains_(o,t)*in(o,t)) > 1.0)
		  {
			gains_(o,t) = gains_(o,t) - 0.01;
		  }
			
	    out(o,t) =  gains_(o,t) * in(o,t);
      }
}







	
