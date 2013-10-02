/*
** Copyright (C) 2010 Stefaan Lippens
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

#ifndef MARSYAS_DELAYSAMPLES_H
#define MARSYAS_DELAYSAMPLES_H

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
	\ingroup Processing
	\brief Simple MarSystem that shifts samples back in time.

	This is MarSystem is similar to
	- Delay, which can be overkill because it interpolates
	  between delays when they delay is changed.
	- Shifter, which is lame for time shifts because it does not work across
	  slices and shortens the windows size.

	This MarSystem works across slices and with
	delays larger than the window size.

	Controls:
	- \b mrs_natural/delay [w] : sets the delay in samples
*/

class marsyas_EXPORT DelaySamples: public MarSystem
{
private:

  /// Add specific controls needed by this MarSystem.
  void addControls();

  /// Reads changed controls and sets up variables if necessary.
  void myUpdate(MarControlPtr sender);


  /// MarControlPtr for the gain control
  MarControlPtr ctrl_delay_;

  /// Cache of the delay.
  mrs_natural delay_;

  /// Buffers of previous samples.
  realvec memory_;

public:
  /// DelaySamples constructor.
  DelaySamples(std::string name);

  /// DelaySamples copy constructor.
  DelaySamples(const DelaySamples& a);

  /// DelaySamples destructor.
  ~DelaySamples();

  /// Implementation of the MarSystem::clone() method.
  MarSystem* clone() const;

  /// Implementation of the MarSystem::myProcess method.
  void myProcess(realvec& in, realvec& out);
};

}
//namespace Marsyas

#endif
//MARSYAS_DELAYSAMPLES_H

