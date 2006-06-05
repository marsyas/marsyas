/*
** Copyright (C) 1998-2004 George Tzanetakis <gtzan@cs.uvic.ca>
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
    \class fft:
    \brief fft calculation.

   System for calculating the fft (Fast Fourier Transform) of the input fvec.
The actual fft code is scammed from the CARL software and is very similar 
to the fft implementation in Numerical Recipes. Returns an fvec of complex 
values (2*i, 2*i+1) = (Real, Imaginary).
*/

#ifndef MARSYAS_FFT_H 
#define MARSYAS_FFT_H
	


#include <stdio.h>
#include <math.h>
#include "common.h" 

class fft
{
public:
  void bitreverse(real x[], int N);
  void rfft( real x[], int  N, int forward);
  void cfft(real x[], int N, int forward);

  // fft(SignalSource *src);
  
  // void update(unsigned int interSize, unsigned int decimSize);
  // void process(fvec& in, fvec& out);
  // void process(fvec& in, int instart, int inlength, fvec& out, int outstart, int outlength);
  // void unprocess(fvec& out, fvec& in);
};


#endif
	
