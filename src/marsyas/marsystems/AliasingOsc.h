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

#ifndef MARSYAS_MARSYSTEMTEMPLATEBASIC_H
#define MARSYAS_MARSYSTEMTEMPLATEBASIC_H

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
	\class AliasingOsc
	\ingroup Synthesis
	\brief A simple aliasing oscillator

	A simple aliasing oscillator capable of Saw and PWM.

	The pitch can be modulated by using in(0,t).

	The cyclic rate can be modulated by using in(1,t). If cyclicin is
	set to true.

	TODO: Implement triangle wave.
	TODO: Make it so that there is control over what in channel is
	      mapped to what parameter is getting modulated.

	Controls:
	- \b mrs_real/frequency [w] : Sets the fundental frequency for the oscillator.
	- \b mrs_real/cyclicrate [w] : Sets the cyclic rate of the PWM waveform
	- \b mrs_bool/cyclicin [w] : If true the input realvec(1,t) will control the cyclic rate
	- \b mrs_natural/type [w] : Sets the oscillator type. (saw = 0, PWM = 1)
*/

class AliasingOsc: public MarSystem
{
private:
  // Add specific controls needed by this MarSystem.
  void addControls();

  // Reads changed controls and sets up variables if necessary.
  void myUpdate(MarControlPtr sender);

  mrs_real currentValue_;
  mrs_real incr_;
  mrs_real cyclicRate_;
  mrs_bool cyclicIn_;
  mrs_real israte_;
  mrs_real frequency_;
  mrs_natural type_;

public:
  // AliasingOsc constructor.
  AliasingOsc(std::string name);

  // AliasingOsc copy constructor.
  AliasingOsc(const AliasingOsc& a);

  // AliasingOsc destructor.
  ~AliasingOsc();

  // Implementation of the MarSystem::clone() method.
  MarSystem* clone() const;

  // Implementation of the MarSystem::myProcess method.
  void myProcess(realvec& in, realvec& out);
};

}
//namespace Marsyas

#endif
//MARSYAS_MARSYSTEMTEMPLATEBASIC_H

