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

InvSpectrum::InvSpectrum():MarSystem()
{
  type_ = "InvSpectrum";
}


InvSpectrum::InvSpectrum(string name)
{
  type_ = "InvSpectrum";
  name_ = name;
  addControls();
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
InvSpectrum::addControls()
{
  addDefaultControls();
}


void 
InvSpectrum::update()
{
  setctrl("natural/onSamples", getctrl("natural/inObservations"));
  setctrl("natural/onObservations", (natural)1);
  setctrl("real/osrate", getctrl("real/israte").toReal() * getctrl("natural/inObservations").toNatural());
  
  tempVec_.create(getctrl("natural/onSamples").toNatural());
  
  defaultUpdate();
}

void 
InvSpectrum::process(realvec& in, realvec& out)
{
  checkFlow(in,out);
  
  for (t=0; t < onSamples_; t++)
    out(0,t) = in(t,0);

  real *tmp = out.getData();
  myfft_.rfft(tmp, onSamples_/2, FFT_INVERSE);  
  
  
}






	

	
