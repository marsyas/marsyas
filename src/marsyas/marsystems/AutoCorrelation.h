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

#ifndef MARSYAS_AUTOCORRELATION_H
#define MARSYAS_AUTOCORRELATION_H

#include <marsyas/system/MarSystem.h>
#include <marsyas/fft.h>

namespace Marsyas
{
/**
  \ingroup Analysis
  \brief Generalized autocorrelation

  Computes the generalized autocorrelation of the input window
  (with optional magnitude compression) using the
  Fast Fourier Transform (FFT).

  Controls:
  - \b mrs_bool/aliasedOutput [w] : (default: false)
    - \b true: The output will be an alised time domain signal
    (as used by the original Marsyas0.1 code implemented by George Tzanetakis).
    - \b false: The FFTs will be computed using the next power of 2
    of the inSamples*2+1, which avoids alising in the iFFT step. In this mode,
    only the first inSamples of the autocorrleation function will be output
    (since the remaining ones are mirrored versions or zero valued).

  - \b mrs_bool/setr0to1" [w]:
  If set to true, the output will be normalized so that r_xx(0) = 1.
  Had to use a weird name because there already was a normalize control [AL]
*/

class AutoCorrelation: public MarSystem
{
private:
  fft *myfft_;
  realvec scratch_;

  realvec norm_;
  mrs_natural normalize_;
  mrs_real octaveCost_;
  mrs_real octaveMax_;
  mrs_real voicing_;
  mrs_natural fftSize_;
  mrs_real re_,im_,am_, k_;
  mrs_natural lowSamples_;
  mrs_natural numSamples_;

  MarControlPtr ctrl_magcompress_;
  MarControlPtr	ctrl_normalize_;
  MarControlPtr	ctrl_octaveCost_;
  MarControlPtr ctrl_voicingThreshold_;
  MarControlPtr ctrl_aliasedOutput_;
  MarControlPtr ctrl_makePositive_;
  MarControlPtr ctrl_setr0to1_;
  MarControlPtr ctrl_setr0to0_;
  MarControlPtr ctrl_lowCutoff_;
  MarControlPtr ctrl_highCutoff_;


  virtual void addControls();
  void myUpdate(MarControlPtr sender);

public:
  AutoCorrelation(std::string name);
  ~AutoCorrelation();
  AutoCorrelation(const AutoCorrelation& a);

  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);
};

}//namespace Marsyas

#endif


