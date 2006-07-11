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
    \class PvUnconvert
    \brief PvUnconvert

    PvUnconvert N real and imaginary spectrum values to 
N/2+1 pairs of magnitude and phase values. The phases 
are unwrapped and successive phase differences are 
used to compute estimates of the instantaneous frequencies 
for each phase vocoder analysis channel; decimation rate 
and sampling rate are used to render these frequencies 
directly in Hz. 
*/

#ifndef MARSYAS_PVUNCONVERT_H
#define MARSYAS_PVUNCONVERT_H

#include "MarSystem.h"	

namespace Marsyas
{

class PvUnconvert: public MarSystem
{
private:
  realvec lastphase_;
  mrs_real fundamental_;
  mrs_real factor_;
  mrs_natural N2_;
  
  void addControls();
	void localUpdate();
  
public:
  PvUnconvert(std::string name);

  ~PvUnconvert();
  MarSystem* clone() const;    

  void process(realvec& in, realvec& out);
  
};

}//namespace Marsyas

#endif

	

	

	

	
