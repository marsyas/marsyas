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
    \class PowerSpectrum
    \brief PowerSpectrum computes the magnitude/power of the complex spectrum 

    Computes the magnitude/power of a complex spectrum 
*/

#include "PowerSpectrum.h"
using namespace std;


PowerSpectrum::PowerSpectrum():MarSystem()
{
  type_ = "PowerSpectrum";
}


PowerSpectrum::PowerSpectrum(string name)
{
  type_ = "PowerSpectrum";
  name_ = name;
  addControls();
}

PowerSpectrum::~PowerSpectrum()
{
}

void
PowerSpectrum::addControls()
{
  addDefaultControls();
  addctrl("string/spectrumType", "power");
  setctrlState("string/spectrumType", true);
}

MarSystem* 
PowerSpectrum::clone() const 
{
  return new PowerSpectrum(*this);
}


void 
PowerSpectrum::update()
{
  setctrl("natural/onSamples", (natural)1);
  // setctrl("natural/onObservations", (getctrl("natural/inObservations").toNatural() / 2) + 1);
  setctrl("natural/onObservations", (getctrl("natural/inObservations").toNatural() / 2));
  setctrl("real/osrate", getctrl("real/israte"));
  
  setctrl("string/onObsNames", getctrl("string/inObsNames"));  

  stype_ = getctrl("string/spectrumType").toString();
  if (stype_ == "power")
    ntype_ = PSD_POWER;
  else if (stype_ == "magnitude") 
    ntype_ = PSD_MAG;
  else if (stype_ == "decibels")
    ntype_ = PSD_DB;
  else if (stype_ == "powerdensity")
    ntype_ = PSD_PD;
  
  
  
  defaultUpdate();
  N2_ = inObservations_ / 2;
  ostringstream oss;
  
  for (natural n=0; n < N2_; n++)
    oss << "mbin_" << n << ",";
  setctrl("string/onObsNames", oss.str());

}



void 
PowerSpectrum::process(realvec& in, realvec& out)
{
  checkFlow(in,out);
  
  
  for (t=0; t < N2_; t++)
    {
      if (t==0)
	{
	  re_ = in(0,0);
	  im_ = 0.0;
	}
      else if (t == N2_) 
	{
	  re_ = in(1, 0);
	  im_ = 0.0;
	}
      else
	{
	  re_ = in(2*t, 0);
	  im_ = in(2*t+1, 0);
	}
      
      switch(ntype_)
	{
	case PSD_POWER:
	  out(t, 0) = re_*re_ + im_*im_;	  
	  break;
	case PSD_MAG:
	  out(t,0) = sqrt(re_ * re_ + im_ * im_);
	  break;
	case PSD_DB:
	  dB_ = (real)(20 * log10(re_ * re_ + im_ * im_ + 0.000000001));
	  if (dB_ < -100) dB_ = -100;
	  out(t,0) = dB_;	  
	  break;
	case PSD_PD:
	  pwr_ = re_ * re_ + im_ * im_;	  
	  out(t,0) = (real)(2.0 * pwr_) / N2_;	  
	  break;
	}
    }
  
}









	

	
	
