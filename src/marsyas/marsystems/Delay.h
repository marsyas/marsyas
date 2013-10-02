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

#ifndef MARSYAS_DELAY_H
#define MARSYAS_DELAY_H

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
    \class Delay
	\ingroup Processing Basic
    \brief Delay

	Simple Delay example; in the first block, it will adapt the delay from 0 to the actual delay

	Controls:
	- \b mrs_real/maxDelaySamples [w] : maximum delay in samples.
	- \b mrs_real/maxDelaySeconds [w] : maximum delay in seconds.
	- \b mrs_real/delaySamples [w] : delay in samples.
	- \b mrs_realvec/delaySamples [w] : multiple delays in samples.
	- \b mrs_real/delaySeconds [w] : delay in seconds.
	- \b mrs_realvec/delaySeconds [w] : multiple delays in seconds.

*/


class Delay: public MarSystem
{
private:
  void addControls();
  void myUpdate(MarControlPtr sender);

  // required for allocating the delay buffer
  mrs_natural	nextPowOfTwo (mrs_natural value);
  // get the increment per sample for the delay control value
  static void getLinearInterPInc (const mrs_realvec startVal, const mrs_realvec stopVal,
                                  mrs_realvec &incVal, const mrs_natural numSamples);

  // conversion functions
  mrs_real samples2Seconds (mrs_real samples);
  mrs_real seconds2Samples (mrs_real seconds);
  mrs_realvec samples2Seconds (mrs_realvec samples);
  mrs_realvec seconds2Samples (mrs_realvec seconds);

  // parameters
  mrs_real	maxDelayLengthInSamples_,
            singleDelayInSamples_;

  //!< the actual delay line
  mrs_realvec	buffer_;


  mrs_realvec delayInSamples_,		//!< the current (multiple) delays
              prevDelayInSamples_,	//!< the previous (multiple) delays
              ctrlIncrement_;			//!< only used in the process function for smoothing delay transitions

  mrs_natural writeCursor_,	//!< position of write head
              cursorMask_;	//!< bit mask for length of buffer

public:
  Delay(std::string name);
  ~Delay();
  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);

protected:
  Delay(const Delay& a);

};

}//namespace Marsyas

#endif
