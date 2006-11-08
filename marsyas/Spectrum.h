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

#ifndef MARSYAS_SPECTRUM_H
#define MARSYAS_SPECTRUM_H

#include "MarSystem.h"	
#include "fft.h"

namespace Marsyas
{

class Spectrum: public MarSystem
{
private:
  fft myfft_;
  mrs_real cutoff_;
  mrs_real lowcutoff_;
  
  void addControls();
	void myUpdate();
  mrs_real re_,im_;
  mrs_natural ponObservations_;
 
public:
  Spectrum(std::string name);
  ~Spectrum();
  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);
  
};

}//namespace Marsyas

#endif

	
