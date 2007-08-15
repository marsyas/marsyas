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

#ifndef MARSYAS_POWERSPECTRUM_H
#define MARSYAS_POWERSPECTRUM_H

#include "MarSystem.h"	
#include "fft.h"

namespace Marsyas
{
/** 
    \class PowerSpectrum
	\ingroup Analysis
    \brief PowerSpectrum computes the magnitude/power of the complex spectrum 

    Computes the magnitude/power of a complex spectrum 
*/


class PowerSpectrum: public MarSystem
{
private:
  std::string stype_;
  
  mrs_natural ntype_;
  mrs_natural N2_;
  mrs_real re_, im_;
  mrs_real dB_, pwr_;
  MarControlPtr ctrl_spectrumType_;
  

  void addControls();
  void myUpdate(MarControlPtr sender);
  


public:
  PowerSpectrum(std::string name);
  PowerSpectrum(const PowerSpectrum&);
  ~PowerSpectrum();
  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);
  
};

}//namespace Marsyas

#endif

	

	
