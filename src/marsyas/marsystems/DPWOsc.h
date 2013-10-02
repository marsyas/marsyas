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

#ifndef MARSYAS_DPWOSC_H
#define MARSYAS_DPWOSC_H

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
	\class DPWOsc
	\ingroup Synthesis
	\brief

	Controls:
	- \b mrs_real/frequency [w] : Sets the fundental frequency for the oscillator.
	- \b mrs_real/cyclicrate [w] : Sets the cyclic rate of the PWM waveform
	- \b mrs_bool/cyclicin [w] : If true the input realvec(1,t) will control the cyclic rate
	- \b mrs_natural/type [w] : Sets the oscillator type. (saw = 0, PWM = 1)
*/

class DPWOsc: public MarSystem
{
private:

  class Differentiator
  {
  private:
    mrs_real x1;
  public:

    mrs_real operator()(mrs_real x)
    {
      mrs_real y = x - x1;
      x1 = x;
      return y;
    }
  };

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

  mrs_real c_; // Amplitude scalar

  Differentiator df;

public:
  // DPWOsc constructor.
  DPWOsc(std::string name);

  // DPWOsc copy constructor.
  DPWOsc(const DPWOsc& a);

  // DPWOsc destructor.
  ~DPWOsc();

  // Implementation of the MarSystem::clone() method.
  MarSystem* clone() const;

  // Implementation of the MarSystem::myProcess method.
  void myProcess(realvec& in, realvec& out);
};

}
//namespace Marsyas

#endif
//MARSYAS_DPWOSC_H

