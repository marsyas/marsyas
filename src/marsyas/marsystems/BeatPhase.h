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

#ifndef MARSYAS_BEATPHASE_H
#define MARSYAS_BEATPHASE_H

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
    \class BeatPhase
    \ingroup Processing Analysis
    \brief Given a periodicity calculate best matching phase

*/


class BeatPhase: public MarSystem
{
private:
  //Add specific controls needed by this MarSystem.
  void addControls();
  MarControlPtr ctrl_tempos_;
  MarControlPtr ctrl_tempo_candidates_;

  MarControlPtr ctrl_temposcores_;

  MarControlPtr ctrl_phase_tempo_;
  MarControlPtr ctrl_ground_truth_tempo_;

  MarControlPtr ctrl_beats_;
  MarControlPtr ctrl_bhopSize_;
  MarControlPtr ctrl_bwinSize_;
  MarControlPtr ctrl_timeDomain_;
  MarControlPtr ctrl_beatOutput_;
  MarControlPtr ctrl_nCandidates_;
  MarControlPtr ctrl_factor_;


  mrs_natural pinSamples_;
  mrs_real current_beat_location_;


  void myUpdate(MarControlPtr sender);
  mrs_natural sampleCount_;
  mrs_real factor_;

public:
  BeatPhase(std::string name);
  BeatPhase(const BeatPhase& a);
  ~BeatPhase();
  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);
};

}//namespace Marsyas

#endif
