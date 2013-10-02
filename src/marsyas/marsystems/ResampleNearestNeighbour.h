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

#ifndef MARSYAS_RESAMPLENEARESTNEIGHBOUR_H
#define MARSYAS_RESAMPLENEARESTNEIGHBOUR_H

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
	\ingroup Processing
	\brief Stretch samples using the samples' nearest neighbours

	Controls:
	- **mrs_real/stretch**: Output/input sampling rate ratio
	(number of output samples = number of input samples * stretch).
	- **mrs_bool/samplingRateAdjustmentMode**: (default: true)
	Whether to set the 'osrate' control to the target sampling rate, or just pass on the input
	sampling rate.
*/

class ResampleNearestNeighbour: public MarSystem
{
private:
  void addControls();
  void myUpdate(MarControlPtr sender);

  MarControlPtr ctrl_stretch_;
  MarControlPtr ctrl_samplingRateAdjustmentMode_;

public:
  ResampleNearestNeighbour(std::string name);
  ResampleNearestNeighbour(const ResampleNearestNeighbour& a);
  ~ResampleNearestNeighbour();
  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);
};

}

#endif

