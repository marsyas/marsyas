/*
** Copyright (C) 1998-2010 George Tzanetakis <gtzan@cs.uvic.ca>
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

#ifndef MARSYAS_MEMORY_H
#define MARSYAS_MEMORY_H

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
	\ingroup Processing Basic
	\brief Collects and outputs N past input samples.

	Memory is essentially a circular buffer that collects and outputs past input samples.
	It is used to compute "texture" features (or dynamic features) - which are stastics of
	short-term features (typically around 10-20 milliseconds) over a larger window -
	typically around 1 second.

	Controls:
	- \b mrs_natural/memSize [rw] : memory size - a ratio of output samples to input samples
	(amount of output samples will be memSize times amount of input samples).
	- \b mrs_bool/reset [rw] : clear and reset the memory buffer
*/


class Memory: public MarSystem
{
private:
  void addControls();
  void myUpdate(MarControlPtr sender);

  mrs_natural end_;
  MarControlPtr ctrl_reset_;
  MarControlPtr ctrl_memSize_;
  mrs_natural counter_since_reset_;
  mrs_realvec cir_out_;

public:
  Memory(std::string name);
  Memory(const Memory& a);
  ~Memory();
  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);

};

}//namespace Marsyas

#endif




