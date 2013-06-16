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

#include "MarSystem.h"	

namespace Marsyas
{
/** 
	\class ResampleSinc
	\ingroup Processing Basic
	\brief resamples all observations using a sum of sinc functions 
	wheareas each point is modeled by a sinc function with the given points amplitude, that is set to zero after the 5th zero crossing

	Normally this Marsystem expects as input samples witch are to be interpolated. 
	Multiple observations will be interpolated independently.

	Controls:
	mrs_bool ctrl_windowedMode	-	false:	apply sinc interpolation directly
									true:	(default) use values of Kaiser Window and interpolate linearly between them if necessary
	mrs_bool ctrl_samplingRateAdjustmentMode - adjust new resulting SamplingRate for following Marsystems
	mrs_real stretch - desired stretch ratio (number of output samples = input number of samples*stretch)
	mrs_real offStart - (default:0) offset from the start (towards the end) of the Samples (if only a part of the samples should be used to interpolate)
	mrs_real offEnd - (default:0) offset from the end (towards the start) of the Samples (if only a part of the samples should be used to interpolate)
	

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
