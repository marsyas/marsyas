/*
** Copyright (C) 1998-2005 George Tzanetakis <gtzan@cs.uvic.ca>
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
    \class InvSpectrum
    \brief Convert back the complex spectrum to time domain

    Convert back the complex spectrum to time domain 
using the Inverse Fast Fourier Transform (IFFT) 
*/

#include "InvSpectrum.h"

using namespace std;
using namespace Marsyas;

InvSpectrum::InvSpectrum(string name):MarSystem("InvSpectrum",name)
{
  //type_ = "InvSpectrum";
  //name_ = name;
}

InvSpectrum::~InvSpectrum()
{
}


MarSystem* 
InvSpectrum::clone() const 
{
  return new InvSpectrum(*this);
}


void 
InvSpectrum::myUpdate(MarControlPtr sender)
{
  setctrl("mrs_natural/onSamples", getctrl("mrs_natural/inObservations"));
  setctrl("mrs_natural/onObservations", (mrs_natural)1);
  setctrl("mrs_real/osrate", getctrl("mrs_real/israte")->toReal() * getctrl("mrs_natural/inObservations")->toNatural());
  
  tempVec_.create(getctrl("mrs_natural/onSamples")->toNatural());
}

void 
InvSpectrum::myProcess(realvec& in, realvec& out)
{
  //checkFlow(in,out);
  
  for (t=0; t < onSamples_; t++)
    out(0,t) = in(t,0);

  mrs_real *tmp = out.getData();
  myfft_.rfft(tmp, onSamples_/2, FFT_INVERSE);  
  
  
}






	

	
