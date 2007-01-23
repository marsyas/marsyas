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
    \class AutoCorrelation
    \brief Compute the generalized autocorrelation of input window 

    Computes the generalized autocorrelation (with optional magnitude
    compression) using the Fast Fourier Transform (FFT).
*/

#ifndef MARSYAS_AUTOCORRELATION_H
#define MARSYAS_AUTOCORRELATION_H

#include "MarSystem.h"	
#include "fft.h"

namespace Marsyas
{

class AutoCorrelation: public MarSystem
{
private:
  fft *myfft_;
  realvec scratch_;
  virtual void addControls();
	void myUpdate(MarControlPtr sender);
  
public:
  AutoCorrelation(std::string name);
  ~AutoCorrelation();
  AutoCorrelation(const AutoCorrelation& a);
  
  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);
};

}//namespace Marsyas

#endif

	
