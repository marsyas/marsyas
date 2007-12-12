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

#ifndef MARSYAS_MCAULAYQUATIERI_H
#define MARSYAS_MCAULAYQUATIERI_H

#include "MarSystem.h"

namespace Marsyas
{
/**
	\class McAulayQuatieri
	\ingroup MarSystem
	\brief Performs McAulay-Quatieri frame-to-frame peak matching

	This MarSystem takes as input a peakView realvec with peak information, and performs McAulay-Quatieri
	frame-to-frame peak matching (i.e. peak continuation), as described in the following paper:

	R. McAulay and T. Quatieri, "Speech analysis/Synthesis based on a sinusoidal representation,"
	IEEE Transactions on Acoustics, Speech, and Signal Processing vol. 34, pp. 744-754, August 1986.

	Controls:
	- \b mrs_bool/reset [w]: resets internal memory.
	- \b mrs_bool/useMemory [w] : when true, uses the last frame of the previous input for continuing peak tracking.
	- \b mrs_bool/useGroups [w] : take into consideration the assigned groups (i.e. clusters from NCut) in the input peaks.
*/

class McAulayQuatieri: public MarSystem
{
private:
	void addControls();
	void myUpdate(MarControlPtr sender);

	MarControlPtr ctrl_useMemory_;
	MarControlPtr ctrl_reset_;
	MarControlPtr ctrl_useGroups_;
	MarControlPtr ctrl_delta_;

public:
	McAulayQuatieri(std::string name);
	McAulayQuatieri(const McAulayQuatieri& a);
	~McAulayQuatieri();
	MarSystem* clone() const;

	void myProcess(realvec& in, realvec& out);
};

}

#endif

