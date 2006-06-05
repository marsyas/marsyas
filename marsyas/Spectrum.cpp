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
    \class Spectrum
    \brief Compute the complex spectrum of input window 

    Computes the complex spectrum of the input window 
using the Fast Fourier Transform (FFT). 
*/


#include "Spectrum.h"
using namespace std;



Spectrum::Spectrum(string name)
{
  type_ = "Spectrum";
  name_ = name;
  ponObservations_ = 0;
  
  addControls();
}




Spectrum::~Spectrum()
{
}

void
Spectrum::addControls()
{
  addDefaultControls();
  addctrl("real/cutoff", 1.0);
  setctrlState("real/cutoff", true);
  addctrl("real/lowcutoff", 0.0);
  setctrlState("real/lowcutoff", true);
}

MarSystem* 
Spectrum::clone() const 
{
  return new Spectrum(*this);
}


void 
Spectrum::update()
{
  
  setctrl("natural/onSamples", (natural)1);
  setctrl("natural/onObservations", getctrl("natural/inSamples"));
  setctrl("real/osrate", getctrl("real/israte").toReal() / getctrl("natural/inSamples").toNatural());
  
  cutoff_ = getctrl("real/cutoff").toReal();
  lowcutoff_ = getctrl("real/lowcutoff").toReal();


  defaultUpdate();

  if (ponObservations_ != onObservations_)
    {
      tempVec_.stretch(inSamples_);
      ostringstream oss;
      for (natural n=0; n < onObservations_/2; n++)
	{
	  oss << "rbin_" << n << ",";
	  oss << "ibin_" << n << ",";
	}
      setctrl("string/onObsNames", oss.str());
    }
  
  ponObservations_ = onObservations_;
  
}



void 
Spectrum::process(realvec& in, realvec& out)
{
  checkFlow(in,out);
  
  // copy to output to perform inplace fft 
  // notice transposition of matrix 
  // from row to column 
  for (t=0; t < inSamples_; t++)
    {
      out(t,0) = in(0,t);	
    }

  real *tmp = tempVec_.getData();
  tmp = out.getData();
  myfft_.rfft(tmp, inSamples_/2, FFT_FORWARD);


  
  if (cutoff_ != 1.0) 
    {
      for (t= (natural)((cutoff_ * inSamples_) / 2); t < inSamples_/2; t++)
	{
	  out(2*t) = 0;
	  out(2*t+1) = 0;
	}
    }
  
  if (lowcutoff_ != 0.0)
    {
      for (t=0; t < (natural)((lowcutoff_ * inSamples_) /2); t++)
	{
	  out(2*t) = 0;
	  out(2*t+1) = 0;	
	}
    }
  
      

  
  return;
}









	

	
