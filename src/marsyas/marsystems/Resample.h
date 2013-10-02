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

#ifndef MARSYAS_Resample_H
#define MARSYAS_Resample_H

#include <marsyas/system/MarSystem.h>


#include "ResampleLinear.h"
#include "ResampleBezier.h"
#include "ResampleSinc.h"
#include "ResampleNearestNeighbour.h"


namespace Marsyas
{
/**
	\ingroup Processing
	\brief Resampling in linear, bezier, nearest-neighbour, or sinc mode.

	This MarSystem internally instantiates another MarSystem that does the actual work, depending
	on the 'resamplingMode' control.

	Controls:
	- **mrs_string/resamplingMode**: mode of operation:
		- \b "linear" = Use ResampleLinear.
		- \b "bezier" = Use ResampleBezier.
		- \b "near" = Use ResampleNearestNeighbour.
		- \b "sincip" = Use ResampleSinc. [Why "sincip", and not just "sinc"? Typo?]
	- **mrs_real/stretch**: Output/input sampling rate ratio
		(number of output samples = number of input samples * stretch).
	- **mrs_real/offStart**: (default:0) Offset in samples from the beginning of the input vector
		(if only a part of the input vector should be used).
	- **mrs_real/offEnd**: (default:0) Offset in samples from the end of the input vector
		(if only a part of the samples should be used to interpolate)
	- **mrs_bool/option**:
		An additional on/off option depending on 'resamplingMode':
		- in "bezier" mode: equivalent to 'tangentMode' control of ResampleBezier.
		- in "sinc" mode: equivalent to 'windowedMode' control of ResampleSinc.
	- **mrs_bool/samplingRateAdjustmentMode**: (default: true)
		Whether to set the 'osrate' control to the target sampling rate, or just pass on the input
		sampling rate.
*/

class marsyas_EXPORT Resample: public MarSystem
{





private:
  //Add specific controls needed by this MarSystem.
  void addControls();
  MarControlPtr ctrl_option_;
  MarControlPtr ctrl_samplingRateAdjustmentMode_;
  MarControlPtr ctrl_offStart_;
  MarControlPtr ctrl_offEnd_;
  MarControlPtr ctrl_newSamplingRate_;
  MarControlPtr ctrl_resamplingMode_;
  MarSystem* interpolator_;
  ResampleBezier* resabez_;
  ResampleLinear* resalin_;
  ResampleSinc* resasinc_;
  //mrs_string resaModeOld_;
  void myUpdate(MarControlPtr sender);


public:
  Resample(std::string name);
  Resample(const Resample& a);
  ~Resample();
  MarSystem* clone() const;
  //MarSystem* Resample::getInterpolator() const;

  void myProcess(realvec& in, realvec& out);


};

}//namespace Marsyas

#endif
