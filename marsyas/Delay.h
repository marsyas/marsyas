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
    \class Delay
    \brief Delay

   Simple Delay example.
*/

#ifndef MARSYAS_DELAY_H
#define MARSYAS_DELAY_H

#include "MarSystem.h"	

namespace Marsyas
{

class Delay: public MarSystem
{
private: 
  void addControls();
	realvec buffer_;
	mrs_real delay_;
	mrs_real gain;
	mrs_real feedback;
	mrs_real delay;
	int i_;
	mrs_natural cursor_;
	mrs_natural bufferSize_;
	
public:
  Delay(std::string name);
  ~Delay();
  MarSystem* clone() const;  
  
  void update();
  void process(realvec& in, realvec& out);
};

}//namespace Marsyas

#endif
