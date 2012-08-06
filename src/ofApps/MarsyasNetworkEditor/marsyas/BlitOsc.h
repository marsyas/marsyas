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

#ifndef MARSYAS_BLITOSC_H
#define MARSYAS_BLITOSC_H

#include "MarSystem.h"

namespace Marsyas
{
/**
	\ingroup Processing Basic Certified
	\brief Multiply input realvec with a fixed value.

	Multiply all the values of the input realvec with
mrs_real/gain and put them in the output vector.

	Controls:
	- \b mrs_real/gain [w] : adjust the gain multiplier.
*/


class BlitOsc: public MarSystem
{
private:
	//Add specific controls needed by this MarSystem.
	void addControls();
	void myUpdate(MarControlPtr sender);

	mrs_real frequency_;
	mrs_real israte_;
	mrs_bool noteon_;
	mrs_natural phase_;
	mrs_natural N_;
	mrs_real delay_;
	mrs_real dc_;

	// All Pass Delays + Coefficients
	mrs_real ax1_;
	mrs_real ax2_;
	mrs_real ay1_;
	mrs_real ay2_;
	mrs_real a1_;
	mrs_real a2_;

	mrs_real allPass(mrs_real x);

	// Leaky Integrator Delays + Coefficients
	mrs_real ly1_;
	mrs_real le_;

	mrs_real leakyIntegrator(mrs_real x);

public:
	BlitOsc(std::string name);
	BlitOsc(const BlitOsc& a);
	~BlitOsc();
	MarSystem* clone() const;

	void myProcess(realvec& in, realvec& out);
};

}//namespace Marsyas

#endif
