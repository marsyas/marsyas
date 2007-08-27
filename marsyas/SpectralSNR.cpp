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
 



#include "SpectralSNR.h"

using namespace std;
using namespace Marsyas;



SpectralSNR::SpectralSNR(string name):MarSystem("SpectralSNR",name)
{
  addControls();
}

SpectralSNR::SpectralSNR(const SpectralSNR& a):MarSystem(a)
{
}


SpectralSNR::~SpectralSNR()
{
}

void
SpectralSNR::addControls()
{
}

MarSystem* 
SpectralSNR::clone() const 
{
  return new SpectralSNR(*this);
}


void 
SpectralSNR::myUpdate(MarControlPtr sender)
{
  ctrl_onSamples_->setValue((mrs_natural)1, NOUPDATE);
  ctrl_onObservations_->setValue((mrs_natural)1, NOUPDATE);
  ctrl_osrate_->setValue(ctrl_israte_->to<mrs_real>());
  ctrl_onObsNames_->setValue(ctrl_inObsNames_);
  
}

void 
SpectralSNR::myProcess(realvec& in, realvec& out)
{
  cout << "SpectralSNR called" << endl;
 
  mrs_real orig;
  mrs_real extr;
  mrs_real ratio;
  mrs_natural N2 = inObservations_/2;
  mrs_real sum = 0.0;
  cout << "N2 = " << N2 << endl;
  
  
  
  for (t = 0; t < inSamples_; t++)
    {
      for (o=0; o < N2; o++)
	{
	  orig = in(o,0);
	  extr = in(N2+o, 0);
	  if (orig != 0.0) 
	    sum += (orig * orig) / ((orig-extr) * (orig-extr));
	}
    }
  if (sum != 0.0) sum /= N2;
  
  
  out(0,0) = 10.0 * log10(sqrt(sum));

  cout << "sum = " << sum << endl;
  cout << "out = " << out(0,0) << endl;
  
}









	

	
	

	

	
