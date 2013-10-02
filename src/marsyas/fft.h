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

#ifndef MARSYAS_FFT_H
#define MARSYAS_FFT_H

#include <cstdio>
#include <cmath>
#include <marsyas/common_header.h>

namespace Marsyas
{
/**
    \class fft:
    \brief fft calculation.

   System for calculating the fft (Fast Fourier Transform) of the input fvec.
The actual fft code is scammed from the CARL software and is very similar
to the fft implementation in Numerical Recipes. Returns an fvec of complex
values (2*i, 2*i+1) = (Real, Imaginary).
*/


class fft
{
public:
  void bitreverse(mrs_real x[], int N);
  void rfft( mrs_real x[], int  N, int forward);
  void cfft(mrs_real x[], int N, int forward);

  // fft(SignalSource *src);

  // void update(unsigned int interSize, unsigned int decimSize);
  // void myProcess(fvec& in, fvec& out);
  // void myProcess(fvec& in, int instart, int inlength, fvec& out, int outstart, int outlength);
  // void unprocess(fvec& out, fvec& in);
};

}//namespace Marsyas


#endif

