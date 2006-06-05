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

#ifndef MARSYAS_POWERSPECTRUM_H
#define MARSYAS_POWERSPECTRUM_H

#include "MarSystem.h"	
#include "fft.h"



#define PSD_POWER 1
#define PSD_MAG 2
#define PSD_DB  3
#define PSD_PD  4 


class PowerSpectrum: public MarSystem
{
private:
  void addControls();
  
  std::string stype_;
  natural ntype_;

  natural N2_;
  real re_, im_;
  real dB_, pwr_;
  

public:
  PowerSpectrum();
  PowerSpectrum(std::string name);
  ~PowerSpectrum();
  MarSystem* clone() const;

  void update();
  void process(realvec& in, realvec& out);
  
};

#endif

	

	
