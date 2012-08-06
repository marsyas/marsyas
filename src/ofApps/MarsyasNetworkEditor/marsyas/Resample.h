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

#include "MarSystem.h"


#include "ResampleLinear.h"
#include "ResampleBezier.h"
#include "ResampleSinc.h"
#include "ResampleNearestNeighbour.h"


namespace Marsyas
{
/** 
	\class Resample
	\ingroup Processing Basic
	\brief resamples all observations using a piecewise cubic bezier polynomial for interpolation between samples

	Normally this Marsystem expects as input samples witch are to be interpolated. 
	Multiple observations will be interpolated independently.

	Controls:
	mrs_bool ctrl_tangentMode	-	false:	(default)tangent at interpolation point is derived from the previous and immediately following sample; 
									true:	tangents at each interpolation point are parallel to the axis along the indices of the samples
											which alows for smooth transition at frame endings, 
											maximum amplitude will be limited to the maximum prior to interpolation
	mrs_bool ctrl_samplingRateAdjustmentMode - adjust new resulting SamplingRate for following Marsystems
	mrs_real stretch - desired stretch ratio (number of output samples = input number of samples*stretch)
	mrs_real offStart - (default:0) offset from the start (towards the end) of the Samples (if only a part of the samples should be used to interpolate)
	mrs_real offEnd - (default:0) offset from the end (towards the start) of the Samples (if only a part of the samples should be used to interpolate)
	

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
