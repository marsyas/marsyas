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

#ifndef MARSYAS_APDELAYOSC_H
#define MARSYAS_APDELAYOSC_H

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
	\class APDelayOsc
	\ingroup Synthesis
	\brief A non-aliasing analog oscillator algorithm

	This is a non-aliasing virtual analog oscillator algorithm.

	For the saw algorithm the output of the fractional delay is fed back into
	the delay line. This creates a perceptually harmonic spectrum that
	approximates having all the harmonics at equal power up to the Nyquist
	frequency.  The saw algorithm ends up having a considerable DC offset, that
	is removed by subtracting frequency/Samplerate from each sample. Finally
	the leaky integrator is used to apply an exponential decay to the frequency
	spectrum.

	The saw wave is generated in a similar way, but we don't need to worry
	about any DC offset. The only real difference is the square algorithm is
	that we negate the samples as we feed them back into the delay line. This
	will generate a spectrum containing the even harmonics. The one other side
	effect of this as that we double our period, so we must shorten the delay
	line to compensate.

	TODO: Add triangle wave

	TODO: Add ability to modulate pitch.
	          This could be done using two read pointers
			  and a cross fader.
			  It could also be done by changing the delay time
			  at the end of each cycle.
	Controls:
	- \b mrs_real/frequency [w] : sets the frequency of the wave.
	- \b mrs_natural/type [w] : sets the oscillator type. (saw = 0, square = 1).
	- \b mrs_bool/noteon [w] : turns on the oscillator
*/

class APDelayOsc: public MarSystem
{
private:
  // Add specific controls needed by this MarSystem.
  void addControls();

  // Reads changed controls and sets up variables if necessary.
  void myUpdate(MarControlPtr sender);

  class FirstOrderAllPass
  {
  private:
    mrs_real y, y1;
    mrs_real x, x1;
    mrs_real a, d;

  public:
    void delay(mrs_real din)
    {
      d = din;
      a = (1 - d)/(1 + d);
      y1 = x1 = 0;
    }

    mrs_real get_delay()
    {
      return d;
    }

    mrs_real operator()(mrs_real x)
    {
      y = (a * x) + (x1 - (a * y1));
      x1 = x;
      y1 = y;
      return y;
    }
  };

  class LeakyIntegrator
  {
  private:
    mrs_real y, y1;
    mrs_real x;
    mrs_real e;

  public:
    LeakyIntegrator(): e(0.003) {}

    void leaky(mrs_real amount)
    {
      e = amount;
    }

    mrs_real operator()(mrs_real x)
    {
      y = x + ((1 - e) * y1);
      y1 = y;
      return y;
    }
  };

  class DCBlocker
  {
  private:
    mrs_real y, y1;
    mrs_real x, x1;
    mrs_real R;

  public:
    DCBlocker(): R(0.995) {}

    mrs_real operator()(mrs_real x)
    {
      y = x - x1 + (R * y1);
      y1 = y;
      x1 = x;
      return y;
    }
  };

  mrs_real frequency_;

  mrs_natural delaylineSize_;
  realvec delayline_;

  mrs_real dc_;
  mrs_real frac_;

  mrs_real israte_; // Sample rate of the system
  mrs_real dcoff_;  // The precalculated DC offset
  mrs_real neg_;    // Used to invert the system if
  // only even harmonics are wanted

  FirstOrderAllPass ap1;
  FirstOrderAllPass ap2; // The tuning filter
  DCBlocker dcb;
  LeakyIntegrator le1;

  mrs_natural wp_;   // Write Pointer
  mrs_natural rp_;   // Read pointer one
  mrs_natural rpp_;  // Read pointer two
  mrs_natural N_;    // The delayline length for our current pitch
  mrs_natural type_; // The current type of the oscillator

  mrs_bool noteon_;

  mrs_real allPass(mrs_real x);
  mrs_real leakyIntegrator(mrs_real x);
  mrs_real dcBlocker(mrs_real x);

public:
  // APDelayOsc constructor.
  APDelayOsc(std::string name);

  // APDelayOsc copy constructor.
  APDelayOsc(const APDelayOsc& a);

  // APDelayOsc destructor.
  ~APDelayOsc();

  // Implementation of the MarSystem::clone() method.
  MarSystem* clone() const;

  // Implementation of the MarSystem::myProcess method.
  void myProcess(realvec& in, realvec& out);
};

}
//namespace Marsyas

#endif
//MARSYAS_MARSYSTEMTEMPLATEBASIC_H

