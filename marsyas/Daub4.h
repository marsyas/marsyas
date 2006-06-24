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
    \class Daub4
    \brief Daubechies4 WaveletStep

   Applies the Daubechies 4-coefficient wavelet filter as 
a WaveletStep for the WaveletPyramid algorithm. The code is 
is based on the Numerical Recipies wavelet code. 
*/

#ifndef MARSYAS_DAUB4_H 
#define MARSYAS_DAUB4_H

#include "MarSystem.h"	
#include "WaveletStep.h"

namespace Marsyas
{

class Daub4: public MarSystem
{
private:
  realvec workspace_;
  mrs_real c0_;
  mrs_real c1_;
  mrs_real c2_;
  mrs_real c3_;

  
public:
  Daub4(std::string name);
  ~Daub4();
  MarSystem* clone() const;
  void addControls();
  void update();
  void process(realvec& in, realvec& out);
  
};

}//namespace Marsyas

#endif
