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
    \class Memory
    \brief Memory output the past memSize input observations. 

    Memory is essentially a circular buffer that holds 
past observations. It used to compute "texture" features 
-or dynamic features- which are stastics of short-term features 
-typically around 10-20 milliseconds- over a larger 
window -around 1 second-.
*/

#ifndef MARSYAS_MEMORY_H
#define MARSYAS_MEMORY_H

#include "MarSystem.h"	



class Memory: public MarSystem
{
private: 
  void addControls();
  natural end_;
  bool reset_;

public:
  Memory();
  Memory(std::string name);
  ~Memory();
  MarSystem* clone() const;  
  
  void update();
  void process(realvec& in, realvec& out);
  

};

#endif

	

	
