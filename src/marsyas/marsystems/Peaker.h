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

#ifndef MARSYAS_PEAKER_H
#define MARSYAS_PEAKER_H

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
\ingroup Analysis
\brief Pick peaks out of signal

Peaker is used to select peaks(or valleys) from the input vector. Various
parameters of the peak selection process can be adjusted.

Controls:
- \b mrs_real/peakSpacing [w] : expressed in percentage of total vector length and is how much spacing you allow between the peaks
- \b mrs_real/peakStrength [w] : threshold compared to the RMS of the vector (should be renamed to peakStrengthRelRms to keep naming consistent, but this might break existing systems)
- \b mrs_real/peakStrengthAbs [w] : absolute threshold (might make sense e.g. for normalized spectra)
- \b mrs_real/peakStrengthRelMax [w] : threshold compared to global max of the vector (range: 0...1, example: 0.001 if all peaks with a level of more than 60dB below the maximum should be discarded
- \b mrs_real/peakStrengthRelThresh [w] : threshold compared to an adaptive threshold of the vector (lp filtered version of the signal); value is a factor that moves threshold up and down, e.g. value = 2 means that the lp filtered signal is moved up by 6dB
- \b mrs_real/peakStrengthTreshLpParam [w] : coefficient for the single pole lowpass for computing the adaptive threshold (between 0...1)
- \b mrs_natural/peakStart [w] : expressed in absolute positions of the vector and it just to adjust what part of the vector will be considered
- \b mrs_natural/peakEnd [w] : expressed in absolute positions of the vector length and it just to adjust what part of the vector will be considered
- \b mrs_natural/interpolation [w] : TODO - mlagrange? [!]
*/
// - \b mrs_real/peakGain [w] : TODO - mlagrange? [!]

class Peaker: public MarSystem
{
private:
  mrs_real rms_,
           max_,
           lpCoeff_,
           currThresh_;

  mrs_realvec lpThresh_;

  void addControls();
  void myUpdate(MarControlPtr sender);

  mrs_bool doThresholding (mrs_real input, mrs_real peakStrengthRelRms, mrs_real peakStrengthRelMax, mrs_real peakStrengthRelThresh, mrs_real peakStrengthAbs);
  void compLpThresh (const mrs_realvec input, mrs_realvec &output);

  mrs_natural twice_;
  mrs_natural half_;
  mrs_natural triple_;
  mrs_natural third_;

public:
  Peaker(std::string name);
  ~Peaker();
  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);
};

}//namespace Marsyas

#endif


