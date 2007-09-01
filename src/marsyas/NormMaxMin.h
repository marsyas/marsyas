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

#ifndef MARSYAS_NORMMINMAX_H
#define MARSYAS_NORMMINMAX_H

#include "MarSystem.h"	

#include <cfloat>

namespace Marsyas
{
/** 
\class NormMaxMin
\ingroup Processing
\brief Normalize my mapping min/max range to user specified range


*/


class NormMaxMin: public MarSystem
{
private: 
  mrs_real lower_;
  mrs_real upper_;
  mrs_real range_;
  bool train_;
  bool init_;
  mrs_string mode_;
  mrs_string prev_mode_;	
  
  void addControls();
  void myUpdate(MarControlPtr sender);

  realvec maximums_;
  realvec minimums_;

  MarControlPtr lowerPtr_;
  MarControlPtr upperPtr_;
  MarControlPtr trainPtr_;
  MarControlPtr initPtr_;
  MarControlPtr maximumsPtr_;
  MarControlPtr minimumsPtr_;
  
  
public:
  NormMaxMin(std::string name);
  NormMaxMin(const NormMaxMin&);
  ~NormMaxMin();
  MarSystem* clone() const;  
  
  void myProcess(realvec& in, realvec& out);
};

}//namespace Marsyas

#endif
	
