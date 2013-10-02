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

#ifndef MARSYAS_PERESIDUAL_H
#define MARSYAS_PERESIDUAL_H

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
\ingroup Analysis
\brief Compute the Signal-to-Noise Ratio (input: obs1:noisy signal, obs2:original signal)
Controls:
- \b mrs_real/SNR [r] : result (SNR between observations)
- \b mrs_bool/snrInDb [rw] : flag whether the SNR output is in decibel or not (default: true)
- \b mrs_string/outFilePath [rw] : path to optional output text file (SNR between observations) (default: EMPTYSTRING)
*/


class PeakResidual: public MarSystem
{
private:
  MarControlPtr ctrl_SNR_ ;

  void addControls();
  void myUpdate(MarControlPtr sender);

  std::ofstream outFile_;
public:
  PeakResidual(std::string name);
  PeakResidual(const PeakResidual& a);
  ~PeakResidual();
  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);
};

}//namespace Marsyas

#endif
