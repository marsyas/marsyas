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

#ifndef MARSYAS_ResampleSinc_H
#define MARSYAS_ResampleSinc_H

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
	\ingroup Processing
	\brief Resamples each observation using a sum of sinc functions

	Each point is modeled by a sinc function with the given points amplitude,
	that is set to zero after the 5th zero crossing. [eh???]

	Controls:
	- **mrs_real/stretch**: Output/input sampling rate ratio
	(number of output samples = number of input samples * stretch).
	- **mrs_real/offStart**: (default:0) Offset in samples from the beginning of the input vector
	(if only a part of the input vector should be used).
	- **mrs_real/offEnd**: (default:0) Offset in samples from the end of the input vector
	(if only a part of the samples should be used to interpolate)
	- **mrs_bool/windowedMode**:
		- false: apply sinc interpolation directly
		- true: (default) use values of Kaiser Window and interpolate linearly between them if necessary
	- **mrs_bool/samplingRateAdjustmentMode**: (default: true)
	Whether to set the 'osrate' control to the target sampling rate, or just pass on the input
	sampling rate.
*/

class ResampleSinc: public MarSystem
{





private:
  //Add specific controls needed by this MarSystem.
  void addControls();
  MarControlPtr ctrl_windowedMode_;
  MarControlPtr ctrl_samplingRateAdjustmentMode_;
  MarControlPtr ctrl_offStart_;
  MarControlPtr ctrl_offEnd_;
  MarControlPtr ctrl_stretch_;
  void myUpdate(MarControlPtr sender);
  mrs_real sinc(mrs_real t);
  mrs_real window(mrs_real position);

  realvec arrx_;

public:
  ResampleSinc(std::string name);
  ResampleSinc(const ResampleSinc& a);
  ~ResampleSinc();
  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);


};

}//namespace Marsyas

#endif
