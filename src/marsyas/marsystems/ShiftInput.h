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

#ifndef MARSYAS_SHIFTINPUT_H
#define MARSYAS_SHIFTINPUT_H

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
\class ShiftInput
\ingroup Processing Basic
\brief Apply sliding window with certain hop size and window size.

The ShiftInput MarSystem is useful for generating slices from an
input source in a sliding window fashion with certain window size
and hop size.

The **hop size** between the overlapping windows is defined by the number of
input samples to the ShiftInput.

The **window size** can be set with the *winSize* control.

If the hop size is smaller than the window size, there will be
overlap between successive output slices. If the hop size is
larger than the window size, the output slices will be trimmed
versions of the input slices.

A typical example is a SoundFileSource followed by a ShiftInput, in a Series.
Mind that he number of input samples to a MarSystem is automatically determined
from the number of output samples of the **preceding** MarSystem. Hence, to
define the hop size of the sliding window produced by the ShiftInput, one has
to make sure that the *onSamples* control of the SoundFileSource equals the
desired hop size. Because each MarSystem automatically determines its output
number of samples from it's input number of samples, that in turn requires
setting the *inSamples* control of the SoundFileSource.

Controls:
- \b mrs_natural/winSize [rw] : the window size of the sliding window.
- \b mrs_bool/reset [w] : reset the internal buffer (used in case of
    overlapping windows) to zero.
- \b mrs_bool/clean [w] : call for a partial clean of the internal buffer.
    (limited by lowCleanLimit and highCleanLimit).
- \b mrs_real/lowCleanLimit [rw] : low limit of a cleaning request
    (as a portion of the internal buffer size).
- \b mrs_real/highCleanLimit [rw] : high limit of a cleaning request
    (as a portion of the internal buffer size).

\see SoundFileSource

\see SoundFileSourceHopper for generating an audio slices from
a SoundFileSource in a sliding/hopping window fashion.
*/


class ShiftInput: public MarSystem
{
private:
  /// Internal cache for the window size value.
  mrs_natural winSize_;

  /// Internal cache for the hop size value.
  mrs_natural hopSize_;

  /// Internal sample buffer.
  realvec outSavedData_;

  /// Internal pointer to the reset MarControl.
  MarControlPtr ctrl_reset_;

  /// Internal pointer to the winSize MarControl.
  MarControlPtr ctrl_winSize_;

  /// Internal pointer to the clean MarControl.
  MarControlPtr ctrl_clean_;

  /// Internal pointer to the lowCleanLimit MarControl.
  MarControlPtr ctrl_lowCleanLimit_;

  /// Internal pointer to the highCleanLimit MarControl.
  MarControlPtr ctrl_highCleanLimit_;

  void addControls();
  void myUpdate(MarControlPtr sender);

public:
  ShiftInput(std::string name);
  ShiftInput(const ShiftInput& a);
  ~ShiftInput();
  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);

};

}//namespace Marsyas

#endif


