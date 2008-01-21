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

#ifndef MARSYAS_FLUX2_H
#define MARSYAS_FLUX2_H

#include "MarSystem.h"	

namespace Marsyas
{
/** 
    \class Flux2
    \ingroup Analysis
    \brief Flux2 calculates the flux between the current and prev. vector,
	difference with Flux.h stands in the use of the L1 norm and linear magnitude (NB: the 
	PowerSpectrum spectrum type must be "magnitude"), there is also no normalization with max_ anymore

*/


  class Flux2: public MarSystem
  {
  private: 
    void myUpdate(MarControlPtr sender);
    
    realvec prevWindow_;
    mrs_real diff_, flux_, max_;
    mrs_real tmp_;
	//mrs_real tmp2_;
  public:
    Flux2(std::string name);
  
    ~Flux2();
    MarSystem* clone() const;  
  
    void myProcess(realvec& in, realvec& out);
  };

}//namespace Marsyas

#endif

	

	

	
