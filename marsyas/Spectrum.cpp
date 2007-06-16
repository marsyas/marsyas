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
	\ingroup Analysis
    \brief Compute the complex spectrum of input window 

    Computes the complex spectrum of the input window 
    using the Fast Fourier Transform (FFT). 
*/

#include "Spectrum.h"

using namespace std;
using namespace Marsyas;

Spectrum::Spectrum(string name):MarSystem("Spectrum",name)
{
  ponObservations_ = 0;
  
  cutoff_ = 1.0;
  lowcutoff_ = 0.0;
  re_ = 0.0;
  im_ = 0.0;
  addControls();
}

Spectrum::~Spectrum()
{
}

Spectrum::Spectrum(const Spectrum& a): MarSystem(a)
{
  ctrl_cutoff_ = getctrl("mrs_real/cutoff");
  ctrl_lowcutoff_ = getctrl("mrs_real/lowcutoff");
}

void
Spectrum::addControls()
{
  addctrl("mrs_real/cutoff", 1.0, ctrl_cutoff_);
  setctrlState("mrs_real/cutoff", true);
  addctrl("mrs_real/lowcutoff", 0.0, ctrl_lowcutoff_);
  setctrlState("mrs_real/lowcutoff", true);
}

MarSystem* 
Spectrum::clone() const 
{
  return new Spectrum(*this);
}

void 
Spectrum::myUpdate(MarControlPtr sender)
{
  ctrl_onSamples_->setValue((mrs_natural)1, NOUPDATE);
  ctrl_onObservations_->setValue(ctrl_inSamples_, NOUPDATE);
  ctrl_osrate_->setValue(ctrl_israte_->toReal() / ctrl_inSamples_->toNatural());
  
  cutoff_ = ctrl_cutoff_->toReal();
  lowcutoff_ = ctrl_lowcutoff_->toReal();

  onObservations_ = ctrl_onObservations_->toNatural();

  if (ponObservations_ != onObservations_)
    {
      ostringstream oss;
      for (mrs_natural n=0; n < onObservations_/2; n++)
	{
	  oss << "rbin_" << n << ",";
	  oss << "ibin_" << n << ",";
	}
      ctrl_onObsNames_->setValue(oss.str(), NOUPDATE);
    }
  
  ponObservations_ = onObservations_;
}

void 
Spectrum::myProcess(realvec& in, realvec& out)
{
  
  // copy to output to perform inplace fft 
  // notice transposition of matrix 
  // from row to column 
  for (t=0; t < inSamples_; t++)
    {
      out(t,0) = in(0,t);	
    }

  mrs_real *tmp = out.getData();
  myfft_.rfft(tmp, inSamples_/2, FFT_FORWARD);


  
  if (cutoff_ != 1.0) 
    {
      for (t= (mrs_natural)((cutoff_ * inSamples_) / 2); t < inSamples_/2; t++)
	{
	  out(2*t) = 0;
	  out(2*t+1) = 0;
	}
    }
  
  if (lowcutoff_ != 0.0)
    {
      for (t=0; t < (mrs_natural)((lowcutoff_ * inSamples_) /2); t++)
	{
	  out(2*t) = 0;
	  out(2*t+1) = 0;	
	}
    }
  
  // [!] compare with matlab fft   
  //	 MATLAB_PUT(in, "vec");
  //	 MATLAB_EVAL("out=fft(vec);");
  //   MATLAB_GET("vec", out);
  
  return;
}









	

	
