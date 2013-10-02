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

#ifndef MARSYAS_ONSETTIMES_H
#define MARSYAS_ONSETTIMES_H

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
    \class OnsetTimes
	\ingroup Processing Basic
    \brief Retrieves M first beat times (phases, in "frames"-ticks), retrieved from
	peaks from an onset function.

	Input: Supposed to have PeakerOnsets, or similar, as input.
	Output: [x|Phasei|x|Phasei+1|  ...  |x|PhaseM]

	- \b mrs_natural/n1stOnsets [w] : nr. of first detected onsets (= M phases).
	- \b mrs_natural/nPeriods [w] : nr. of considered period hypotheses (N) - to avoid Fanout crash (requires equal number of columns in each line)!
	- \b mrs_natural/lookAheadSamples [w] : nr. of frames to adjust the lookahead used in PeakerOnset.
	- \b mrs_natural/tickCount [r] : current considered time (tick count) updated from BeatReferee.
*/


class OnsetTimes: public MarSystem
{
private:
  //Add specific controls needed by this MarSystem.
  void addControls();
  MarControlPtr ctrl_n1stOnsets_;
  MarControlPtr ctrl_lookAheadSamples_;
  MarControlPtr ctrl_nPeriods_;
  MarControlPtr ctrl_tickCount_;
  MarControlPtr ctrl_inductionTime_;
  MarControlPtr ctrl_accSize_;
  MarControlPtr ctrl_triggerInduction_;

  mrs_natural maxCount_;
  mrs_realvec phasesRaw_;
  mrs_natural size_;
  mrs_bool triggerInduction_;
  mrs_natural lastInductionTime_;
  mrs_natural accSize_;
  mrs_natural inductionSize_;
  mrs_natural nPeriods_;
  mrs_natural lookAhead_;
  mrs_natural acc_;
  mrs_natural n_;
  mrs_natural timeElapsed_;
  mrs_natural count_;

  void myUpdate(MarControlPtr sender);
  void delSurpassedOnsets();

public:
  OnsetTimes(std::string name);
  OnsetTimes(const OnsetTimes& a);
  ~OnsetTimes();
  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);
};

}//namespace Marsyas

#endif
