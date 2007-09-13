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

#ifndef MARSYAS_SPECTRUM2CHROMA_H
#define MARSYAS_SPECTRUM2CHROMA_H

#include "MarSystem.h"

namespace Marsyas
{
/**
	\class Spectrum2Chroma
	\ingroup Special
	\brief Basic example on how to use controls efficiently in MarSystems

	This example is the same as Gain; it scales the output by
multiplying each sample with a real number.

	Controls:
	- \b mrs_real/gain [w] : sets the gain multiplier.
	- \b mrs_bool/dummy [rw] : does nothing.
*/

class Spectrum2Chroma: public MarSystem
{
private:
	realvec chromaMap_;

	MarControlPtr ctrl_nbins_;
	MarControlPtr ctrl_middleAfreq_;
	MarControlPtr ctrl_weightCenterFreq_;
	MarControlPtr ctrl_weightStdDev_;

	mrs_natural pnbins_;
	mrs_real pmiddleAfreq_;
	mrs_real pweightCenterFreq_;
	mrs_real pweightStdDev_;

	void addControls();
	void myUpdate(MarControlPtr sender);

public:
	Spectrum2Chroma(std::string name);
	Spectrum2Chroma(const Spectrum2Chroma& a);
	~Spectrum2Chroma();
	MarSystem* clone() const;

	void myProcess(realvec& in, realvec& out);
};

}

#endif

