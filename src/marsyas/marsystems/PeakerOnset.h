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

#ifndef MARSYAS_PEAKERONSET_H
#define MARSYAS_PEAKERONSET_H

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
	\class PeakerOnset
	\ingroup Analysis
	\brief Detects if input contains a onset point

	PeakerOnset is based on the onset peak picking algorithm presented in:

	Dixon, S. (2006). Onset detection revisited. In Proc. International Conference on Digital
	Audio Effects (DAFx), Montreal, Canada.

	It takes as input an onset function over time (i.e. a row vector with the times series
	of a onset function like flux) and evaluates a specific point for onset presence, as below:

	Input = [zzzzzzzzzXyyy]

	Point "X" at the input will be evaluated. The onsetWinSize control specifies a "look ahead" parameter,
	i.e. how many samples (represented above as "y") after "X" will be used for evaluating if it is an onset or not (i.e. look ahead samples).

	Controls:
	- \b mrs_natrual/lookAheadSamples [w]: specifies the "look ahead" nr of "samples" (the number of "y"s in the above example ) for the detection of an onset at the input.
	- \b mrs_real/threshold [w]: specifies threshold (in % of local mean) for onset detection
	- \b mrs_real/confidence [r]: outputs the confidence that point "X" is an onset (it will 0.0 if it is not detected as an onset)
	- \b mrs_bool/onsetDetected [r]: flags if an onset was detected on point "X".
*/

class PeakerOnset: public MarSystem
{
private:
  MarControlPtr ctrl_lookAheadSamples_;
  MarControlPtr ctrl_threshold_;
  MarControlPtr ctrl_onsetDetected_;
  MarControlPtr ctrl_confidence_;

  mrs_real prevValue_;
  mrs_natural t_;

  void addControls();
  void myUpdate(MarControlPtr sender);

public:
  PeakerOnset(std::string name);
  PeakerOnset(const PeakerOnset& a);
  ~PeakerOnset();
  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);
};

}

#endif

