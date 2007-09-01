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

#ifndef MARSYAS_ACCUMULATOR_H
#define MARSYAS_ACCUMULATOR_H

#include "MarSystem.h"	

namespace Marsyas
{
/** 
	\class Accumulator 
	\ingroup Composites
	\brief Accumulate result of multiple ticks

Accumulate result of multiple tick process calls 
to internal MarSystem. Spit output only once when
all the results are accumulated. Used to change 
the rate of process requests.

For example, if \c nTimes is 5, then each time the Accumulator recieves
a tick(), it sends 5 tick()s to the MarSystems that are inside it.

Controls:
- \b mrs_natural/nTimes [rw] : the multiplier of ticks() for the internal
  MarSystems.
*/


class Accumulator: public MarSystem
{
private: 
	mrs_natural nTimes_;
	MarControlPtr ctrl_nTimes_;
	realvec tout_;

	void addControls();
	void myUpdate(MarControlPtr sender);
  
public:
	Accumulator(std::string name);
	Accumulator(const Accumulator& a);
  ~Accumulator();
  MarSystem* clone() const;  

  void myProcess(realvec& in, realvec& out);
};

}//namespace Marsyas

#endif
