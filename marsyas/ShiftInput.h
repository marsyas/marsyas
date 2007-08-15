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

#ifndef MARSYAS_SHIFTINPUT_H
#define MARSYAS_SHIFTINPUT_H

#include "MarSystem.h"	
 
namespace Marsyas
{
/** 
    \class ShiftInput
    \ingroup Processing Basic
    \brief ShiftInput (hopSize, winSize)

    The ShiftInput MarSystem is used for performing efficient overlap
input from a source. The hop size between the overlapped windows is
controlled by the inSamples control which needs to be set by the <b>
preceding </b> source or MarSystem. The output of the MarSystem is
the overlapped windows with size adjusted by the WindowSize control.
 
	Controls:
	- \b mrs_natural/WindowSize [rw] : The window size of the overlapped windows
	- \b mrs_bool/reset [??] : [DOCME]
*/


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

	
