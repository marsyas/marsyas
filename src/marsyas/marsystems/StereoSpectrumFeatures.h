/*
** Copyright (C) 1998-2007 George Tzanetakis <gtzan@cs.uvic.ca>
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


#ifndef MARSYAS_STEREOSPECTRUMFEATURES_H
#define MARSYAS_STEREOSPECTRUMFEATURES_H

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
	\ingroup Analysis
	\brief Panning information from StereSpectrum output.

	After computing the StereoSpectrum we can summarize
	it by extracting features using the StereoSpectrumFeatures.
*/

class StereoSpectrumFeatures: public MarSystem
{
private:
  mrs_real m0_;
  mrs_real m1_;
  mrs_real audioBW_;
  mrs_natural low_;
  mrs_natural high_;

  void myUpdate(MarControlPtr sender);

public:
  StereoSpectrumFeatures(std::string name);
  ~StereoSpectrumFeatures();
  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);
};

}//namespace Marsyas

#endif






