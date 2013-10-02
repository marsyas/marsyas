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

#ifndef MARSYAS_HWPSspectrum_H
#define MARSYAS_HWPSspectrum_H

#include <marsyas/system/MarSystem.h>
#include "Series.h"

namespace Marsyas
{
/**
	\class WHaSp
	\ingroup MarSystem
	\brief Calculate Wrapped Harmonically Spectrum (WHaSp)

	- \b mrs_natural/histSize [w] : set the discretization size when comparing HWPS spectrums (i.e. the histogram nr of bins).
	- \b mrs_natural/totalNumPeaks [w] : this control sets the total num of peaks at the input (should normally be linked with PeakConvert similar control)
	- \b mrs_natural/frameMaxNumPeaks [w] : this control sets the maximum num of peaks per frame at the input (should normally be linked with PeakConvert similar control)
*/


class WHaSp: public MarSystem
{
private:
  Series* HWPSnet_;
  realvec simMatrix_;

  MarControlPtr ctrl_histSize_;
  MarControlPtr ctrl_totalNumPeaks_;
  MarControlPtr ctrl_frameMaxNumPeaks_;

  void createSimMatrixNet();
  void addControls();
  void myUpdate(MarControlPtr sender);

public:
  WHaSp(std::string name);
  WHaSp(const WHaSp& a);
  ~WHaSp();
  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);
};

}//namespace Marsyas

#endif
