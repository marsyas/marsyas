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

#ifndef MARSYAS_MCAULEYQUATIERI_H
#define MARSYAS_MCAULEYQUATIERI_H

#include "MarSystem.h"

namespace Marsyas
{
/**
	\class McAuleyQuatieri
	\ingroup Special
	\brief Basic example on how to use controls efficiently in MarSystems

	This example is the same as Gain; it scales the output by
multiplying each sample with a real number.

	Controls:
	- \b mrs_bool/reset [w]: resets internal memory.
	- \b mrs_bool/useMemory [w] : when true, uses the last frame of the previous input for continuing peak tracking.
	- \b mrs_bool/useGroups [w] : take into consideration the assigned groups (i.e. clusters from NCut) in the input peaks.
*/

class McAuleyQuatieri: public MarSystem
{
private:
	void addControls();
	void myUpdate(MarControlPtr sender);

	MarControlPtr ctrl_useMemory_;
	MarControlPtr ctrl_reset_;
	MarControlPtr ctrl_useGroups_;
	MarControlPtr ctrl_delta_;

public:
	McAuleyQuatieri(std::string name);
	McAuleyQuatieri(const McAuleyQuatieri& a);
	~McAuleyQuatieri();
	MarSystem* clone() const;

	void myProcess(realvec& in, realvec& out);
};

}

#endif

