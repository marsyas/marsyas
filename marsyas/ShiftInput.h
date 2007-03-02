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
    \class ShiftInput
    \brief Shift next D samples  

    Shift next D samples from the input source to the output source 
into the right-hand end of the outputSlice. 
with gain and put them in the output vector. 
*/

#ifndef MARSYAS_SHIFTINPUT_H
#define MARSYAS_SHIFTINPUT_H

#include "MarSystem.h"	

namespace Marsyas
{

class ShiftInput: public MarSystem
{
private:
  realvec pout_;
  void addControls();
  void myUpdate(MarControlPtr sender);
  
  mrs_natural N_, D_, W_, PW_;
  bool reset_;
  
  MarControlPtr ctrl_reset_;
  
public:
  ShiftInput(std::string name);
  ShiftInput(const ShiftInput& a);
  ~ShiftInput();
  MarSystem* clone() const;    

  void myProcess(realvec& in, realvec& out);
  
};

}//namespace Marsyas

#endif

	
