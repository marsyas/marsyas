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
    \class Fanout
    \brief Fanout composite of MarSystem objects
    
    Combines a parallel set of MarSystem objects to a single MarSystem 
corresponding to executing the System objects in parallel and 
make each of their outputs a separate observation
*/

#ifndef MARSYAS_FANOUT_H
#define MARSYAS_FANOUT_H

#include "MarSystem.h"

namespace Marsyas
{

class Fanout: public MarSystem
{
private:
  void addControls();
	void myUpdate(MarControlPtr sender);
  void deleteSlices();  

  realvec enabled_;
  realvec localIndices_;
  
  bool probe_;
  mrs_natural disable_;
	mrs_natural enable_;
  
public:
  Fanout(std::string name);
  ~Fanout();
  MarSystem* clone() const;    
 
  void myProcess(realvec& in, realvec& out);
};

}//namespace Marsyas

#endif

	

	
