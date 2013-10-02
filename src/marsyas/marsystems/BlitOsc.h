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

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
	\ingroup Synthesis
	\brief A BLIT oscillator

	An implementation of a bandlimited impulse train oscillator.

	This implementation relies on a second order all pass filter
	as a fractional delay. The delay reduces some aliasing caused
	by the impulse train.

	TODO: Add triangle shape
	TODO: Add ability to shift the pitch via one of the input channels

	Controls:
	- \b mrs_real/frequency [w] : The desired frequency for the oscillator.
	- \b mrs_natural/type [w] : The type of oscillator (saw = 0, square = 1)
	- \b mrs_bool/noteon [w] : Not yet implemented
*/


class BlitOsc: public MarSystem
{
private:

  class LeakyIntegrator
  {
  private:
    mrs_real y1;
    mrs_real le;
  public:
    LeakyIntegrator() : le(0.005) { }

    void set_le_amount(mrs_real l)
    {
      le = l;
    }

    mrs_real operator()(mrs_real x)
    {
      mrs_real y = x + ((1 - le) * y1);
      y1 = y;
      return y;
    }
  };

  class Allpass
  {
  private:
    mrs_real x1;
    mrs_real x2;
    mrs_real y1;
    mrs_real y2;
    mrs_real a1;
    mrs_real a2;
    mrs_real d;
  public:

    void set_delay(mrs_real d)
    {
      a1 = -2 * ((d - 2) / (d + 1));
      a2 = ((d - 1)*(d - 2))/((d + 1)*(d + 2));
      x1 = 0;
      x2 = 0;
      y1 = 0;
      y2 = 0;
    }

    mrs_real operator()(mrs_real x)
    {
      mrs_real y = (a2 * x) + (a1 * x1) + (x2) - (a1 * y1) - (a2 * y2);
      x2 = x1;
      x1 = x;
      y2 = y1;
      y1 = y;
      return y;
    }
  };

  //Add specific controls needed by this MarSystem.
  void addControls();
  void myUpdate(MarControlPtr sender);

  mrs_real frequency_; // the oscillator frequency
  mrs_real israte_;    // sample rate of the system
  mrs_bool noteon_;    // is our note on?
  mrs_natural phase_;  // the current phase
  mrs_natural N_;      // the maximum of the phase counter
  mrs_real delay_;     // The delay of the Allpass filter
  mrs_real dc_;        // The DC offset for the saw form
  mrs_real inv_;       // Used to create a bipolar impulse train for square form
  mrs_natural type_;   // The oscillator type

  mrs_real frac_;

  Allpass ap1; // The Tuning filter
  Allpass ap2; // The antialiasing filter
  LeakyIntegrator le;

public:
  BlitOsc(std::string name);
  BlitOsc(const BlitOsc& a);
  ~BlitOsc();
  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);
};

}//namespace Marsyas

#endif
