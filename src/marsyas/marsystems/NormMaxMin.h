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

#ifndef MARSYAS_NORMMINMAX_H
#define MARSYAS_NORMMINMAX_H

#include <marsyas/system/MarSystem.h>

#include <cfloat>

namespace Marsyas
{
/**

	\class NormMaxMin
	\ingroup Processing
	\brief Normalize by mapping min/max range to user specified range.

	You can normalize over the entire slice, within an entire
	observation channel, or over a sample by using the "domain" control.

	You can do the normalization all at once by setting the "mode"
	control to twopass. Alternatively you can train the normalizer
	with several sets of data by setting the mode control to "train",
	and then switch to "predict" to normalize a slice to the maximums
	and minimums found in the training slices.

	Controls:
	- \b mrs_real/lower [] : The lower bound you want to normalize the data to (default 0.0)
	- \b mrs_real/upper [] : The upper bound you want to normalize the data to (default 1.0)
	- \b mrs_realvec/maximums [] : The current maximum values for each observation or sample
	- \b mrs_realvec/minimums [] : The current minimum values for each observation or sample
	- \b mrs_string/mode [] : Are we training, predicting or in twopass mode?  (train, predict, twopass) (default "train")
	- \b mrs_string/domain [] : Are we normalizing over observations, samples or entire slices? (observations, samples, slices) (default "observations")
	- \b mrs_bool/ignoreLast [] : Should we ignore the last observation or sample?
	- \b mrs_bool/init [] :

*/


class marsyas_EXPORT NormMaxMin: public MarSystem
{
private:
  mrs_real lower_;
  mrs_real upper_;
  mrs_real range_;
  bool train_;
  bool init_;
  mrs_string mode_;
  mrs_string prev_mode_;
  mrs_string domain_;

  void addControls();
  void myUpdate(MarControlPtr sender);

  realvec maximums_;
  realvec minimums_;

  MarControlPtr lowerPtr_;
  MarControlPtr upperPtr_;
  MarControlPtr trainPtr_;
  MarControlPtr initPtr_;
  MarControlPtr maximumsPtr_;
  MarControlPtr minimumsPtr_;

  void do_observations(realvec& in, realvec& out);
  void do_samples(realvec& in, realvec& out);
  void do_slices(realvec& in, realvec& out);

public:
  NormMaxMin(std::string name);
  NormMaxMin(const NormMaxMin&);
  ~NormMaxMin();
  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);
};

}//namespace Marsyas

#endif

