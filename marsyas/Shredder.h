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
** MERCHANTABILITY or FITNAESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
** 
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software 
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

/** 
    \class Shredder 

    \brief Split the output of an accumulator

    Split the result an accumulator. Spit several outputs when
    the input is recieved. Used to restore
    the rate of process requests initially change by an accumulator
*/

	
#ifndef MARSYAS_SHREDDER_H
#define MARSYAS_SHREDDER_H

#include "MarSystem.h"	

namespace Marsyas
{

class Shredder: public MarSystem
{
private: 
	mrs_natural nTimes_;
	realvec tin_;

	void addControls();
	void myUpdate(MarControlPtr sender);
	bool updControl(MarControlPtr control, MarControlPtr newcontrol, bool upd=true);
  
public:
	Shredder(std::string name);
  ~Shredder();
  MarSystem* clone() const; 

  void myProcess(realvec& in, realvec& out);
};

}//namespace Marsyas

#endif
