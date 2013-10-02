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

#ifndef MARSYAS_CROSSCORRELATION_H
#define MARSYAS_CROSSCORRELATION_H

#include <marsyas/system/MarSystem.h>
#include <marsyas/fft.h>

namespace Marsyas
{
/**
	\ingroup Analysis
	\brief Cross correlation of input observations.

	For N input observations outputs N-1 observations, with each output observation
	being the cross correlation of the input observations n and n-1.

	Controls:
	- \b "mrs_string/mode" [w]: Sets the type of cross correlation.
		- "general"  is the default, and will compute the generalized cross correlation.
		- "phat" will compute the generalized cross correlation with phase transform
		- "ml" will compute a maximum likelihood cross correlation.  This works well
		specifically for time delay estimation in noisy or reverberant environments.

	Note: inSamples should be 2^k or FFT will not work properly.

	\author Gabrielle Odowichuk
*/


class CrossCorrelation: public MarSystem
{
private:
  fft *myfft_;
  realvec scratch_;
  realvec scratch1_;
  realvec scratch2_;
  realvec sub_scratch1_;
  realvec sub_scratch2_;
  mrs_string mode_;
  MarControlPtr ctrl_mode_;
  virtual void addControls();
  void myUpdate(MarControlPtr sender);

public:
  CrossCorrelation(std::string name);
  ~CrossCorrelation();
  CrossCorrelation(const CrossCorrelation& a);

  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);
};

}//namespace Marsyas

#endif



