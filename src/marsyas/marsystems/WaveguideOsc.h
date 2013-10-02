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

#ifndef MARSYAS_WAVEGUIDEOSC_H
#define MARSYAS_WAVEGUIDEOSC_H

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
	\ingroup Synthesis
	\brief Generates a sine wave.

	Creates a sine wave of a chosen frequency using a wave
	guide.

	The frequency can be modulated by using in(0,t). This
	currently doesn't work at audio rates because there is
	amplitude scaling cause when the coefficient k is changed.

	A scaling value could be calculated to compensate for the
	amplitude change.

	Controls:
	- \b mrs_real/frequency [w] : the sinewave's frequency in hz.
	- \b mrs_bool/noteon [w] : currently not implemented.
*/


class WaveguideOsc: public MarSystem
{
private:

  mrs_real x1n1_;  // Last x1
  mrs_real x2n1_;  // Last x2

  mrs_real israte_; // Sampling rate of the system

  mrs_real k_;      // filter coefficient

  mrs_real frequency_;
  //Add specific controls needed by this MarSystem.
  void addControls();
  void myUpdate(MarControlPtr sender);

public:
  WaveguideOsc(std::string name);
  WaveguideOsc(const WaveguideOsc& a);
  ~WaveguideOsc();
  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);
};

}//namespace Marsyas

#endif
